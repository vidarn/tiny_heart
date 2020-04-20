#include <Windows.h>
#include "engine.h"
#include "os/os.h"

int reference_resolution = 800;
float magnification = 1.f;

extern struct GameState play_state;

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	struct GameState game_states[] = { play_state};
	int res = reference_resolution * magnification;
	launch_game("Tiny Heart", res, res, 0, 1, 0, game_states, 0, 0);
	return ERROR_SUCCESS;
}
