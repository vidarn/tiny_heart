#include "engine.h"
#include "os/os.h"
#include "os/win32/key_codes.h"
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
};
static struct PlayStateData *state_data = 0;
static struct RenderContext *context = 0;

int grass_sprite = 0;
int hole_sprite = 0;
int player_sprite = 0;
int flower_sprites[3] = { 0 };
int dot_sprites[3] = { 0 };
int slot_sprites[3] = { 0 };
float tile_w = 0.f;
float tile_h = 0.f;

enum ObjectIds {
    R = 1,
    Y,
    G,
    P,
    H,
    E,
};

void load_level(int i) {
    struct Level lvl1 = {
        4, 4,
        0, 0, E, R,
        R, H, P, R,
        0, G, Y, E,
        0, E, G, E,
    };
    struct Level lvl2 = {
        6, 2,
        P, G, E, G, 0, H,
        0, 0, Y, R, 0, 0,
    };
    struct Level* lvls[] = {
        &lvl1, &lvl2,
    };
    int num_levels = sizeof(lvls) / sizeof(*lvls);
    i = i % num_levels;
    struct Level* lvl = lvls[i];
    state_data->level = calloc(1, sizeof(struct Level));
    memcpy(state_data->level, lvl, sizeof(struct Level));
    for (int y = 0; y < lvl->h; y++) {
        for (int x = 0; x < lvl->w; x++) {
            int obj = lvl->objects[x + y * lvl->w];
            if (obj == P) {
                state_data->player_x = x;
                state_data->player_y = y;
                state_data->player_target_x = x;
                state_data->player_target_y = y;
            }
        }
    }
    state_data->dots[0] = 3;
    state_data->dots[1] = 3;
    state_data->dots[2] = 3;
}

int init_sprite(const char* path, struct GameData* data) {
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

static void init_game(struct GameData *data, void *argument, int parent_state)
{
    state_data = calloc(1,sizeof(struct PlayStateData));
    set_custom_data_pointer(state_data,data);
    state_data->context = calloc(1,sizeof(struct RenderContext));
    state_data->main_frame_data = frame_data_new();
    state_data->context->frame_data = state_data->main_frame_data;
    
    load_level(0);
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

    if (os_is_key_down(KEY_UP)) {
        state_data->next_move = DIR_N;
    }
    if (os_is_key_down(KEY_DOWN)) {
        state_data->next_move = DIR_S;
    }
    if (os_is_key_down(KEY_LEFT)) {
        state_data->next_move = DIR_W;
    }
    if (os_is_key_down(KEY_RIGHT)) {
        state_data->next_move = DIR_E;
    }

    for (int i = 0; i < input_state.num_keys_typed; i++) {
        switch (input_state.keys_typed[i]) {
        case 'r':
        case 'R':
            load_level(state_data->current_level);
            break;
        }
    }


    if (state_data->move_t <= 0.f && state_data->next_move) {
        int dir = state_data->next_move;
        int tx = state_data->player_x + dir_x[dir];
        int ty = state_data->player_y + dir_y[dir];
        int obj = lvl->objects[tx + ty*w];
        if (obj != E && tx >= 0 && ty >=0 && tx < w && ty < h) {
			state_data->player_target_x = tx;
			state_data->player_target_y = ty;
			state_data->move_t = 1.f;
			lvl->objects[state_data->player_x + state_data->player_y*w] = 0;
        }
    }

    if (state_data->move_t > 0.f) {
		state_data->next_move = DIR_NONE;
        state_data->move_t -= 6.f*dt;
		if (state_data->move_t <= 0.f) {
			state_data->player_x = state_data->player_target_x;
			state_data->player_y = state_data->player_target_y;
			int obj = lvl->objects[state_data->player_x + state_data->player_y*w];
			lvl->objects[state_data->player_x + state_data->player_y*w] = P;
			state_data->move_t = 0.f;
            for (int i = 0; i < 3; i++) {
                state_data->dots[i]--;
            }
			switch (obj) {
			case H:
				load_level(++state_data->current_level);
                return 0;
			case R:
			case Y:
			case G:
				state_data->dots[obj - 1] = 3;
				break;
			}
            for (int i = 0; i < 3; i++) {
                if (state_data->dots[i] < 0) {
                    load_level(state_data->current_level);
                    return 0;
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
    context->view_3d = get_identity_matrix4();
    context->view_3d = get_identity_matrix4();

	{
		float py = tile_h * (float)(h - 1);
		for (int y = 0; y < h; y++) {
			float px = 0.f;
			for (int x = 0; x < w; x++) {
                int obj = lvl->objects[x + y * w];
                if(obj == H)
					render_sprite_screen(hole_sprite, px, py, context);
                else if(obj != E)
					render_sprite_screen(grass_sprite, px, py, context);
				px += tile_w;
			}
			py -= tile_h;
		}
	}

	{
        float py = tile_h * (float)(h - 1);
        for (int y = 0; y < h; y++) {
            float px = 0.f;
            for (int x = 0; x < w; x++) {
                int obj = lvl->objects[x + y * w];
                switch (obj) {
                case R:
                case Y:
                case G:
                    render_sprite_screen(flower_sprites[obj - 1], px + 0.06f, py + 0.3f, context);
                    break;
                }

                px += tile_w;
            }
            py -= tile_h;
        }
    }

    {
        float t = state_data->move_t;
        float px = (t * state_data->player_x + (1.f - t)*state_data->player_target_x)*tile_w;
        float py = (t * state_data->player_y + (1.f - t)*state_data->player_target_y)*tile_h;
        py -= tile_h * 0.2f * sinf(t * 3.14f);
		py = tile_h * (float)(h - 1) - py;
		render_sprite_screen(player_sprite, px - 0.03f, py + 0.2f, context);
    }

    context->camera_2d = get_identity_matrix3();

    float x_min, y_min, x_max, y_max;
    get_window_extents(&x_min, &x_max, &y_min, &y_max, data);
    for (int col = R; col <= G; col++) {
        float py = y_max - 0.1f;
        for (int i = 0; i < 3; i++) {
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


	return 0;//NOTE(Vidar): Return 1 to wait for input before calling update again (e.g. from a GUI)
}

struct GameState play_state = {
	update_game,
	init_game,
	destroy_game,
	0
};
