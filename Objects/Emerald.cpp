#include <stdint.h>
#include <allegro.h>
#include <math.h>

#include "../Engine/ObjectDefinitions.h"
#include "../Engine/TerrainAI.h"
#include "Terrain.h"
#include "Emerald.h"

#define RANDOM(MIN,MAX) (MIN+(uint16_t)((float)MAX*(rand()/(RAND_MAX+1.0))))

Emerald::Emerald ( Terrain *terrain, uint16_t ID, uint16_t value )
: TerrainAI(terrain,ID)
{
	animatorCurrent = RANDOM(ITEM_EMERALD_NUM_MIN, (ITEM_EMERALD_NUM_MAX-1));
	this->value = value;
}

void Emerald::setConsumeMask ( BITMAP *mask ) {
	this->consumeMask = mask;
}

void Emerald::setConsumeSound ( SAMPLE *sound ) {
	this->sound = sound;
}

uint16_t Emerald::consume ( void ) {
	draw_trans_sprite( terrainMap, consumeMask, BX->x_LT, BX->y_LT);
	play_sample(sound, 100, 128, 1000, 0);
	terrain->notifyConsume(this);
	return value;
}

Emerald::~Emerald ( ) { }
