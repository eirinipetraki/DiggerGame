#include <stdint.h>
#include <allegro.h>

#include "../Engine/Sprite.h"

#ifndef __TERRAIN_TILE_H__
#define __TERRAIN_TILE_H__

class TerrainTile : public Sprite {
private :
	bool	solid;
public :
	TerrainTile ( BITMAP *bitmap ) ;
	void setState ( bool solid ) ;
	bool isSolid ( void ) ;
	~TerrainTile ( ) ;
};

#endif	/*	__TERRAIN_TILE_H__	*/
