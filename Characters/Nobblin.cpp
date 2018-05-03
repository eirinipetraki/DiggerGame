#include <typeinfo>

#include "../Engine/ObjectDefinitions.h"

#include "../Objects/TerrainTile.h"
#include "../Objects/Gold.h"
#include "../Objects/Emerald.h"
#include "../Objects/BonusItem.h"
#include "../GUI/ControlTile.h"
#include "../Objects/Terrain.h"

#include "Character.h"
#include "Nobblin.h"
#include "Hobblin.h"
#include "Digger.h"
#include "Bot.h"

#include "../Common.h"

void Nobblin::animationFilmChange ( void ) {
	this->setFilm(
		terrain->getFilm(ITEM_NOBBLIN_ID, direction)
	);
}

bool Nobblin::collisionDetect ( uint8_t direction ) {
	uint16_t x , y;

	if(terrainBoundCollision(direction)){
		this->terrainCollision = true;
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
			y = this->BX->y_LT;
			for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
				tmp[i] = terrain->getSpriteAt(x , y);
				y += TERRAIN_PIXELS;
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
			x = this->BX->x_LT;
			for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
				tmp[i] = terrain->getSpriteAt(x , y);
				x += TERRAIN_PIXELS;
			}
			break;
	}

	for(uint8_t i=0; i<SUB_GRID_ELEMS_D; i++){
		if(*tmp[i] != NULL){
			uint8_t type = (*tmp[i])->getClassID();
			switch(type){
				case ITEM_GOLD_ID :
					if(!((Gold *)(*tmp[i]))->isPushable(this, this->direction))
						return true;
					break;
				case ITEM_SOLID_TERRAIN_ID :
				case ITEM_ROCK_ID :
				case ITEM_HOBBLIN_ID :
				case ITEM_NOBBLIN_ID :
				case ITEM_EMERALD_ID :
				case ITEM_RUBY_ID :
				case ITEM_SAPHIRE_ID :
				case ITEM_BONUS_LIFE_ID :
				case ITEM_BONUS_FATAL_ID :
					return true;
					break;
			}
		}
	}
	return false;
}

void Nobblin::handleCollision ( void ) {
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
						((Gold *)(*tmp[i]))->consume();
					else
						((Gold *)(*tmp[i]))->push(this, this->direction);
					break;
				case ITEM_DIGGER_ID :
					if(((Digger *)(*tmp[i]))->isFatal())
						die();
					else
						((Digger *)(*tmp[i]))->die();
					break;
			}
		}
	}
}

Nobblin::Nobblin ( Terrain *terrain, uint16_t ID )  : Bot(terrain,ID)
{
	this->direction = DIRECTION_LEFT;
	this->lifes = 1;
	this->refresh = true;
	this->animatorActive = true;
	this->terrainCollision = false;
}

Nobblin::~Nobblin ( ) { }
