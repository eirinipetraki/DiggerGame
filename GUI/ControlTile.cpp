#include <iostream>
#include <stdint.h>
#include <malloc.h>
#include <stdlib.h>
#include <allegro.h>

#include "ControlTile.h"
#include "../GFX/gfx.h"

void ControlTile::setScreenBuffer ( BITMAP *ScreenBuffer ) {
	this->screenBuffer = ScreenBuffer;
}

void ControlTile::setNumberBuffer ( uint8_t index, BITMAP *number ) {
	this->Numbers[index] = number;
}

void ControlTile::update ( uint32_t score, uint8_t lifes, uint8_t level ) {
	bool flags[3] = {false, false, false};
	if(this->lifes != lifes){
		this->lifes = lifes;
		refresh = true;
		flags[0] = true;
	}
	if(this->score != score){
		this->score = score;
		refresh = true;
		flags[1] = true;
	}
	if(this->level != level){
		this->level = level;
		refresh = true;
		flags[2] = true;
	}
	updateLifes(flags[0]);
	updateScore(flags[1]);
	updateLevel(flags[2]);
}

void ControlTile::draw ( void ) {
	if(refresh){
		draw_sprite( screenBuffer, backBuffer, X, Y);
		refresh = false;
	}
}

ControlTile::ControlTile (
		uint16_t mode,
		BITMAP *LevelBitmap,
		BITMAP *ScoreBitmap,
		BITMAP *LifeBitmap
) {
	this->LevelBitmap = LevelBitmap;
	this->ScoreBitmap = ScoreBitmap;
	this->LifeBitmap = LifeBitmap;

	BX = (struct BoundingBox *)memalign(CACHE_LINE, sizeof(struct BoundingBox));
	GraphicsMode = mode;
	if(!getBannerResolution (mode, &BX->x_LT, &BX->y_LT, &BX->x_RB, &BX->y_RB)){
		std::cerr << "Unsupported Graphics Mode.\n";
		exit(-1);
	}
	if(!getBannerPosition (mode, &X, &Y)){
		std::cerr << "Unsupported Graphics Mode.\n";
		exit(-1);
	}
	backBuffer = create_bitmap( BX->x_RB - BX->x_LT, BX->y_RB - BX->y_LT );
	clear_to_color(backBuffer, makecol(0, 0, 0));
	BlackTile = create_bitmap( 48, 48 );
	clear_to_color(BlackTile, makecol(0, 0, 0));
	offset = ((BX->x_RB - BX->x_LT)-736)/2;
	draw_trans_sprite ( backBuffer, ScoreBitmap, BX->x_LT, BX->y_LT+8) ;
	uint16_t level_str_pos = 128 + (5*48) + offset;
	draw_trans_sprite ( backBuffer, LevelBitmap, level_str_pos, BX->y_LT+8) ;
	levelPos = level_str_pos + 128 + (2*48);
}

ControlTile::~ControlTile ( ) {
	free(BX);
	destroy_bitmap(backBuffer);
	destroy_bitmap(BlackTile);
}

void ControlTile::updateLifes ( bool flag ) {
	if(!flag)
		return;

	uint16_t x_LT = BX->x_RB - 48;
	uint16_t y_LT = BX->y_LT + 8;
	for(uint8_t i=0; i<3; i++){
		draw_sprite ( backBuffer, BlackTile, x_LT, y_LT) ;
		x_LT -= 48;
	}
	x_LT = BX->x_RB - 48;
	for(uint8_t i=0; i<lifes && i<3; i++){
		draw_trans_sprite ( backBuffer, LifeBitmap, x_LT, y_LT) ;
		x_LT -= 48;
	}
}

void ControlTile::updateScore ( bool flag ) {
	if(!flag)
		return;

	uint32_t tmpScore = score;
	uint32_t index;
	uint16_t x_LT = BX->x_LT + 320;
	uint16_t y_LT = BX->y_LT + 8;
	for(uint8_t i=0; i<5; i++){
		draw_sprite ( backBuffer, BlackTile, x_LT, y_LT) ;
		x_LT -= 48;
	}
	x_LT = BX->x_LT + 320;
	if(!tmpScore){
		draw_trans_sprite ( backBuffer, Numbers[0], x_LT, y_LT) ;
	}
	for(uint8_t i=0; i<5 && tmpScore; i++){
		index = tmpScore % 10;
		tmpScore /= 10;
		draw_trans_sprite ( backBuffer, Numbers[index], x_LT, y_LT) ;
		x_LT -= 48;
	}
}

void ControlTile::updateLevel ( bool flag ) {
	if(!flag)
		return;

	uint32_t tmpLevel = level;
	uint32_t index;
	uint16_t x_LT = levelPos - 48;
	uint16_t y_LT = BX->y_LT + 8;
	for(uint8_t i=0; i<2; i++){
		draw_sprite ( backBuffer, BlackTile, x_LT, y_LT) ;
		x_LT -= 48;
	}
	x_LT = levelPos - 48;
	for(uint8_t i=0; i<2 && tmpLevel; i++){
		index = tmpLevel % 10;
		tmpLevel /= 10;
		draw_trans_sprite ( backBuffer, Numbers[index], x_LT, y_LT) ;
		x_LT -= 48;
	}
}
