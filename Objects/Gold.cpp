#include <typeinfo>
#include <stdint.h>

#include "../Engine/ObjectDefinitions.h"

#include "../Characters/Character.h"
#include "../Characters/Digger.h"

#include "../GUI/ControlTile.h"
#include "../Common.h"
#include "TerrainTile.h"
#include "Terrain.h"
#include "Gold.h"

Gold::Gold ( Terrain *terrain, uint16_t ID, uint16_t value )
: TerrainAI(terrain,ID)
{
	mvAnimationProgress = 0;
	this->value = value;
	isFalling = false;
	CD_Aware = true;
	packed = true;
	refresh = true;
	animatorActive = true;
	triggerFall = false;
}

Gold::~Gold ( ) { }

bool	 Gold::isPacked ( void ) {
	return packed;
}

uint16_t Gold::consume ( void ) {
	draw_sprite( terrainMap, consumeMask, BX->x_LT, BX->y_LT);
	play_sample(c_sound, 100, 128, 1000, 0);
	terrain->notifyConsume(this);
	return value;
}

void	 Gold::setDarkTerrainMask ( BITMAP *mask ) {
	this->darkenTerrain = mask;
}

void	 Gold::setConsumeMask ( BITMAP *mask ) {
	this->consumeMask = mask;
}

void Gold::setConsumeSound ( SAMPLE *sound ) {
	this->c_sound = sound;
}

void Gold::setUnpackSound ( SAMPLE *sound ) {
	this->s_sound = sound;
}

bool	 Gold::isPushable ( Character *character, uint16_t direction ) {
	uint16_t x = this->terrainSizeX + TERRAIN_PIXELS;
	uint16_t y = this->terrainSizeY + TERRAIN_PIXELS;

	if(!packed)
		return true;
	if(isFalling)
		return false;
	if(terrainBoundCollision(direction))
		return false;

	switch(direction){
	case DIRECTION_LEFT :
		x = this->BX->x_LT - TERRAIN_PIXELS;
		y = this->BX->y_LT;
		for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
			tmp[i] = terrain->getSpriteAt(x , y);
			y += TERRAIN_PIXELS;
		}
		break;
	case DIRECTION_RIGHT :
		x = this->BX->x_RB;
		y = this->BX->y_RB - TERRAIN_PIXELS;
		for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
			tmp[i] = terrain->getSpriteAt(x , y);
			y -= TERRAIN_PIXELS;
		}
		break;
	case DIRECTION_UP :
		return false;
	case DIRECTION_DOWN :
		return false;
	}

	if(x > this->terrainSizeX || y > this->terrainSizeY)
		return false;

	uint8_t charType = character->getClassID();
	if(charType == ITEM_DIGGER_ID){
		for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
			if(*tmp[i] != NULL) {
				uint8_t type = (*tmp[i])->getClassID();
				if(type != ITEM_SOLID_TERRAIN_ID)
					return false;
			}
		}
	}
	else{
		for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
			if(*tmp[i] != NULL){
				return false;
			}
		}
	}
	return true;
}

void	 Gold::push ( Character *character, uint16_t direction ) {
	this->direction = direction;
	mvAnimationProgress = MOVE_ANIM_STEPS_REQ;
}

void	Gold::animate ( void ) {
	if(isFalling)
		return;
	if(mvAnimationProgress){
		if(mvAnimationProgress == MOVE_ANIM_STEPS_REQ){
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
		}
		this->refresh = true;
		--mvAnimationProgress;
		if(!mvAnimationProgress && triggerFall){
			triggerFall = false;
			startFall();
		}
	}
	this->AnimatedSprite::animate();
}

void	 Gold::fall ( void ) {
	if(!isFalling)
		return;

	if(fallAnimationProgress){
		if(fallAnimationProgress == MOVE_ANIM_STEPS_REQ){
			if(terrainBoundCollision(DIRECTION_DOWN)){
				HandleCollision();
				return;
			}
			terrain->notifyMove(this, DIRECTION_DOWN);
		}
		this->BX->y_LT += MOVE_ANIM_STEP;
		this->BX->y_RB += MOVE_ANIM_STEP;
		this->refresh = true;
		--fallAnimationProgress;
	}
	else{
		if(CollisionDetect()){
			HandleCollision();
		}
		else
			fallAnimationProgress = MOVE_ANIM_STEPS_REQ;
	}
	this->AnimatedSprite::animate();
}

void	Gold::draw ( void ) {
	if(refresh){
		draw_trans_sprite( terrainMap, darkenTerrain, BX->x_LT, BX->y_LT);
		draw_trans_sprite( terrainMap, spriteBitmap, BX->x_LT, BX->y_LT);
	}
	refresh = false;
}

void	 Gold::startFall ( void ) {
	if(!packed || isFalling || !groundCollisionDetect())
		return;
	if(mvAnimationProgress){
		triggerFall = true;
		return;
	}

	fallAnimationProgress = MOVE_ANIM_STEPS_REQ;
	mvAnimationProgress = 0;
	this->direction = DIRECTION_DOWN;
	this->setFilm( terrain->getFilm(ITEM_GOLD_ID, ITEM_GOLD_FALLING_STATE) );
	startFallingY = BX->y_LT;
	isFalling = true;
}

bool	 Gold::groundCollisionDetect ( void ) {
	if(terrainBoundCollision(DIRECTION_DOWN)){
		return true;
	}
	uint16_t y = this->BX->y_RB;
	uint16_t x = this->BX->x_RB - TERRAIN_PIXELS;
	for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
		tmp[i] = terrain->getSpriteAt(x , y);
		x -= TERRAIN_PIXELS;
	}
	for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
		if(*tmp[i] != NULL){
			if((*tmp[i])->getClassID() != ITEM_GAP_ID){
				return false;
			}
		}
	}
	return true;
}

bool	 Gold::CollisionDetect ( void ) {
	if(terrainBoundCollision(DIRECTION_DOWN)){
		return true;
	}
	uint16_t y = this->BX->y_RB;
	uint16_t x = this->BX->x_RB - TERRAIN_PIXELS;
	for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
		tmp[i] = terrain->getSpriteAt(x , y);
		x -= TERRAIN_PIXELS;
	}
	for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
		if(*tmp[i] != NULL){
			if((*tmp[i])->getClassID() != ITEM_GAP_ID){
				return true;
			}
		}
	}
	return false;
}

void	 Gold::HandleCollision ( void  ) {
	fallAnimationProgress = 0;
	isFalling = false;
	if(BX->y_LT - startFallingY > 64){
		unPack();
	} else {
		this->setFilm( terrain->getFilm(ITEM_GOLD_ID, ITEM_GOLD_PACKED_STATE) );
	}
	for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
		if(*tmp[i] != NULL){
			uint8_t type = (*tmp[i])->getClassID();
			switch(type){
				case ITEM_DIGGER_ID :
				case ITEM_HOBBLIN_ID :
				case ITEM_NOBBLIN_ID :
					((Character *)*tmp[i])->die();
					break;
				default :
					break;
			}
		}
	}
}

void	 Gold::unPack ( void ) {
	packed = false;
	play_sample(s_sound, 100, 128, 1000, 0);
	this->setFilm( terrain->getFilm(ITEM_GOLD_ID, ITEM_GOLD_UNPACKED_STATE) );
}
