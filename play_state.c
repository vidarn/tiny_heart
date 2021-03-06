#include "engine.h"
#include "os/os.h"
#include "os/win32/key_codes.h"
#include "window/window.h"
#include "stb_image.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define MAX_LEVEL_SIZE 32
struct Level {
    int w, h;
    int objects[MAX_LEVEL_SIZE * MAX_LEVEL_SIZE];
};

enum Direction {
    DIR_NONE,
    DIR_N,
    DIR_E,
    DIR_S,
    DIR_W,
};

int dir_x[] = {
    0,0,1,0,-1
};

int dir_y[] = {
    0,-1,0,1,0
};

#define MAX_NUM_SNAILS 32
struct Snail {
    int awake;
    int dir;
    int px, py;
    int tx, ty;
    int anim_y;
};

struct PlayStateData{
    struct RenderContext *context;
    struct FrameData *main_frame_data;

    struct Level *level;
    int player_x, player_y;
    int player_target_x, player_target_y;
    float move_t;
    int next_move;

    int current_level;
    int dots[3];

    int num_snails;
    struct Snail snails[MAX_NUM_SNAILS];

    int message_i;
    float start_fade_t;
    float death_fade_t;
    float screen_anim_t;
    int end;

    int key_up;

    int tick_accum;
};
static struct PlayStateData *state_data = 0;
static struct RenderContext *context = 0;

const char* messages[] = {
    "You did it!\n Thanks to you, the friends\n are reunited at last.",
    "This little creature is Mimmi.\nShe has a tiny heart,\nbeating very fast.",
    "Mimmi's heart is aching,\nshe has lost her friend.",
    "Please help her by guiding her\nto the tunnel at the end\n of each level.",
    "Tiny creatures must eat often.\nIf she runs out of nutrients,\nmimmi will collapse.",
    "Make sure she eats flowers\nto keep her strength up.",
};
int num_messages = sizeof(messages) / sizeof(*messages);


int grass_sprite = 0;
int hole_sprite = 0;
int player_sprite = 0;
int flower_sprites[3] = { 0 };
int dot_sprites[3] = { 0 };
int slot_sprites[3] = { 0 };
int snail_sprites[2] = { 0 };
int buddy_sprite = 0;
int heart_sprite = 0;
int message_sprite = 0;
int message_font = 0;
float tile_w = 0.f;
float tile_h = 0.f;

enum ObjectIds {
    R = 1,
    Y,
    G,
    P,
    H,
    E,
    S,
    SNAIL,
    B,
};

int start_level = 0;

