#include <allegro.h>
#include "TerrainTile.h"

void TerrainTile::setState ( bool solid ) {
	this->solid = solid;
}

bool TerrainTile::isSolid ( void ) {
	return solid;
}

TerrainTile::TerrainTile ( BITMAP *bitmap ) {
	this->setBitmap(bitmap);
}

TerrainTile::~TerrainTile( ) { }
