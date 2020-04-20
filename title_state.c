#include "engine.h"
#include "os/os.h"
#include "os/win32/key_codes.h"
#include "stb_image.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

int title_sprite;
extern int heart_sprite;
extern int message_font;

extern int grass_sprite;
extern int flower_sprites[];

struct TitleStateData{
    struct RenderContext *context;
    struct FrameData *main_frame_data;

    int tick_accum;
};
static struct TitleStateData *state_data = 0;
static struct RenderContext *context = 0;

int init_sprite(const char* path, struct GameData* data);

static void init_title(struct GameData *data, void *argument, int parent_state)
{
    state_data = calloc(1,sizeof(struct TitleStateData));
    set_custom_data_pointer(state_data,data);
    state_data->context = calloc(1,sizeof(struct RenderContext));
    state_data->main_frame_data = frame_data_new();
    state_data->context->frame_data = state_data->main_frame_data;

    title_sprite = init_sprite("sprites/title", data);

    add_game_state(1, data, 0);
}

static void destroy_title(struct GameData *data)
{
    free(state_data->context);
    free(state_data->main_frame_data);
    free(state_data);
}

static int update_title(int ticks, struct InputState input_state,
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

    context->camera_2d = get_identity_matrix3();
    render_sprite_screen(title_sprite, 0.1f, 0.6f, context);

    float px = 0.3f;
    float py = 0.55f;
    struct Color col = { 0.87f, 0.7f, 0.7f };
    render_string_screen("Press any key to start", message_font, &px, &py, col, context);

    float scale = fabsf(sinf(anim_t*7.f))*0.1f + 0.4f;
    context->camera_2d = multiply_matrix3(
        multiply_matrix3(
			get_translation_matrix3(-0.1f, -0.1f),
			get_scale_matrix3(scale)
            ),
        get_translation_matrix3(0.65f, 0.55f)
        );
    render_sprite_screen(heart_sprite, 0.f, 0.f, context);

    context->camera_2d = get_identity_matrix3();
    render_sprite_screen_scaled(grass_sprite, 0.4f, 0.1f, 0.5f, context);
    render_sprite_screen_scaled(flower_sprites[1], 0.42f, 0.25f, 0.5f, context);

    if (input_state.num_keys_typed > 0) {
        switch_game_state(1, data);
    }



	return 0;//NOTE(Vidar): Return 1 to wait for input before calling update again (e.g. from a GUI)
}

struct GameState title_state = {
	update_title,
	init_title,
	destroy_title,
	0
};