void load_level(int i) {
    struct Level lvl1 = {
        4, 4,
        P, 0, E, H,
        R, G, Y, R,
        0, G, Y, E,
        0, E, G, E,
    };
    struct Level lvl2 = {
        6, 3,
        P, G, E, G, E, H,
        R, 0, Y, R, G, 0,
        E, E, R, 0, E, E,
    };
    struct Level snaillvl = {
        6, 4,
        P, R, E, 0, G, H,
        Y, S, R, Y, G, R,
        E, Y, G, E, R, E,
        E, R, R, G, Y, E,
    };
    struct Level snail2lvl = {
        6, 6,
        G, 0, R, 0, G, H,
        0, Y, 0, R, Y, 0,
        S, R, R, Y, G, Y,
        0, G, E, G, E, G,
        S, Y, R, Y, G, R,
        0, P, 0, E, Y, G,
    };
    struct Level dotlvl = {
        6, 6,
        E, E, 0, 0, E, E,
        E, P, Y, Y, 0, E,
        R, 0, 0, Y, 0, G,
        0, 0, R, G, 0, 0,
        E, 0, 0, H, Y, E,
        E, E, 0, R, E, E,
    };
    struct Level ulvl = {
        6, 7,
        E, E, E, E, E, H,
        P, 0, E, E, G, 0,
        R, Y, 0, E, 0, R,
        G, R, E, E, Y, G,
        0, G, E, 0, R, Y,
        Y, 0, G, G, Y, 0,
        E, Y, R, 0, 0, E,
    };
    struct Level getting_started = {
        6, 4,
        P, Y, E, 0, E, E,
        Y, 0, R, 0, E, E,
        G, 0, R, Y, G, E,
        E, E, Y, R, 0, H,
    };
    struct Level intermediatelvl = {
        6, 7,
        E, 0, R, E, E, 0,
        P, R, Y, G, 0, G,
        0, 0, G, E, Y, R,
        0, R, E, 0, R, H,
        E, Y, E, Y, E, G,
        E, G, R, G, Y, 0,
        E, E, E, Y, R, R,
    };
    struct Level biglvl = {
        8, 8,
        P, Y, E, G, E, B, 0, 0,
        0, 0, G, R, R, 0, R, 0,
        R, G, E, Y, R, R, 0, R,
        Y, E, E, R, 0, G, E, Y,
        Y, G, Y, G, 0, Y, Y, R,
        E, R, E, R, R, E, E, G,
        E, E, E, E, Y, G, Y, 0,
        E, E, E, E, 0, G, R, E,
    };
    struct Level endlvl = {
        4,2,
        0,P,0,0,
        E,Y,B,0,
    };
    struct Level* lvls[] = {
        &lvl1,
        &lvl2, &dotlvl, &getting_started, &snaillvl, &ulvl, &snail2lvl, &intermediatelvl, &biglvl,
        &endlvl,
    };
    int num_levels = sizeof(lvls) / sizeof(*lvls);
    i = i % num_levels;
    struct Level* lvl = lvls[i];
    if (lvl == &endlvl) {
        state_data->message_i = 0;
        state_data->start_fade_t = 1.f;
        state_data->end = 1;
    }
    state_data->level = calloc(1, sizeof(struct Level));
    memcpy(state_data->level, lvl, sizeof(struct Level));
    state_data->num_snails = 0;
    for (int y = 0; y < lvl->h; y++) {
        for (int x = 0; x < lvl->w; x++) {
            int obj = lvl->objects[x + y * lvl->w];
            if (obj == P) {
                state_data->player_x = x;
                state_data->player_y = y;
                state_data->player_target_x = x;
                state_data->player_target_y = y;
            }
            if (obj == S) {
                int snail_i = state_data->num_snails++;
                state_data->snails[snail_i].awake = 0;
                state_data->snails[snail_i].dir = DIR_E;
                state_data->snails[snail_i].px = x;
                state_data->snails[snail_i].py = y;
                state_data->snails[snail_i].tx = x;
                state_data->snails[snail_i].ty = y;
            }
        }
    }
    state_data->dots[0] = 3;
    state_data->dots[1] = 3;
    state_data->dots[2] = 3;
}

int init_sprite(const char* path, struct GameData* data)
{
    FILE *fp = open_file(path, ".png", "rb", data);
    if (fp) {
        int w, h, c;
        uint8_t* pixels = stbi_load_from_file(fp, &w, &h, &c, 4);
        for (int i = 0; i < w * h; i++) {
            float r = (float)pixels[i * 4 + 0] / 255.f;
            float g = (float)pixels[i * 4 + 1] / 255.f;
            float b = (float)pixels[i * 4 + 2] / 255.f;
            float a = (float)pixels[i * 4 + 3] / 255.f;
            pixels[i * 4 + 0] = (r * a) * 255.f;
            pixels[i * 4 + 1] = (g * a) * 255.f;
            pixels[i * 4 + 2] = (b * a) * 255.f;
            pixels[i * 4 + 3] = (  a  ) * 255.f;
        }
        int ret = load_sprite_from_memory(w, h, pixels, data);
        free(pixels);
        fclose(fp);
        return ret;
    }
    return -1;
}

float tile_height(float t, float x, float y) {
    return 0.03f*sinf(t + 0.3f*x + y*0.3f);
}

static void init_game(struct GameData *data, void *argument, int parent_state)
{
    state_data = calloc(1,sizeof(struct PlayStateData));
    set_custom_data_pointer(state_data,data);
    state_data->context = calloc(1,sizeof(struct RenderContext));
    state_data->main_frame_data = frame_data_new();
    state_data->context->frame_data = state_data->main_frame_data;

    state_data->start_fade_t = 1.f;
    
    state_data->current_level = start_level;
    load_level(state_data->current_level);
    grass_sprite = init_sprite("sprites/grass", data);
    get_sprite_size(grass_sprite, &tile_w, &tile_h, data);
    tile_w *= 0.87;
    tile_h *= 0.55;
    hole_sprite = init_sprite("sprites/hole", data);
    player_sprite = init_sprite("sprites/player", data);
    flower_sprites[0] = init_sprite("sprites/red_flower", data);
    flower_sprites[1] = init_sprite("sprites/yellow_flower", data);
    flower_sprites[2] = init_sprite("sprites/green_flower", data);
    dot_sprites[0] = init_sprite("sprites/red_dot", data);
    dot_sprites[1] = init_sprite("sprites/yellow_dot", data);
    dot_sprites[2] = init_sprite("sprites/green_dot", data);
    slot_sprites[0] = init_sprite("sprites/red_slot", data);
    slot_sprites[1] = init_sprite("sprites/yellow_slot", data);
    slot_sprites[2] = init_sprite("sprites/green_slot", data);
    snail_sprites[0] = init_sprite("sprites/snail_shell", data);
    snail_sprites[1] = init_sprite("sprites/snail", data);
    buddy_sprite = init_sprite("sprites/buddy", data);
    heart_sprite = init_sprite("sprites/heart", data);

    message_sprite = init_sprite("sprites/message", data);
    message_font = load_font("fonts/BalooBhaina2-Regular", 100.f, data);

    state_data->message_i = 1;
}

