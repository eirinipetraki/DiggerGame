#include <stdint.h>
#include <math.h>

#include "../Engine/ObjectDefinitions.h"
#include "../Objects/Terrain.h"
#include "../Common.h"
#include "Bot.h"

void Bot::move ( struct BoundingBox *target ) {
	uint8_t dirs[4];

	if(!lifes){
		draw_trans_sprite( terrainMap, darkenTerrain, BX->x_LT, BX->y_LT);
		terrain->notifyDeath(this);
		return;
	}

	if(!mvAnimationProgress){
		if(this->ClassID == ITEM_NOBBLIN_ID){
			if(collisionDetect(direction)){
				this->direction = selectDirection(target, dirs);
			}
		}
		else{
			this->direction = selectDirection(target, dirs);
		}
		animationFilmChange();
		if(!collisionDetect(direction))
			mvAnimationProgress = MOVE_ANIM_STEPS_REQ;
	}
}
/*
void Bot::move ( struct BoundingBox *target ) {
	uint8_t dirs[4];
	uint8_t selected = DIRECTION_LEFT;

	if(!lifes){
		draw_trans_sprite( terrainMap, darkenTerrain, BX->x_LT, BX->y_LT);
		terrain->notifyDeath(this);
		return;
	}

	if(!mvAnimationProgress){
		int16_t diff_x = target->x_LT - this->BX->x_LT;
		int16_t diff_y = target->y_LT - this->BX->y_LT;

		if(abs(diff_x) > abs(diff_y)){
			if(diff_x < 0){
				dirs[0] = DIRECTION_LEFT;
				dirs[2] = DIRECTION_RIGHT;
			}else{
				dirs[0] = DIRECTION_RIGHT;
				dirs[2] = DIRECTION_LEFT;
			}
			if(diff_y < 0){
				dirs[1] = DIRECTION_UP;
				dirs[3] = DIRECTION_DOWN;
			}else{
				dirs[1] = DIRECTION_DOWN;
				dirs[3] = DIRECTION_UP;
			}
		}
		else{
			if(diff_x < 0){
				dirs[1] = DIRECTION_LEFT;
				dirs[3] = DIRECTION_RIGHT;
			}else{
				dirs[1] = DIRECTION_RIGHT;
				dirs[3] = DIRECTION_LEFT;
			}
			if(diff_y < 0){
				dirs[0] = DIRECTION_UP;
				dirs[2] = DIRECTION_DOWN;
			}else{
				dirs[0] = DIRECTION_DOWN;
				dirs[2] = DIRECTION_UP;
			}
		}
		for(uint8_t i=0; i<4; i++){
			if(!collisionDetect(dirs[i])){
				selected = dirs[i];
				break;
			}
		}
		this->direction = selected;
		animationFilmChange();
		if(!collisionDetect(direction))
			mvAnimationProgress = MOVE_ANIM_STEPS_REQ;
	}
}
*/
uint8_t Bot::selectDirection ( struct BoundingBox *target, uint8_t dirs[4] ) {
	uint8_t selected = DIRECTION_LEFT;
	int16_t diff_x = target->x_LT - this->BX->x_LT;
	int16_t diff_y = target->y_LT - this->BX->y_LT;
	bool flag = false;

	if(abs(diff_x) > abs(diff_y)){
		if(diff_x < 0){
			dirs[0] = DIRECTION_LEFT;
			dirs[2] = DIRECTION_RIGHT;
		}else{
			dirs[0] = DIRECTION_RIGHT;
			dirs[2] = DIRECTION_LEFT;
		}
		if(diff_y < 0){
			dirs[1] = DIRECTION_UP;
			dirs[3] = DIRECTION_DOWN;
		}else{
			dirs[1] = DIRECTION_DOWN;
			dirs[3] = DIRECTION_UP;
		}
	}
	else{
		if(diff_x < 0){
			dirs[1] = DIRECTION_LEFT;
			dirs[3] = DIRECTION_RIGHT;
		}else{
			dirs[1] = DIRECTION_RIGHT;
			dirs[3] = DIRECTION_LEFT;
		}
		if(diff_y < 0){
			dirs[0] = DIRECTION_UP;
			dirs[2] = DIRECTION_DOWN;
		}else{
			dirs[0] = DIRECTION_DOWN;
			dirs[2] = DIRECTION_UP;
		}
	}

	for(uint8_t i=0; i<4; i++){
		if(!collisionDetect(dirs[i]) && !isOpposite(dirs[i])){
			selected = dirs[i];
			flag = true;
			break;
		}
	}
	for(uint8_t i=0 && !flag; i<4; i++){
		if(!collisionDetect(dirs[i])){
			selected = dirs[i];
			break;
		}
	}
	return selected;
}

bool Bot::isOpposite ( uint8_t direction) {
	switch(direction){
		case DIRECTION_LEFT :
			if(this->direction == DIRECTION_RIGHT)
				return true;
			break;
		case DIRECTION_RIGHT :
			if(this->direction == DIRECTION_LEFT)
				return true;
			break;
		case DIRECTION_UP :
			if(this->direction == DIRECTION_DOWN)
				return true;
			break;
		case DIRECTION_DOWN :
			if(this->direction == DIRECTION_UP)
				return true;
			break;
	}
	return false;
}

void Bot::die ( void ) {
	lifes = 0;
}

Bot::Bot ( Terrain *terrain, uint16_t ID ) : Character(terrain,ID) {}

Bot::~Bot ( ) {}
