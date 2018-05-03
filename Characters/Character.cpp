#include <stdint.h>
#include <allegro.h>

#include "Character.h"
#include "../Common.h"
#include "../Engine/AnimatedSprite.h"
#include "../Objects/Terrain.h"

void Character::setPosition ( uint16_t x, uint16_t y ) {
	this->BX->x_LT = x;
	this->BX->y_LT = y;
	this->BX->x_RB = this->BX->x_LT + CHARACTER_PIXELS;
	this->BX->y_RB = this->BX->y_LT + CHARACTER_PIXELS;
}

void Character::setDirection ( uint8_t direction ) {
	this->direction = direction;
}

uint8_t Character::getDirection ( void ) {
	return this->direction;
}

void Character::setLifes ( uint8_t lifes ) {
	this->lifes = lifes;
}

void Character::setDarkTerrainMask ( BITMAP *mask ) {
	this->darkenTerrain = mask;
}

void Character::draw ( void ) {
	if(refresh){
		draw_trans_sprite( terrainMap, darkenTerrain, BX->x_LT, BX->y_LT);
		draw_trans_sprite( terrainMap, spriteBitmap, BX->x_LT, BX->y_LT);
	}
	refresh = false;
}

void Character::animate ( void ) {
	if(mvAnimationProgress){
		if(mvAnimationProgress == MOVE_ANIM_STEPS_REQ){
			handleCollision();
			terrain->notifyMove(this, direction);
		}
		switch(this->direction){
			case DIRECTION_LEFT :
				this->BX->x_LT -= MOVE_ANIM_STEP;
				this->BX->x_RB -= MOVE_ANIM_STEP;
				break;
			case DIRECTION_RIGHT :
				this->BX->x_LT += MOVE_ANIM_STEP;
				this->BX->x_RB += MOVE_ANIM_STEP;
				break;
			case DIRECTION_UP :
				this->BX->y_LT -= MOVE_ANIM_STEP;
				this->BX->y_RB -= MOVE_ANIM_STEP;
				break;
			case DIRECTION_DOWN :
				this->BX->y_LT += MOVE_ANIM_STEP;
				this->BX->y_RB += MOVE_ANIM_STEP;
				break;
		}
		refresh = true;
		--mvAnimationProgress;
	}
	this->AnimatedSprite::animate();
}

Character::Character ( Terrain *terrain, uint16_t ID ) : TerrainAI(terrain,ID) {
	this->animatorActive = true;
	this->mvAnimationProgress = 0;
	this->CD_Aware = true;
}

Character::~Character ( ) { }
