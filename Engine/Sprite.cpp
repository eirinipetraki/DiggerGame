#include <allegro.h>
#include <malloc.h>
#include "Sprite.h"

Sprite::Sprite ( ){
	BX = (struct BoundingBox *)memalign(CACHE_LINE, sizeof(struct BoundingBox));
}

void Sprite::setClassID ( uint8_t ID ) {
	this->ClassID = ID;
}

uint8_t Sprite::getClassID ( void ) {
	return this->ClassID;
}

void Sprite::set ( uint16_t x, uint16_t y, uint16_t dim ) {
	BX->x_LT = x;
	BX->y_LT = y;
	BX->x_RB = x + dim;
	BX->y_RB = y + dim;
}

void Sprite::registerMap ( BITMAP *newBitmap ) {
	terrainMap = newBitmap;
}

void Sprite::setBitmap ( BITMAP *newBitmap ) {
	spriteBitmap = newBitmap;
	refresh = true;
}

void Sprite::draw ( void ) {
	if(refresh){
		draw_sprite( terrainMap, spriteBitmap, BX->x_LT, BX->y_LT);
		refresh = false;
	}
}

bool Sprite::CD_Enabled ( void ) {
	return CD_Aware;
}

void Sprite::set_CD ( bool CD_Aware ) {
	this->CD_Aware = CD_Aware;
}

Sprite::~Sprite( ){
	free(BX);
}
