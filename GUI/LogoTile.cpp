#include <allegro.h>
#include <stdint.h>
#include <iostream>

#include "LogoTile.h"
#include "../Common.h"
#include "../GFX/gfx.h"

extern uint64_t currTime;

LogoTile::LogoTile ( uint16_t mode ) {
	GraphicsMode = mode;
	if(!getBannerResolution (mode, &BX->x_LT, &BX->y_LT, &BX->x_RB, &BX->y_RB)){
		std::cerr << "Unsupported Graphics Mode.\n";
		exit(-1);
	}
	if(!getTerrainResolution (mode, &X, &Y)){
		std::cerr << "Unsupported Graphics Mode.\n";
		exit(-1);
	}
	Y += 8;

	backBuffer = create_bitmap( BX->x_RB - BX->x_LT, BX->y_RB - BX->y_LT );
	clear_to_color(backBuffer, makecol(0, 0, 0));
	animationTimer = currTime;
	currAnimation = 0;
	refresh = true;
}

void LogoTile::setAuthor ( uint8_t index, BITMAP *image ) {
	Authors[index] = image;
}

void LogoTile::setScreenBuffer ( BITMAP *ScreenBuffer ) {
	this->screenBuffer = ScreenBuffer;
}

void LogoTile::draw ( void ) {
	if(refresh){
		draw_sprite( screenBuffer, backBuffer, X, Y);
		refresh = false;
	}
}

void LogoTile::animate ( void ) {
	if(currTime - animationTimer > 2500000 ){
		uint16_t x = (X - Authors[currAnimation]->w)/2;
		clear_to_color(backBuffer, makecol(0, 0, 0));
		draw_trans_sprite ( backBuffer, Authors[currAnimation], x, Y);
		currAnimation = ANIM_SUCCESSOR(currAnimation, 3);
		refresh = true;;
	}
}

LogoTile::~LogoTile( void ) {
	free(BX);
	destroy_bitmap(backBuffer);
}
