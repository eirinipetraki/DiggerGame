#include <allegro.h>

#include "AnimationFilmHolder.h"
#include "BitmapHolder.h"
#include "SoundHolder.h"
#include "../Objects/Terrain.h"
#include "../GUI/ControlTile.h"
//#include "../GUI/LogoTile.h"
#include "../Common.h"

#ifndef __GAME_H__
#define __GAME_H__

class Game {
private :
	AnimationFilmHolder  *animFHolder;
	BitmapHolder *BitmapBuffer;
	SoundHolder *SoundBuffer;
	struct BoundingBox *terrainBX;
	ControlTile *controlGUI;
//	LogoTile    *logo;
	BITMAP  *backBuffer;
	Terrain *terrain;
	uint64_t loop_ticks;
	uint64_t screen_ticks;
	uint64_t game_loops;
	uint64_t screen_loops;
	uint32_t freq;
	uint32_t tmpScore;
	uint16_t GraphicsMode;
	uint8_t  tmpDiggerLifes;
	uint8_t  currLevel;
	bool	 overFlag;

public :
	Game ( uint16_t mode ) ;
	void start ( void ) ;
	void pause ( void ) ;
	void over ( void ) ;
	void printStatistics ( void ) ;
	~Game ( ) ;
private :
	void initAnimationFilms ( void ) ;
	void initBitMaps ( void ) ;
	void initSounds ( void ) ;
	void GameLoop ( void ) ;
	void GameOverLoop ( uint32_t flag ) ;
	void IntroLoop ( void ) ;
};

#endif /* __GAME_H__ */
