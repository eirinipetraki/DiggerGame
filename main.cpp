#include <iostream>
#include <allegro.h>
#include "Engine/Game.h"

int main ( void ) {
	if (allegro_init() != 0){
		std::cerr << "Error Initializing allegro runtime" << std::endl;
		return (-1);
	}
	Game game(2);

	game.start();
	game.printStatistics();

	return 0;
}

END_OF_MAIN()
