#include "Game.h"

int main()
{
	Game game;
	game.init();
	game.runLoop();
	game.shutdown();

	return 0;
}