static void destroy_game(struct GameData *data)
{
    free(state_data->context);
    free(state_data->main_frame_data);
    free(state_data);
}

static int update_game(int ticks, struct InputState input_state,
    struct GameData* data)
{
    state_data->tick_accum += ticks;
    float anim_t = (float)state_data->tick_accum / (float)TICKS_PER_SECOND;
    float dt = (float)ticks/(float)TICKS_PER_SECOND;
    state_data = get_custom_data_pointer(data);
    context = state_data->context;
    context->w = context->h = 1.0f;
    context->data = data;

    set_active_frame_data(state_data->main_frame_data, data);
    frame_data_reset(state_data->main_frame_data);
    struct Color clear_col = { 0.34f,0.34f,0.34f,0.f };
    frame_data_clear(state_data->main_frame_data, clear_col);

    struct Level* lvl = state_data->level;
    int w = lvl->w;
    int h = lvl->h;

    if (state_data->start_fade_t > 0.f) {
        if (state_data->message_i < num_messages) {
            if (input_state.num_keys_typed > 0 || input_state.mouse_state == MOUSE_CLICKED) {
                state_data->message_i++;
                if (state_data->message_i == 1) {
                    state_data->message_i = num_messages;
                }
            }
        }
        else {
            state_data->start_fade_t -= dt;
        }
    }
    else if (state_data->death_fade_t > 0.f) {
        state_data->death_fade_t -= dt;
    }
    else if (state_data->screen_anim_t > 0.f) {
        float prev = state_data->screen_anim_t;
        state_data->screen_anim_t -= dt*2.f;
        if (prev > 1.f && state_data->screen_anim_t < 1.f) {
			load_level(++state_data->current_level);
        }
    }
    else if(!state_data->end) {
        int key_up = 1;
        if (window_is_key_down(KEY_UP) || window_is_key_down('W') || window_is_key_down('K') || window_is_key_down('Z')) {
            if (state_data->key_up)
                state_data->next_move = DIR_N;
            key_up = 0;
        }
        if (window_is_key_down(KEY_DOWN) || window_is_key_down('S') || window_is_key_down('J')) {
            if (state_data->key_up)
                state_data->next_move = DIR_S;
            key_up = 0;
        }
        if (window_is_key_down(KEY_LEFT) || window_is_key_down('A') || window_is_key_down('H') || window_is_key_down('Q')) {
            if (state_data->key_up)
                state_data->next_move = DIR_W;
            key_up = 0;
        }
        if (window_is_key_down(KEY_RIGHT) || window_is_key_down('D')  || window_is_key_down('L')) {
            if (state_data->key_up)
                state_data->next_move = DIR_E;
            key_up = 0;
        }
        if (key_up) state_data->key_up = 1;

        for (int i = 0; i < input_state.num_keys_typed; i++) {
            switch (input_state.keys_typed[i]) {
            case 'r':
            case 'R':
                load_level(state_data->current_level);
                break;
            }
        }


        if (state_data->move_t <= 0.f && state_data->next_move && state_data->key_up) {
            int dir = state_data->next_move;
            int tx = state_data->player_x + dir_x[dir];
            int ty = state_data->player_y + dir_y[dir];
            int obj = lvl->objects[tx + ty * w];
            if (obj != E && obj != S && tx >= 0 && ty >= 0 && tx < w && ty < h) {
                state_data->key_up = 0;
                state_data->player_target_x = tx;
                state_data->player_target_y = ty;
                state_data->move_t = 1.f;
                lvl->objects[state_data->player_x + state_data->player_y * w] = 0;

                for (int i = 0; i < state_data->num_snails; i++) {
                    struct Snail* s = state_data->snails + i;
                    if (s->awake) {
                        int tx = s->px + dir_x[s->dir];
                        int ty = s->py + dir_y[s->dir];
                        int obj = lvl->objects[tx + ty * w];
                        if (obj != E && obj != S && obj != SNAIL && tx >= 0 && ty >= 0 && tx < w && ty < h) {
                            s->tx = tx;
                            s->ty = ty;
                            lvl->objects[s->px + s->py * w] = 0;
                        }
                    }
                    s->awake = 1;
                }
            }
        }

        if (state_data->move_t > 0.f) {
            state_data->next_move = DIR_NONE;
            state_data->move_t -= 6.f * dt;
            if (state_data->move_t <= 0.f) {
                state_data->player_x = state_data->player_target_x;
                state_data->player_y = state_data->player_target_y;
                int obj = lvl->objects[state_data->player_x + state_data->player_y * w];
                lvl->objects[state_data->player_x + state_data->player_y * w] = P;
                state_data->move_t = 0.f;
                for (int i = 0; i < 3; i++) {
                    state_data->dots[i]--;
                }
                int skip = 0;
                switch (obj) {
                case H:
                case B:
                    state_data->screen_anim_t = 2.f;
                    skip = 1;
                    break;
                case R:
                case Y:
                case G:
                    state_data->dots[obj - 1] = 3;
                    break;
                }
                
                if (!skip) {
                    for (int i = 0; i < 3; i++) {
                        if (state_data->dots[i] < 0) {
                            load_level(state_data->current_level);
                            state_data->death_fade_t = 1.f;
                            return 0;
                        }
                    }

                    for (int i = 0; i < state_data->num_snails; i++) {
                        struct Snail* s = state_data->snails + i;
                        s->px = s->tx;
                        s->py = s->ty;
                        lvl->objects[s->px + s->py * w] = SNAIL;
                    }
                }

            }
        }
    }



