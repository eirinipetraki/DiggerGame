#include "TerrainAI.h"
#include "../Objects/Terrain.h"

bool TerrainAI::terrainBoundCollision ( uint8_t direction ) {
	switch(direction){
		case DIRECTION_LEFT :
			if(BX->x_LT == 0)
				return true;
			break;
		case DIRECTION_RIGHT :
			if(BX->x_RB == terrainSizeX)
				return true;
			break;
		case DIRECTION_UP :
			if(BX->y_LT == 0)
				return true;
			break;
		case DIRECTION_DOWN :
			if(BX->y_RB == terrainSizeY)
				return true;
			break;
	}
	return false;
}

TerrainAI::TerrainAI ( Terrain *terrain, uint16_t ID ) {
	this->terrain = terrain;
	terrain->getTerrainBounds(&terrainSizeX, &terrainSizeY);
	this->ID = ID;
}

struct BoundingBox *TerrainAI::getCoords ( void ) {
	return BX;
}

uint16_t TerrainAI::getID ( void ) {
	return ID;
}

TerrainAI::~TerrainAI ( ) { }
