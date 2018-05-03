#include <stdint.h>
#include <allegro.h>
#include <vector>
#include <stack>
#include <map>

#include "../Engine/AnimationFilmHolder.h"
#include "../Engine/BitmapHolder.h"
#include "../Engine/SoundHolder.h"
#include "../Engine/TerrainAI.h"
#include "../Engine/Sprite.h"

#include "../Characters/Character.h"
#include "../Characters/Digger.h"

#include "../Objects/TerrainTile.h"
#include "../Objects/BonusItem.h"
#include "../Objects/Gold.h"

#include "../Common.h"

#ifndef __TERRAIN_H__
#define __TERRAIN_H__

class Game;

class Terrain {
private :
	Sprite *Grid[TERRAIN_GRID_HEIGHT][TERRAIN_GRID_WIDTH];
	std::map<uint16_t, TerrainAI *, uint16_comparator> TerrainObjects;
	std::map<uint16_t, Character *, uint16_comparator> Characters;
	std::map<uint16_t, Gold *, uint16_comparator> gravityAware;
	std::map<uint16_t, TerrainAI *, uint16_comparator> dynamicRecycleBin;
	std::stack<Character *> hiddenNobblins;
	std::stack<Character *> hiddenHobblins;
	std::stack<BonusItem *> hiddenLifes;
	std::stack<BonusItem *> hiddenSW;
	std::vector<Sprite *> staticRecycleBin;
	AnimationFilmHolder  *animFHolder;
	struct BoundingBox *terrainBX;
	BitmapHolder *BitmapBuffer;
	SoundHolder *SoundBuffer;
	Digger		 *digger;
	BITMAP		 *terrainBuffer;
	Game		 *game;
	uint16_t	 gameLevel;
	uint16_t	 progress;
	uint16_t	 GraphicsMode;
	uint16_t	 SizeX;
	uint16_t	 SizeY;
	uint16_t	 X;
	uint16_t	 Y;
	uint16_t	 diggerInitX;
	uint16_t	 diggerInitY;
	uint16_t	 botInitX;
	uint16_t	 botInitY;
	uint8_t		 HobblinCounter;
	uint8_t		 NobblinCounter;
	uint8_t		 nextDirection;
	bool		 lInitDynamic[8];
	bool		 schedConflictResolve;

public :
	Terrain (
			Game *game,
			uint16_t mode,
			uint16_t level,
			BitmapHolder *bitmapHolder,
			AnimationFilmHolder  *animationHolder,
			SoundHolder *soundHolder
	);

	Sprite **getSpriteAt ( uint16_t x, uint16_t y ) ;
	std::vector<BITMAP *> *getFilm ( uint16_t objectID, uint16_t filmID ) ;
	void getTerrainBounds ( uint16_t *x, uint16_t *y ) ;
	struct BoundingBox *getTerrainBX ( void ) ;
	BITMAP *getBackBuffer ( void ) ;

	uint16_t getTerrainProgress ( void ) ;
	void setLifes ( uint8_t lifes ) ;
	uint8_t getLifes ( void ) ;
	void setScore ( uint32_t score ) ;
	uint32_t getScore ( void ) ;
	void timeShift ( uint64_t offset ) ;
	void setCheatMode ( bool *cheat ) ;

	void notifyMove ( TerrainAI *caller, uint8_t direction ) ;
	void notifyConsume ( TerrainAI *caller ) ;
	void notifyDeath ( TerrainAI *caller ) ;
	void notifyDiggerReset ( void ) ;

	void purge ( void ) ;

	void scheduler ( void ) ;
	void getKeyboard ( uint8_t key ) ;
	void gravityCheck ( void ) ;
	void move ( void ) ;
	void animate ( void ) ;
	void render ( void ) ;

	~Terrain ( ) ;

private :
	bool initDynamicObject ( uint8_t objectType ) ;
	void preCacheBots ( uint8_t nobblins, uint8_t hobblins ) ;
	void initBackGround ( void ) ;
	void initTerrain ( void ) ;
	void printTerrain ( void ) ;
};

#endif /* __TERRAIN_H__ */