    float map_w = (float)w * tile_w;
    float map_h = (float)h * tile_h;
    float map_size = map_w > map_h ? map_w : map_h;
    float offset_x = 0.f;
    float offset_y = 0.f;
    if (map_w > map_h) {
        offset_y += (map_w - map_h - tile_h*0.5f) * 0.5f;
    }
    else {
        offset_x += (map_h - map_w - tile_w*0.5f) * 0.5f;
    }
    context->camera_2d =  multiply_matrix3(
        multiply_matrix3(
            get_translation_matrix3(0.1f,0.1f),
			get_scale_matrix3(0.8f)
		),
        multiply_matrix3(
            get_translation_matrix3(offset_x, offset_y),
			get_scale_matrix3(1.f / map_size)
		)
	);
    if (state_data->screen_anim_t > 1.f) {
        float t = 2.f - state_data->screen_anim_t;
        t *= t;
        context->camera_2d = multiply_matrix3(context->camera_2d, get_translation_matrix3(-t, 0.f));
    }
    else if (state_data->screen_anim_t > 0.f) {
        float t = state_data->screen_anim_t;
        t *= t;
        context->camera_2d = multiply_matrix3(context->camera_2d, get_translation_matrix3(t, 0.f));
    }

	{
		float py = tile_h * (float)(h - 1);
		for (int y = 0; y < h; y++) {
			float px = 0.f;
			for (int x = 0; x < w; x++) {
                int obj = lvl->objects[x + y * w];
                float pyy = py + tile_height(anim_t, px, py);
                if(obj == H)
					render_sprite_screen(hole_sprite, px, pyy, context);
                else if(obj != E)
					render_sprite_screen(grass_sprite, px, pyy, context);
				px += tile_w;
			}
			py -= tile_h;
		}
	}

