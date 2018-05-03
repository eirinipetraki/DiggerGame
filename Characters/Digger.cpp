#include <typeinfo>
#include "Digger.h"

#include "../Engine/ObjectDefinitions.h"

#include "../Objects/TerrainTile.h"
#include "../Objects/Gold.h"
#include "../Objects/Emerald.h"
#include "../Objects/BonusItem.h"
#include "../Objects/Terrain.h"

#include "../GUI/ControlTile.h"

#include "Character.h"

#include "../Common.h"

extern uint64_t currTime;

Digger::Digger ( Terrain *terrain, uint16_t ID ) : Character(terrain,ID)
{
	this->direction = DIRECTION_LEFT;
	this->lifes = DIGGER_LIFES;
	this->terrainCollision = false;
	this->animatorActive = true;
	this->refresh = true;
	this->fatal = false;
	this->score = 0;
}

void Digger::reset ( uint16_t x, uint16_t y, uint16_t dim, uint8_t direction ) {
	BX->x_LT = x;
	BX->y_LT = y;
	BX->x_RB = x + dim;
	BX->y_RB = y + dim;
	this->direction = direction;
	this->refresh = true;
	this->animatorActive = true;
	this->fatal = false;
	this->terrainCollision = false;
	mvAnimationProgress = 0;
	this->setFilm(
		terrain->getFilm(ITEM_DIGGER_ID, direction)
	);
}

void Digger::setLifes ( uint8_t lifes ) {
	this->lifes = lifes;
}

uint8_t Digger::getLifes ( void ) {
	return this->lifes;
}

void Digger::setScore ( uint32_t score ) {
	this->score = score;
}

uint32_t Digger::getScore ( void) {
	return this->score;
}

void Digger::setCheat ( void ) {
	fatal = true;
	if(lifes < 8)
		lifes++;
}

bool Digger::isFatal ( void ) {
	return fatal;
}

void Digger::setGrave (BITMAP *grave) {
	this->grave = grave;
}

void Digger::setDeathSound ( SAMPLE *sound ) {
	this->sound = sound;
}

void Digger::animationFilmChange( void ) {
	this->setFilm(
		terrain->getFilm(ITEM_DIGGER_ID, direction)
	);
}

void Digger::move (  uint8_t direction ) {
	if(lock)
		return;
	if(!mvAnimationProgress){
		this->direction = direction;
		animationFilmChange();
		if(!collisionDetect(direction))
			mvAnimationProgress = MOVE_ANIM_STEPS_REQ;
	}
}

bool Digger::collisionDetect ( uint8_t direction ) {
	uint16_t x , y;

	if(terrainBoundCollision(direction)){
		this->direction = direction;
		this->terrainCollision = true;
		animationFilmChange();
		return true;
	}
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
			y = this->BX->y_RB  - TERRAIN_PIXELS;
			for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
				tmp[i] = terrain->getSpriteAt(x , y);
				y -= TERRAIN_PIXELS;
			}
			break;
		case DIRECTION_UP :
			y = this->BX->y_LT - TERRAIN_PIXELS;
			x = this->BX->x_LT;
			for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
				tmp[i] = terrain->getSpriteAt(x , y);
				x += TERRAIN_PIXELS;
			}
			break;
		case DIRECTION_DOWN :
			y = this->BX->y_RB;
			x = this->BX->x_RB - TERRAIN_PIXELS;
			for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
				tmp[i] = terrain->getSpriteAt(x , y);
				x -= TERRAIN_PIXELS;
			}
			break;
	}

	for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
		if(*tmp[i] != NULL){
			uint8_t type = (*tmp[i])->getClassID();
			switch(type){
				case ITEM_ROCK_ID :
					return true;
					break;
				case ITEM_GOLD_ID :
					if(!((Gold *)(*tmp[i]))->isPushable(this, this->direction))
						return true;
					break;
			}
		}
	}
	return false;
}

void Digger::handleCollision ( void ) {
	if(terrainCollision == true){
		this->terrainCollision = false;
		return;
	}
	for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
		if(*tmp[i] != NULL){
			uint8_t type = (*tmp[i])->getClassID();
			switch(type){
				case ITEM_GOLD_ID :
					if(!((Gold *)(*tmp[i]))->isPacked())
						score += ((Gold *)(*tmp[i]))->consume();
					else
						((Gold *)(*tmp[i]))->push(this, this->direction);
					break;
				case ITEM_EMERALD_ID :
				case ITEM_RUBY_ID :
				case ITEM_SAPHIRE_ID :
					score += ((Emerald *)(*tmp[i]))->consume();
					break;
				case ITEM_BONUS_LIFE_ID :
					((BonusItem *)(*tmp[i]))->consume();
					++lifes;
					break;
				case ITEM_BONUS_FATAL_ID :
					((BonusItem *)(*tmp[i]))->consume();
					fatalPitch = currTime;
					fatal = true;
					break;
				case ITEM_HOBBLIN_ID :
				case ITEM_NOBBLIN_ID :
					if(fatal)
						((Character *)(*tmp[i]))->die();
					else
						die();
					break;
			}
		}
	}
}

void Digger::animate ( void ) {
	if(lock){
		if(currTime - this->lockTime > 1000000){
			lock = false;
		}
		return;
	}
	if(currTime - fatalPitch > 20000000){
		this->fatal = false;
	}
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

void Digger::die ( void ) {
	if(lock)
		return;
	--lifes;
	draw_trans_sprite( terrainMap, darkenTerrain, BX->x_LT, BX->y_LT);
	draw_trans_sprite( terrainMap, grave, BX->x_LT, BX->y_LT);
	play_sample(sound, 100, 128, 1000, 0);
	lock = true;
	this->lockTime = currTime;
	mvAnimationProgress = 0;
	if(!lifes) {
		terrain->notifyDeath(this);
	}
	else {
		terrain->notifyDiggerReset();
	}
}

Digger::~Digger ( ) { }
