#include <stdint.h>
#include <string>

#include "AnimatedSprite.h"

class Terrain;

#ifndef __TERRAIN_AWARE_ITEM_H__
#define __TERRAIN_AWARE_ITEM_H__

class TerrainAI : public AnimatedSprite {
protected :
	Terrain		*terrain;
	uint16_t	terrainSizeX;
	uint16_t	terrainSizeY;
	uint16_t	ID;

public :
	TerrainAI ( Terrain *terrain, uint16_t ID ) ;
	struct BoundingBox *getCoords ( void ) ;
	uint16_t getID ( void ) ;
	~TerrainAI ( ) ;

protected :
	bool terrainBoundCollision ( uint8_t direction ) ;
};

#endif /* __TERRAIN_AWARE_ITEM_H__ */
