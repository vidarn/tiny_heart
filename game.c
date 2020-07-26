
#include "engine.h"
#include "os/os.h"
#include "window/window.h"


int reference_resolution = 800;
float magnification = 1.f;

extern struct GameState play_state;
extern struct GameState title_state;

void satin_main(void)
{
    struct GameState game_states[] = {title_state, play_state};
    launch_game("Tiny Heart", 800, 600, 0, sizeof(game_states)/sizeof(*game_states), 0, game_states, 0, 0);
}
