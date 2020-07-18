#include "engine.h"
#include "os/os.h"

int reference_resolution = 800;
float magnification = 1.f;

extern struct GameState title_state;
extern struct GameState play_state;

int main(int argc, char **argv){
	struct GameState game_states[] = {title_state, play_state};
	int res = reference_resolution * magnification;
	launch_game("Tiny Heart", res, res, 0, 2, 0, game_states, 0, 0);
	return 0;
}