    int player_y = state_data->move_t > 0.5f ? state_data->player_y : state_data->player_target_y;
    for (int i = 0; i < state_data->num_snails; i++) {
        struct Snail* snail = state_data->snails + i;
        snail->anim_y = state_data->move_t > 0.5f ? snail->py : snail->ty;
    }
	{
        float py = tile_h * (float)(h - 1);
        for (int y = 0; y < h; y++) {
            float px = 0.f;
            for (int x = 0; x < w; x++) {
                float pyy = py + tile_height(anim_t, px, py);
                int obj = lvl->objects[x + y * w];
                switch (obj) {
                case R:
                case Y:
                case G:
                    render_sprite_screen(flower_sprites[obj - 1], px + 0.06f, pyy + 0.3f, context);
                    break;
                case B:
                    render_sprite_screen(buddy_sprite, px + 0.06f, pyy + 0.2f, context);
                    break;
                }

                px += tile_w;
            }
			float t = state_data->move_t;
			for (int i = 0; i < state_data->num_snails; i++) {
				struct Snail* snail = state_data->snails + i;
                if (snail->anim_y == y) {
					float px = (t * (float)snail->px + (1.f - t)*(float)snail->tx)*tile_w;
					float py = (t * (float)snail->py + (1.f - t)*(float)snail->ty)*tile_h;
					py = tile_h * (float)(h - 1) - py;
					float pyy = py + tile_height(anim_t, px, py);
					render_sprite_screen(snail_sprites[snail->awake], px - 0.03f, pyy + 0.2f, context);
                }
			}
            if (player_y == y) {
				float px = (t * state_data->player_x + (1.f - t)*state_data->player_target_x)*tile_w;
				float py = (t * state_data->player_y + (1.f - t)*state_data->player_target_y)*tile_h;
				py -= tile_h * 0.2f * sinf(t * 3.14f);
				py = tile_h * (float)(h - 1) - py;
				float pyy = py + tile_height(anim_t, px, py);
				render_sprite_screen(player_sprite, px - 0.03f, pyy + 0.2f, context);
            }
            py -= tile_h;
        }
    }

    context->camera_2d = get_identity_matrix3();

    float x_min, y_min, x_max, y_max;
    window_get_extents(&x_min, &x_max, &y_min, &y_max, data);
    for (int col = R; col <= G; col++) {
        for (int i = 0; i < 3; i++) {
			float py = y_max - 0.1f;
            py += 0.003f*sinf((anim_t + (float)i*0.2f + (float)col*0.7f) * 3.3f);
			float px = i*0.08 + x_min;
            if (col == G) px = i*0.09f + x_max - 3.f*0.09f;
            if (col == Y) px = i*0.09f + 0.5f*(x_max + x_min) - 1.5f*0.09f;
            if (i >= state_data->dots[col - 1]) {
                render_sprite_screen(slot_sprites[col - 1], px, py, context);
            } else {
                render_sprite_screen(dot_sprites[col - 1], px, py, context);
            }
        }
    }

    if (state_data->start_fade_t > 0.f) {
        struct Color col = { 0.f,0.f,0.f,0.6f*state_data->start_fade_t };
        render_rect_fill_screen(x_min, y_min, x_max, y_max, col, context);
    }
    if (state_data->death_fade_t > 0.f) {
        struct Color col = { 0.f,0.f,0.f,1.f*state_data->death_fade_t };
        render_rect_fill_screen(x_min, y_min, x_max, y_max, col, context);
    }

    if (state_data->message_i < num_messages) {

        render_sprite_screen(message_sprite, 0.f, 0.4f, context);
        switch (state_data->message_i) {
        case 0:
            context->camera_2d = get_scale_matrix3(0.65f);
            render_sprite_screen(heart_sprite, 0.18f, 0.78f, context);
            break;
        case 1:
            context->camera_2d = get_scale_matrix3(0.4f);
            render_sprite_screen(player_sprite, 0.25f, 1.25f, context);
            break;
        case 2:
            context->camera_2d = get_scale_matrix3(0.4f);
            render_sprite_screen(buddy_sprite, 0.26f, 1.25f, context);
            break;
        case 3:
            context->camera_2d = get_scale_matrix3(0.3f);
            render_sprite_screen(hole_sprite, 0.45f, 1.65f, context);
            break;
        case 4:
            context->camera_2d = get_scale_matrix3(0.7f);
            render_sprite_screen(dot_sprites[0], 0.18f, 0.74f, context);
            render_sprite_screen(dot_sprites[1], 0.30f, 0.74f, context);
            render_sprite_screen(dot_sprites[2], 0.25f, 0.82f, context);
            break;
        case 5:
            context->camera_2d = get_scale_matrix3(0.6f);
            render_sprite_screen(flower_sprites[1], 0.23f, 0.85f, context);
            break;
        }
		context->camera_2d = get_scale_matrix3(1.2f);
        float py = 0.52f;
        const char* message = messages[state_data->message_i];
        const char* m = message;
        while (*m != 0) {
            float px = 0.27f;
            int n = 0;
            while (*m != 0 && *m != '\n') {
                m++;
                n++;
            }
            struct Color col = {0.f,0.f,0.f,1.f};
            render_string_screen_n(message, n, message_font, &px, &py, col, context);
            if(*m != 0) message = ++m;
            py -= 0.05f;
        }
    }


	return 0;//NOTE(Vidar): Return 1 to wait for input before calling update again (e.g. from a GUI)
}

struct GameState play_state = {
	update_game,
	init_game,
	destroy_game,
	0
};
