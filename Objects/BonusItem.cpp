#include <stdint.h>
#include <assert.h>
#include "../Engine/TerrainAI.h"
#include "Terrain.h"
#include "BonusItem.h"

uint16_t BonusItem::consume ( void ) {
	draw_trans_sprite( terrainMap, consumeMask, BX->x_LT, BX->y_LT);
	terrain->notifyConsume(this);
	return type;
}

void BonusItem::draw ( void ) {
	if(refresh){
		draw_trans_sprite( terrainMap, consumeMask, BX->x_LT, BX->y_LT);
		draw_trans_sprite( terrainMap, spriteBitmap, BX->x_LT, BX->y_LT);
		refresh = false;
	}
}

void BonusItem::setConsumeMask ( BITMAP *mask ) {
	this->consumeMask = mask;
}

BonusItem::BonusItem ( Terrain *terrain, uint16_t ID, uint16_t type )
: TerrainAI(terrain,ID)
{
	assert(type <= ITEM_TYPE_FATAL);
	this->type = type;
}

BonusItem::~BonusItem ( ) { }
