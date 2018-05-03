#include <iostream>
#include <typeinfo>
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>
#include <allegro.h>
#include <assert.h>
#include <time.h>
#include <vector>
#include <stack>
#include <string>
#include <map>

#include "../Engine/AnimationFilmHolder.h"
#include "../Engine/ObjectDefinitions.h"
#include "../Engine/BitmapHolder.h"
#include "../Engine/SoundHolder.h"
#include "../Engine/TerrainAI.h"
#include "../Engine/Sprite.h"
#include "../Engine/Game.h"
#include "../Engine/Timer.h"

#include "../GFX/LevelHandler.h"
#include "../GFX/gfx.h"

#include "../Characters/Character.h"
#include "../Characters/Nobblin.h"
#include "../Characters/Hobblin.h"
#include "../Characters/Digger.h"
#include "../Characters/Bot.h"

#include "TerrainTile.h"
#include "BonusItem.h"
#include "Emerald.h"
#include "Gold.h"

#include "../Common.h"
#include "Terrain.h"

using namespace std;

#define RANDOM(MIN,MAX) (MIN+(uint16_t)((float)MAX*(rand()/(RAND_MAX+1.0))))

uint64_t previousObjectTime;
uint64_t previousCharacterTime;
uint64_t gravityCheckTime;
uint64_t activeTerrainTime;
uint64_t pHSceduleTime;
uint64_t pNSceduleTime;
uint64_t pBLSceduleTime;
uint64_t pBFSceduleTime;
uint64_t lastCheatTime;
extern uint64_t currTime;

void Terrain::timeShift ( uint64_t offset ) {
	previousObjectTime += offset;
	previousCharacterTime += offset;
	gravityCheckTime += offset;
	activeTerrainTime += offset;
	pHSceduleTime += offset;
	pNSceduleTime += offset;
	pBLSceduleTime += offset;
	pBFSceduleTime += offset;
	lastCheatTime += offset;
}

Terrain::Terrain (
		Game *game,
		uint16_t mode,
		uint16_t level,
		BitmapHolder *bitmapHolder,
		AnimationFilmHolder  *animationHolder,
		SoundHolder *soundHolder
) {
	srand(time(NULL));

	this->GraphicsMode = mode;
	this->gameLevel = level;
	this->HobblinCounter = 1;
	this->NobblinCounter = 1;
	this->progress = 0;
	this->BitmapBuffer = bitmapHolder;
	this->animFHolder = animationHolder;
	this->SoundBuffer = soundHolder;
	schedConflictResolve = true;

	if(!getTerrainResolution(mode, &SizeX, &SizeY)){
		std::cerr << "Unsupported Graphics Mode.\n";
		exit(-1);
	}
	if(!getTerrainPosition(mode, &X, &Y)){
		std::cerr << "Unsupported Graphics Mode.\n";
		exit(-1);
	}

	terrainBuffer = create_bitmap( SizeX, SizeY );

	terrainBX = (struct BoundingBox *)memalign(CACHE_LINE, sizeof(struct BoundingBox));
	terrainBX->x_LT = X;
	terrainBX->y_LT = Y;
	terrainBX->x_RB = X + SizeX;
	terrainBX->y_RB = Y + SizeY;

	this->game = game;

	initBackGround();
	initTerrain();

	uint8_t nobblins;
	uint8_t hobblins;
	getBotsPreCached(gameLevel, &nobblins, &hobblins);
	preCacheBots ( nobblins, hobblins );
	purge();

	previousObjectTime = currTime;
	previousCharacterTime = currTime;
	gravityCheckTime = currTime;
	activeTerrainTime = currTime;
	pHSceduleTime = currTime;
	pNSceduleTime = currTime;
	pBLSceduleTime = currTime;
	pBFSceduleTime = currTime;

#if defined(ASSERTIONS_ENABLED)
	std::cerr << "TerrainObjects : " << TerrainObjects.size() << endl;
	std::cerr << "Characters : " << Characters.size() << endl;
	std::cerr << "GravityAware : " << gravityAware.size() << endl;
	std::cerr << "HiddenNobblins : " << hiddenNobblins.size() << endl;
	std::cerr << "HiddenHobblins : " << hiddenHobblins.size() << endl;
	std::cerr << "HiddenLifes : " << hiddenLifes.size() << endl;
	std::cerr << "HiddenSW : " << hiddenSW.size() << endl;
	std::cerr << "RecycleBin : " << staticRecycleBin.size() << endl;
	std::cerr << "AnimationFilmHolder : " << animFHolder->size() << endl;
	std::cerr << "BitmapBuffer : " << BitmapBuffer->size() << endl;
#endif
}

void Terrain::setLifes ( uint8_t lifes ) {
	digger->setLifes(lifes);
}

uint8_t Terrain::getLifes ( void ) {
	return digger->getLifes();
}

void Terrain::setScore ( uint32_t score ) {
	digger->setScore(score);
}

uint32_t Terrain::getScore ( void) {
	return digger->getScore();
}

void Terrain::setCheatMode ( bool *cheat ) {
	if(*cheat && (currTime-lastCheatTime) > 1000000){
		digger->setCheat();
		lastCheatTime = currTime;
		*cheat = false;
	}
}

BITMAP *Terrain::getBackBuffer ( void ) {
	return this->terrainBuffer;
}

struct BoundingBox *Terrain::getTerrainBX ( void ){
	return terrainBX;
}

Terrain::~Terrain ( ) {
	uint16_t x_16x16, y_16x16;

	getTerrainTiles_16x16(GraphicsMode, &x_16x16, &y_16x16 );

	for(uint16_t i=0,targetY=0; i<y_16x16; i++, targetY+=16){
		for(uint16_t j=0,targetX=0; j<x_16x16; j++,targetX+=16){
			if(Grid[i][j] != NULL){
				uint8_t type = Grid[i][j]->getClassID();
				switch(type){
					case  ITEM_SOLID_TERRAIN_ID :
						staticRecycleBin.push_back(Grid[i][j]);
						Grid[i][j] = NULL;
						break;
					case ITEM_ROCK_ID :
						staticRecycleBin.push_back(Grid[i][j]);
						for(uint8_t ii=0; ii<4; ii++){
							for(uint8_t jj=0; jj<4; jj++){
								Grid[i+ii][j+jj] = NULL;
							}
						}
						break;
					case ITEM_DIGGER_ID :
						if(dynamicRecycleBin.find(digger->getID()) != dynamicRecycleBin.end()){
							dynamicRecycleBin[digger->getID()] = digger;
						}
//						RecycleBin.push_back(digger);
//						delete digger;
						for(uint8_t ii=0; ii<4; ii++){
							for(uint8_t jj=0; jj<4; jj++){
								if(Grid[i+ii][j+jj] != NULL){
									if(digger->getClassID() == Grid[i+ii][j+jj]->getClassID())
										Grid[i+ii][j+jj] = NULL;
								}
							}
						}
						break;
					case ITEM_HOBBLIN_ID :
					case ITEM_NOBBLIN_ID :
						dynamic_cast<Character *>(Grid[i][j])->die();
						break;
					case ITEM_GOLD_ID :
						dynamic_cast<Gold *>(Grid[i][j])->consume();
						break;
					case ITEM_EMERALD_ID :
					case ITEM_RUBY_ID :
					case ITEM_SAPHIRE_ID :
						dynamic_cast<Emerald *>(Grid[i][j])->consume();
						break;
					case ITEM_BONUS_LIFE_ID :
					case ITEM_BONUS_FATAL_ID :
						dynamic_cast<BonusItem *>(Grid[i][j])->consume();
						break;
				}
			}
		}
	}
	while(!hiddenNobblins.empty()){
		Character *tmp = hiddenNobblins.top();
		hiddenNobblins.pop();
		delete tmp;
	}
	while(!hiddenHobblins.empty()){
		Character *tmp = hiddenHobblins.top();
		hiddenHobblins.pop();
		delete tmp;
	}

	while(!hiddenLifes.empty()){
		BonusItem *tmp = hiddenLifes.top();
		hiddenLifes.pop();
		delete tmp;
	}
	while(!hiddenSW.empty()){
		BonusItem *tmp = hiddenSW.top();
		hiddenSW.pop();
		delete tmp;
	}

	destroy_bitmap(terrainBuffer);
	free(terrainBX);
	purge();
}

void Terrain::purge ( void ) {
	std::map<uint16_t, TerrainAI *, uint32_comparator>::iterator iter;

	for(uint16_t i=0; i<staticRecycleBin.size(); i++){
		if(staticRecycleBin[i] != NULL)
			delete staticRecycleBin[i];
	}
	staticRecycleBin.clear();
	for( iter = dynamicRecycleBin.begin(); iter != dynamicRecycleBin.end(); iter++ ) {
		delete iter->second;
	}
	dynamicRecycleBin.clear();
}

Sprite **Terrain::getSpriteAt ( uint16_t x, uint16_t y ) {
	return &Grid[y>>4][x>>4];
}

void Terrain::getTerrainBounds ( uint16_t *x, uint16_t *y ) {
	*x = this->SizeX;
	*y = this->SizeY;
}

std::vector<BITMAP *> *Terrain::getFilm ( uint16_t objectID, uint16_t filmID ) {
	return animFHolder->getFilm(objectID, filmID);
}

void Terrain::notifyMove ( TerrainAI *caller, uint8_t direction ) {
	uint16_t i = caller->getCoords()->y_LT >> 4;
	uint16_t j = caller->getCoords()->x_LT >> 4;

	switch(direction){
		case DIRECTION_LEFT :
			if(Grid[i][j-1] && Grid[i][j-1]->getClassID() == ITEM_SOLID_TERRAIN_ID)
				staticRecycleBin.push_back(Grid[i][j-1]);
			if(Grid[i+1][j-1] && Grid[i+1][j-1]->getClassID() == ITEM_SOLID_TERRAIN_ID)
				staticRecycleBin.push_back(Grid[i+1][j-1]);
			if(Grid[i+2][j-1] && Grid[i+2][j-1]->getClassID() == ITEM_SOLID_TERRAIN_ID)
				staticRecycleBin.push_back(Grid[i+2][j-1]);
			if(Grid[i+3][j-1] && Grid[i+3][j-1]->getClassID() == ITEM_SOLID_TERRAIN_ID)
				staticRecycleBin.push_back(Grid[i+3][j-1]);

			Grid[i][j-1] = caller;
			Grid[i+1][j-1] = caller;
			Grid[i+2][j-1] = caller;
			Grid[i+3][j-1] = caller;

			if(caller->getClassID() != ITEM_GOLD_ID){
				Grid[i][j+3] = NULL;
				Grid[i+1][j+3] = NULL;
				Grid[i+2][j+3] = NULL;
				Grid[i+3][j+3] = NULL;
			}
			else{
				if(Grid[i][j+3] && Grid[i][j+3]->getClassID() == ITEM_GOLD_ID)
					Grid[i][j+3] = NULL;
				if(Grid[i+1][j+3] && Grid[i+1][j+3]->getClassID() == ITEM_GOLD_ID)
					Grid[i+1][j+3] = NULL;
				if(Grid[i+2][j+3] && Grid[i+2][j+3]->getClassID() == ITEM_GOLD_ID)
					Grid[i+2][j+3] = NULL;
				if(Grid[i+3][j+3] && Grid[i+3][j+3]->getClassID() == ITEM_GOLD_ID)
					Grid[i+3][j+3] = NULL;
			}
			break;
		case DIRECTION_RIGHT :
			if(Grid[i][j+4] && Grid[i][j+4]->getClassID() == ITEM_SOLID_TERRAIN_ID)
				staticRecycleBin.push_back(Grid[i][j+4]);
			if(Grid[i+1][j+4] && Grid[i+1][j+4]->getClassID() == ITEM_SOLID_TERRAIN_ID)
				staticRecycleBin.push_back(Grid[i+1][j+4]);
			if(Grid[i+2][j+4] && Grid[i+2][j+4]->getClassID() == ITEM_SOLID_TERRAIN_ID)
				staticRecycleBin.push_back(Grid[i+2][j+4]);
			if(Grid[i+3][j+4] && Grid[i+3][j+4]->getClassID() == ITEM_SOLID_TERRAIN_ID)
				staticRecycleBin.push_back(Grid[i+3][j+4]);

			Grid[i][j+4] = caller;
			Grid[i+1][j+4] = caller;
			Grid[i+2][j+4] = caller;
			Grid[i+3][j+4] = caller;

			if(caller->getClassID() != ITEM_GOLD_ID){
				Grid[i][j] = NULL;
				Grid[i+1][j] = NULL;
				Grid[i+2][j] = NULL;
				Grid[i+3][j] = NULL;
			}else{
				if(Grid[i][j] && Grid[i][j]->getClassID() == ITEM_GOLD_ID)
					Grid[i][j] = NULL;
				if(Grid[i+1][j] && Grid[i+1][j]->getClassID() == ITEM_GOLD_ID)
					Grid[i+1][j] = NULL;
				if(Grid[i+2][j] && Grid[i+2][j]->getClassID() == ITEM_GOLD_ID)
					Grid[i+2][j] = NULL;
				if(Grid[i+3][j] && Grid[i+3][j]->getClassID() == ITEM_GOLD_ID)
					Grid[i+3][j] = NULL;
			}
			break;
		case DIRECTION_UP :
			if(Grid[i-1][j] && Grid[i-1][j]->getClassID() == ITEM_SOLID_TERRAIN_ID)
				staticRecycleBin.push_back(Grid[i-1][j]);
			if(Grid[i-1][j+1] && Grid[i-1][j+1]->getClassID() == ITEM_SOLID_TERRAIN_ID)
				staticRecycleBin.push_back(Grid[i-1][j+1]);
			if(Grid[i-1][j+2] && Grid[i-1][j+2]->getClassID() == ITEM_SOLID_TERRAIN_ID)
				staticRecycleBin.push_back(Grid[i-1][j+2]);
			if(Grid[i-1][j+3] && Grid[i-1][j+3]->getClassID() == ITEM_SOLID_TERRAIN_ID)
				staticRecycleBin.push_back(Grid[i-1][j+3]);

			Grid[i-1][j] = caller;
			Grid[i-1][j+1] = caller;
			Grid[i-1][j+2] = caller;
			Grid[i-1][j+3] = caller;

			if(caller->getClassID() != ITEM_GOLD_ID){
				Grid[i+3][j] = NULL;
				Grid[i+3][j+1] = NULL;
				Grid[i+3][j+2] = NULL;
				Grid[i+3][j+3] = NULL;
			}else{
				if(Grid[i+3][j] && Grid[i+3][j]->getClassID() == ITEM_GOLD_ID)
					Grid[i+3][j] = NULL;
				if(Grid[i+3][j+1] && Grid[i+3][j+1]->getClassID() == ITEM_GOLD_ID)
					Grid[i+3][j+1] = NULL;
				if(Grid[i+3][j+2] && Grid[i+3][j+2]->getClassID() == ITEM_GOLD_ID)
					Grid[i+3][j+2] = NULL;
				if(Grid[i+3][j+3] && Grid[i+3][j+3]->getClassID() == ITEM_GOLD_ID)
					Grid[i+3][j+3] = NULL;
			}
			break;
		case DIRECTION_DOWN :
			if(Grid[i+4][j] && Grid[i+4][j]->getClassID() == ITEM_SOLID_TERRAIN_ID)
				staticRecycleBin.push_back(Grid[i+4][j]);
			if(Grid[i+4][j+1] && Grid[i+4][j+1]->getClassID() == ITEM_SOLID_TERRAIN_ID)
				staticRecycleBin.push_back(Grid[i+4][j+1]);
			if(Grid[i+4][j+2] && Grid[i+4][j+2]->getClassID() == ITEM_SOLID_TERRAIN_ID)
				staticRecycleBin.push_back(Grid[i+4][j+2]);
			if(Grid[i+4][j+3] && Grid[i+4][j+3]->getClassID() == ITEM_SOLID_TERRAIN_ID)
				staticRecycleBin.push_back(Grid[i+4][j+3]);

			Grid[i+4][j] = caller;
			Grid[i+4][j+1] = caller;
			Grid[i+4][j+2] = caller;
			Grid[i+4][j+3] = caller;

			if(caller->getClassID() != ITEM_GOLD_ID){
				Grid[i][j] = NULL;
				Grid[i][j+1] = NULL;
				Grid[i][j+2] = NULL;
				Grid[i][j+3] = NULL;
			}else{
				if(Grid[i][j] && Grid[i][j]->getClassID() == ITEM_GOLD_ID)
					Grid[i][j] = NULL;
				if(Grid[i][j+1] && Grid[i][j+1]->getClassID() == ITEM_GOLD_ID)
					Grid[i][j+1] = NULL;
				if(Grid[i][j+2] && Grid[i][j+2]->getClassID() == ITEM_GOLD_ID)
					Grid[i][j+2] = NULL;
				if(Grid[i][j+3] && Grid[i][j+3]->getClassID() == ITEM_GOLD_ID)
					Grid[i][j+3] = NULL;
			}
			break;
	}
}

void Terrain::notifyConsume ( TerrainAI *caller ) {
	uint32_t i = caller->getCoords()->y_LT >> 4;
	uint32_t j = caller->getCoords()->x_LT >> 4;

	if(caller->getClassID() == ITEM_GOLD_ID){
		if(dynamicRecycleBin.find(caller->getID()) != dynamicRecycleBin.end()){
			dynamicRecycleBin[caller->getID()] = caller;
		}
//		RecycleBin.push_back(gravityAware[caller->getID()]);
		gravityAware.erase(caller->getID());
	}
	else{
		if(dynamicRecycleBin.find(caller->getID()) != dynamicRecycleBin.end()){
			dynamicRecycleBin[caller->getID()] = caller;
		}
//		RecycleBin.push_back(TerrainObjects[caller->getID()]);
		TerrainObjects.erase(caller->getID());
		if(caller->getClassID() == ITEM_EMERALD_ID ||
		   caller->getClassID() == ITEM_RUBY_ID ||
		   caller->getClassID() == ITEM_SAPHIRE_ID
		){
		--this->progress;
		}
	}

	for(uint8_t ii=0; ii<4; ii++){
		for(uint8_t jj=0; jj<4; jj++){
			if(Grid[i+ii][j+jj] && ((TerrainAI *)Grid[i+ii][j+jj])->getID() == caller->getID()){
				Grid[i+ii][j+jj] = NULL;
			}
		}
	}
}

void Terrain::notifyDeath ( TerrainAI *caller ) {
	uint32_t i, j;
	if(dynamicRecycleBin.find(caller->getID()) != dynamicRecycleBin.end()){
		dynamicRecycleBin[caller->getID()] = caller;
	}
//	RecycleBin.push_back(Characters[caller->getID()]);
	Characters.erase(caller->getID());

	for(uint8_t i=0; i<MOVE_ANIM_STEPS_REQ; i++){
		((Character *)caller)->animate();
	}

	i = caller->getCoords()->y_LT >> 4;
	j = caller->getCoords()->x_LT >> 4;

	for(uint8_t ii=0; ii<4; ii++){
		for(uint8_t jj=0; jj<4; jj++){
			Grid[i+ii][j+jj] = NULL;
		}
	}

	if(caller->getClassID() == ITEM_DIGGER_ID){
		game->over();
	}
}

void Terrain::notifyDiggerReset ( void ) {
	uint32_t i, j;
	std::map<uint16_t, Character *, uint32_comparator>::iterator iter;

	nextDirection = DIRECTION_STILL;
	for( iter = Characters.begin(); iter != Characters.end(); iter++ ) {
		iter->second->die();
	}

	for(uint8_t i=0; i<MOVE_ANIM_STEPS_REQ; i++){
		digger->animate();
	}

	i = digger->getCoords()->y_LT >> 4;
	j = digger->getCoords()->x_LT >> 4;

	for(uint8_t ii=0; ii<4; ii++){
		for(uint8_t jj=0; jj<4; jj++){
			Grid[i+ii][j+jj] = NULL;
		}
	}

	digger->reset(diggerInitX, diggerInitY, 64, DIRECTION_RIGHT);

	i = diggerInitY >> 4;
	j = diggerInitX >> 4;

	for(uint8_t ii=0; ii<4; ii++){
		for(uint8_t jj=0; jj<4; jj++){
			if(Grid[i+ii][j+jj] != NULL) {
				uint8_t type = Grid[i+ii][j+jj]->getClassID();
				switch(type){
					case ITEM_SOLID_TERRAIN_ID :
					case ITEM_ROCK_ID :
						staticRecycleBin.push_back(Grid[i+ii][j+jj]);
						break;
					case ITEM_HOBBLIN_ID :
					case ITEM_NOBBLIN_ID :
						((Bot *)Grid[i+ii][j+jj])->die();
						break;
					case ITEM_GOLD_ID :
						((Gold *)Grid[i+ii][j+jj])->consume();
						break;
					case ITEM_EMERALD_ID :
					case ITEM_RUBY_ID :
					case ITEM_SAPHIRE_ID :
						((Emerald *)Grid[i+ii][j+jj])->consume();
						break;
					case ITEM_BONUS_LIFE_ID :
					case ITEM_BONUS_FATAL_ID :
						((BonusItem *)Grid[i+ii][j+jj])->consume();
						break;
				}
			}
		}
	}

	for(uint8_t ii=0; ii<4; ii++){
		for(uint8_t jj=0; jj<4; jj++){
			Grid[i+ii][j+jj] = digger;
		}
	}
}

void Terrain::scheduler ( void ) {
	if(schedConflictResolve){
		pBFSceduleTime = pBLSceduleTime = pHSceduleTime = pNSceduleTime = getTime();
		schedConflictResolve = false;
	}

	if(currTime - pNSceduleTime > 12000000 || !lInitDynamic[0]){	//	12 sec schedule Nobblins
		if(Characters.size() < 3){
			if(hiddenNobblins.size()){
				lInitDynamic[0] = this->initDynamicObject(ITEM_NOBBLIN_ID);
			}
		}
		else{
			lInitDynamic[0] = true;
		}
		pNSceduleTime = currTime;
	}
	if(currTime - pHSceduleTime > 45000000 || !lInitDynamic[1]){	//	45 sec schedule Hobblins
		if(Characters.size() < 3){
			if(hiddenHobblins.size()){
				lInitDynamic[1] = this->initDynamicObject(ITEM_HOBBLIN_ID);
			}
		}
		else{
			lInitDynamic[1] = true;
		}
		pHSceduleTime = currTime;
	}
	if(currTime - pBLSceduleTime > 61000000 || !lInitDynamic[2]){	//	61 sec schedule Lifes
		if(hiddenLifes.size()){
			lInitDynamic[2] = this->initDynamicObject(ITEM_BONUS_LIFE_ID);
		}
		else{
			lInitDynamic[2] = true;
		}
		pBLSceduleTime = currTime;
	}
	if(currTime - pBFSceduleTime > 72000000 || !lInitDynamic[3]){	//	72 sec schedule SuperWeapons
		if(hiddenSW.size()){
			lInitDynamic[3] = this->initDynamicObject(ITEM_BONUS_FATAL_ID);
		}
		else{
			lInitDynamic[3] = true;
		}
		pBFSceduleTime = currTime;
	}
}

void Terrain::getKeyboard ( uint8_t key) {
	nextDirection = key;
}

void Terrain::gravityCheck ( void ) {
	std::map<uint16_t, Gold *, uint16_comparator>::iterator iter;

	if(currTime - gravityCheckTime > 200000){		//	5Hz Gravity Collision Check.
		for( iter = gravityAware.begin(); iter != gravityAware.end(); iter++ ) {
			iter->second->startFall();
		}
		gravityCheckTime = currTime;
	}
}

void Terrain::move ( void ) {
	std::map<uint16_t, Character *, uint32_comparator>::iterator iter;

	if(nextDirection != DIRECTION_STILL)
		digger->move(nextDirection);

	for( iter = Characters.begin(); iter != Characters.end(); iter++ ) {
		((Bot *)(iter->second))->move(digger->getCoords());
	}
}

void Terrain::animate ( void ) {
	std::map<uint16_t, Gold *, uint32_comparator>::iterator GA_Iter;
	std::map<uint16_t, TerrainAI *, uint32_comparator>::iterator TO_Iter;
	std::map<uint16_t, Character *, uint32_comparator>::iterator CH_Iter;

	if(currTime - previousCharacterTime > 16666){	//	60Hz Character && Gold Animation
		for( GA_Iter = gravityAware.begin(); GA_Iter != gravityAware.end(); GA_Iter++ ) {
			GA_Iter->second->animate();
			GA_Iter->second->fall();
		}
		for( CH_Iter = Characters.begin(); CH_Iter != Characters.end(); CH_Iter++ ) {
			CH_Iter->second->animate();
		}
		digger->animate();
		previousCharacterTime = currTime;
	}
	if(currTime - previousObjectTime > 100000){		//	10Hz Object Animation
		for( TO_Iter = TerrainObjects.begin(); TO_Iter != TerrainObjects.end(); TO_Iter++ ) {
			TO_Iter->second->animate();
		}
		previousObjectTime = currTime;
		activeTerrainTime += (currTime - previousObjectTime);
	}
}

void Terrain::render ( void ) {
	std::map<uint16_t, TerrainAI *, uint32_comparator>::iterator TO_Iter;
	std::map<uint16_t, Character *, uint32_comparator>::iterator CH_Iter;
	std::map<uint16_t, Gold *, uint32_comparator>::iterator GA_Iter;

	for( GA_Iter = gravityAware.begin(); GA_Iter != gravityAware.end(); GA_Iter++ ) {
		GA_Iter->second->draw();
	}
	for( CH_Iter = Characters.begin(); CH_Iter != Characters.end(); CH_Iter++ ) {
		CH_Iter->second->draw();
	}
	digger->draw();
	for( TO_Iter = TerrainObjects.begin(); TO_Iter != TerrainObjects.end(); TO_Iter++ ) {
		TO_Iter->second->draw();
	}
#if defined(ASSERTIONS_ENABLED)
	printTerrain();
#endif
}

uint16_t Terrain::getTerrainProgress ( void ) {
	return this->progress;
}

void Terrain::initBackGround ( void ) {
	uint16_t x_16x16;
	uint16_t y_16x16;

	getTerrainTiles_16x16(GraphicsMode, &x_16x16, &y_16x16 );

	for(uint16_t i=0,targetY=0; i<y_16x16; i++, targetY+=16){
		for(uint16_t j=0,targetX=0; j<x_16x16; j++,targetX+=16){
			uint8_t ii = i & 0x03;
			uint8_t jj = j & 0x03;
			TerrainTile *tt = new TerrainTile(
					BitmapBuffer->getBitmap(ITEM_SOLID_TERRAIN_ID,ii,jj)
			);
			tt->setClassID(ITEM_SOLID_TERRAIN_ID);
			tt->registerMap(terrainBuffer);
			tt->set(targetX, targetY, 16);
			tt->setState(false);
			tt->set_CD(true);
			Grid[i][j] = tt;
			tt->draw();
		}
	}
}

void Terrain::initTerrain ( void ) {
	uint16_t x_64x64;
	uint16_t y_64x64;
	uint8_t objectID;

	uint8_t cGoldID = 1;
	uint8_t cEmeraldID = 1;
	uint8_t cRubyID = 1;
	uint8_t cSaphireID = 1;
	uint8_t cLifeID = 1;
	uint8_t cSW_ID = 1;

	getTerrainTiles_64x64(GraphicsMode, &x_64x64, &y_64x64 );

	for(uint16_t i=0,targetY=0; i<y_64x64; i++, targetY+=64){
		for(uint16_t j=0,targetX=0; j<x_64x64; j++,targetX+=64){
			objectID = getItemAt(GraphicsMode, gameLevel, i, j);
			switch(objectID){
				case ITEM_GAP_ID : {
					for(uint8_t ii=0; ii<4; ii++){
						for(uint8_t jj=0; jj<4; jj++){
							staticRecycleBin.push_back(Grid[(i<<2)+ii][(j<<2)+jj]);
							Grid[(i<<2)+ii][(j<<2)+jj] = NULL;
							draw_sprite( terrainBuffer,
									BitmapBuffer->getBitmap(ITEM_GAP_ID,0,0),
									(targetX+(ii<<4)), (targetY+(jj<<4))
							);
						}
					}
				}
					break;
				case ITEM_SOLID_TERRAIN_ID :
					break;
				case ITEM_ROCK_ID : {
					uint16_t rand_rock = RANDOM(ITEM_ROCK_NUM_MIN, (ITEM_ROCK_NUM_MAX-1));
					TerrainTile *tt = new TerrainTile(
							BitmapBuffer->getBitmap(ITEM_ROCK_ID,rand_rock,0)
					);
					tt->setClassID(ITEM_ROCK_ID);
					tt->registerMap(terrainBuffer);
					tt->set(targetX, targetY, 64);
					tt->setState(true);
					tt->set_CD(true);
					for(uint8_t ii=0; ii<4; ii++){
						for(uint8_t jj=0; jj<4; jj++){
							staticRecycleBin.push_back(Grid[(i<<2)+ii][(j<<2)+jj]);
							Grid[(i<<2)+ii][(j<<2)+jj] = tt;
						}
					}
					tt->draw();
				}
					break;
				case ITEM_DIGGER_ID : {
					uint16_t diggerID = ((uint16_t)ITEM_DIGGER_ID<<8) | (uint16_t)1;
					digger = new Digger(this, diggerID);
					diggerInitX = targetX;
					diggerInitY = targetY;
					digger->set(targetX, targetY, 64);
					digger->registerMap(
							terrainBuffer
					);
					digger->setDarkTerrainMask(
							BitmapBuffer->getBitmap(ITEM_ALPHA_MASK_ID, 0, 0)
					);
					digger->setBitmap(
							BitmapBuffer->getBitmap(ITEM_DIGGER_ID,0,0)
					);
					digger->setGrave(
							BitmapBuffer->getBitmap(ITEM_GRAVE_ID, 0, 0)
					);
					digger->setFilm(
							animFHolder->getFilm(ITEM_DIGGER_ID, DIRECTION_LEFT)
					);
					digger->setDeathSound(
							SoundBuffer->getSample(ITEM_SOUND_DEATH_ID)
					);
					digger->setClassID(ITEM_DIGGER_ID);
					digger->start();
					for(uint8_t ii=0; ii<4; ii++){
						for(uint8_t jj=0; jj<4; jj++){
							staticRecycleBin.push_back(Grid[(i<<2)+ii][(j<<2)+jj]);
							Grid[(i<<2)+ii][(j<<2)+jj] = digger;
						}
					}
					digger->draw();
				}
					break;
				case ITEM_HOBBLIN_ID : {
					uint16_t hobblinID = ((uint16_t)ITEM_HOBBLIN_ID<<8) | (uint16_t)HobblinCounter++;
					Hobblin *hobblin = new Hobblin(this, hobblinID);
					hobblin->set(targetX, targetY, 64);
					hobblin->registerMap(
							terrainBuffer
					);
					hobblin->setDarkTerrainMask(
							BitmapBuffer->getBitmap(ITEM_ALPHA_MASK_ID, 0, 0)
					);
					hobblin->setBitmap(
							BitmapBuffer->getBitmap(ITEM_HOBBLIN_ID,0,0)
					);
					hobblin->setFilm(
							animFHolder->getFilm(ITEM_HOBBLIN_ID, DIRECTION_LEFT)
					);
					hobblin->setClassID(ITEM_HOBBLIN_ID);
					hobblin->start();
					botInitX = targetX;
					botInitY = targetY;
					for(uint8_t ii=0; ii<4; ii++){
						for(uint8_t jj=0; jj<4; jj++){
							staticRecycleBin.push_back(Grid[(i<<2)+ii][(j<<2)+jj]);
							Grid[(i<<2)+ii][(j<<2)+jj] = hobblin;
						}
					}
					Characters[hobblinID] = hobblin;
					hobblin->draw();
				}
					break;
				case ITEM_NOBBLIN_ID : {
					uint16_t nobblinID = ((uint16_t)ITEM_NOBBLIN_ID<<8) | (uint16_t)NobblinCounter++;
					Nobblin *nobblin = new Nobblin(this, nobblinID);
					nobblin->set(targetX, targetY, 64);
					nobblin->registerMap(
							terrainBuffer
					);
					nobblin->setDarkTerrainMask(
							BitmapBuffer->getBitmap(ITEM_ALPHA_MASK_ID, 0, 0)
					);
					nobblin->setBitmap(
							BitmapBuffer->getBitmap(ITEM_NOBBLIN_ID,0,0)
					);
					nobblin->setFilm(
							animFHolder->getFilm(ITEM_NOBBLIN_ID, DIRECTION_LEFT)
					);
					nobblin->setClassID(ITEM_NOBBLIN_ID);
					nobblin->start();
					botInitX = targetX;
					botInitY = targetY;
					for(uint8_t ii=0; ii<4; ii++){
						for(uint8_t jj=0; jj<4; jj++){
							staticRecycleBin.push_back(Grid[(i<<2)+ii][(j<<2)+jj]);
							Grid[(i<<2)+ii][(j<<2)+jj] = nobblin;
						}
					}
					Characters[nobblinID] = nobblin;
					nobblin->draw();
				}
					break;
				case ITEM_GOLD_ID : {
					uint16_t gID = ((uint16_t)ITEM_GOLD_ID<<8) | (uint16_t)cGoldID++;
					Gold *gold = new Gold( this, gID, GOLD_MONEY );
					gold->set(targetX, targetY, 64);
					gold->registerMap(
							terrainBuffer
					);
					gold->setBitmap(
							BitmapBuffer->getBitmap(ITEM_GOLD_ID, ITEM_GOLD_PACKED_STATE, 0)
					);
					gold->setDarkTerrainMask(
							BitmapBuffer->getBitmap(ITEM_ALPHA_MASK_ID, 0, 0)
					);
					gold->setConsumeMask(
							BitmapBuffer->getBitmap(ITEM_GAP_ID, 0 ,1)
					);
					gold->setFilm(
							animFHolder->getFilm(ITEM_GOLD_ID, ITEM_GOLD_PACKED_STATE)
					);
					gold->setConsumeSound(
							SoundBuffer->getSample(ITEM_SOUND_PICK_GOLD_ID)
					);
					gold->setUnpackSound(
							SoundBuffer->getSample(ITEM_SOUND_SMASH_ID)
					);
					gold->setClassID(ITEM_GOLD_ID);
					gold->start();
					for(uint8_t ii=0; ii<4; ii++){
						for(uint8_t jj=0; jj<4; jj++){
							staticRecycleBin.push_back(Grid[(i<<2)+ii][(j<<2)+jj]);
							Grid[(i<<2)+ii][(j<<2)+jj] = gold;
						}
					}
					gravityAware[gID] = gold;
					gold->draw();
				}
					break;
				case ITEM_EMERALD_ID : {
					uint16_t eID = ((uint16_t)ITEM_EMERALD_ID<<8) | (uint16_t)cEmeraldID++;
					Emerald *emerald = new Emerald( this, eID, EMERALD_MONEY );
					emerald->set(targetX, targetY, 64);
					emerald->registerMap(
							terrainBuffer
					);
					emerald->setBitmap(
							BitmapBuffer->getBitmap(ITEM_EMERALD_ID, 0, ITEM_EMERALD_NUM_MIN)
					);
					emerald->setConsumeMask(
							BitmapBuffer->getBitmap(ITEM_ALPHA_MASK_ID, 0, 0)
					);
					emerald->setFilm(
							animFHolder->getFilm(ITEM_EMERALD_ID, 0)
					);
					emerald->setConsumeSound(
							SoundBuffer->getSample(ITEM_SOUND_PICK_EMERALD_ID)
					);
					emerald->setClassID(ITEM_EMERALD_ID);
					emerald->start();
					for(uint8_t ii=0; ii<4; ii++){
						for(uint8_t jj=0; jj<4; jj++){
							staticRecycleBin.push_back(Grid[(i<<2)+ii][(j<<2)+jj]);
							Grid[(i<<2)+ii][(j<<2)+jj] = emerald;
						}
					}
					this->progress++;
					TerrainObjects[eID] = emerald;
					emerald->draw();
				}
					break;
				case ITEM_RUBY_ID : {
					uint16_t eID = ((uint16_t)ITEM_RUBY_ID<<8) | (uint16_t)cRubyID++;
					Emerald *emerald = new Emerald( this, eID, RUBY_MONEY );
					emerald->set(targetX, targetY, 64);
					emerald->registerMap(
							terrainBuffer
					);
					emerald->setBitmap(
							BitmapBuffer->getBitmap(ITEM_RUBY_ID, 0, ITEM_RUBY_NUM_MIN)
					);
					emerald->setConsumeMask(
							BitmapBuffer->getBitmap(ITEM_ALPHA_MASK_ID, 0, 0)
					);
					emerald->setFilm(
							animFHolder->getFilm(ITEM_RUBY_ID, 0)
					);
					emerald->setConsumeSound(
							SoundBuffer->getSample(ITEM_SOUND_PICK_EMERALD_ID)
					);
					emerald->setClassID(ITEM_RUBY_ID);
					emerald->start();
					for(uint8_t ii=0; ii<4; ii++){
						for(uint8_t jj=0; jj<4; jj++){
							staticRecycleBin.push_back(Grid[(i<<2)+ii][(j<<2)+jj]);
							Grid[(i<<2)+ii][(j<<2)+jj] = emerald;
						}
					}
					this->progress++;
					TerrainObjects[eID] = emerald;
					emerald->draw();
				}
					break;
				case ITEM_SAPHIRE_ID : {
					uint16_t eID = ((uint16_t)ITEM_SAPHIRE_ID<<8) |(uint16_t) cSaphireID++;
					Emerald *emerald = new Emerald( this, eID, SAPHIRE_MONEY );
					emerald->set(targetX, targetY, 64);
					emerald->registerMap(
							terrainBuffer
					);
					emerald->setBitmap(
							BitmapBuffer->getBitmap(ITEM_SAPHIRE_ID, 0, ITEM_SAPHIRE_NUM_MIN)
					);
					emerald->setConsumeMask(
							BitmapBuffer->getBitmap(ITEM_ALPHA_MASK_ID, 0, 0)
					);
					emerald->setFilm(
							animFHolder->getFilm(ITEM_SAPHIRE_ID, 0)
					);
					emerald->setConsumeSound(
							SoundBuffer->getSample(ITEM_SOUND_PICK_EMERALD_ID)
					);
					emerald->setClassID(ITEM_SAPHIRE_ID);
					emerald->start();
					for(uint8_t ii=0; ii<4; ii++){
						for(uint8_t jj=0; jj<4; jj++){
							staticRecycleBin.push_back(Grid[(i<<2)+ii][(j<<2)+jj]);
							Grid[(i<<2)+ii][(j<<2)+jj] = emerald;
						}
					}
					this->progress++;
					TerrainObjects[eID] = emerald;
					emerald->draw();
				}
					break;
				case ITEM_BONUS_LIFE_ID : {
					uint16_t bonusID = ((uint16_t)ITEM_BONUS_LIFE_ID<<8) | (uint16_t)cLifeID++;
					BonusItem *bi = new BonusItem( this, bonusID, ITEM_TYPE_LIFE );
					bi->set(targetX, targetY, 64);
					bi->registerMap(
							terrainBuffer
					);
					bi->setBitmap(
							BitmapBuffer->getBitmap(ITEM_BONUS_LIFE_ID, 0, 0)
					);
					bi->setConsumeMask(
							BitmapBuffer->getBitmap(ITEM_ALPHA_MASK_ID, 0, 0)
					);
					bi->setClassID(ITEM_BONUS_LIFE_ID);
					hiddenLifes.push(bi);
				}
					break;
				case ITEM_BONUS_FATAL_ID : {
					uint16_t bonusID = ((uint16_t)ITEM_BONUS_FATAL_ID<<8) | (uint16_t)cSW_ID++;
					BonusItem *bi = new BonusItem( this, bonusID, ITEM_TYPE_FATAL );
					bi->set(targetX, targetY, 64);
					bi->registerMap(
							terrainBuffer
					);
					bi->setBitmap(
							BitmapBuffer->getBitmap(ITEM_BONUS_FATAL_ID, 0, 0)
					);
					bi->setConsumeMask(
							BitmapBuffer->getBitmap(ITEM_ALPHA_MASK_ID, 0, 0)
					);
					bi->setClassID(ITEM_BONUS_FATAL_ID);
					hiddenSW.push(bi);
				}
					break;
			}
		}
	}
}

void Terrain::preCacheBots ( uint8_t nobblins, uint8_t hobblins ) {
	for(uint8_t i=0; i<nobblins; i++){
		uint16_t nobblinID = (ITEM_NOBBLIN_ID<<8) | NobblinCounter++;
		Nobblin *nobblin = new Nobblin(this,nobblinID);
		nobblin->setClassID(ITEM_NOBBLIN_ID);
		nobblin->registerMap(
				terrainBuffer
		);
		nobblin->setDarkTerrainMask(
				BitmapBuffer->getBitmap(ITEM_ALPHA_MASK_ID, 0, 0)
		);
		nobblin->setBitmap(
				BitmapBuffer->getBitmap(ITEM_NOBBLIN_ID,DIRECTION_LEFT,0)
		);
		nobblin->setFilm(
				animFHolder->getFilm(ITEM_NOBBLIN_ID, DIRECTION_LEFT)
		);
		hiddenNobblins.push(nobblin);
	}
	for(uint8_t i=0; i<hobblins; i++){
		uint16_t hobblinID = (ITEM_HOBBLIN_ID<<8) | HobblinCounter++;
		Hobblin *hobblin = new Hobblin(this,hobblinID);
		hobblin->setClassID(ITEM_HOBBLIN_ID);
		hobblin->registerMap(
				terrainBuffer
		);
		hobblin->setDarkTerrainMask(
				BitmapBuffer->getBitmap(ITEM_ALPHA_MASK_ID, 0, 0)
		);
		hobblin->setBitmap(
				BitmapBuffer->getBitmap(ITEM_HOBBLIN_ID,DIRECTION_LEFT,0)
		);
		hobblin->setFilm(
				animFHolder->getFilm(ITEM_HOBBLIN_ID, DIRECTION_LEFT)
		);
		hiddenHobblins.push(hobblin);
	}
}

bool Terrain::initDynamicObject ( uint8_t objectType ) {
	uint16_t targetX, targetY, i, j;

	switch(objectType){
		case ITEM_NOBBLIN_ID : {
			targetX = botInitX;
			targetY = botInitY;
			i = targetY >> 4;
			j = targetX >> 4;
			if(hiddenNobblins.empty())
				return false;
			for(uint8_t ii=0; ii<4; ii++){
				for(uint8_t jj=0; jj<4; jj++){
					if(Grid[i+ii][j+jj] != NULL){
						return false;
					}
				}
			}
			Nobblin *nobblin = (Nobblin *)hiddenNobblins.top();
			hiddenNobblins.pop();
			nobblin->set(targetX, targetY, 64);
			for(uint8_t ii=0; ii<4; ii++){
				for(uint8_t jj=0; jj<4; jj++){
					Grid[i+ii][j+jj] = nobblin;
				}
			}
			Characters[nobblin->getID()] = nobblin;
		}
			break;
		case ITEM_HOBBLIN_ID : {
			targetX = botInitX;
			targetY = botInitY;
			i = targetY >> 4;
			j = targetX >> 4;
			if(hiddenHobblins.empty())
				return false;
			for(uint8_t ii=0; ii<4; ii++){
				for(uint8_t jj=0; jj<4; jj++){
					if(Grid[i+ii][j+jj] != NULL){
						return false;
					}
				}
			}
			Hobblin *hobblin = (Hobblin *)hiddenHobblins.top();
			hiddenHobblins.pop();
			hobblin->set(targetX, targetY, 64);
			for(uint8_t ii=0; ii<4; ii++){
				for(uint8_t jj=0; jj<4; jj++){
					Grid[i+ii][j+jj] = hobblin;
				}
			}
			Characters[hobblin->getID()] = hobblin;
		}
			break;
		case ITEM_BONUS_LIFE_ID : {
			if(hiddenLifes.empty())
				return false;
			BonusItem *bi = hiddenLifes.top();
			bool ok = true;
			struct BoundingBox *target = bi->getCoords();
			uint16_t i = (target->y_LT >> 4);
			uint16_t j = (target->x_LT >> 4);
			for(uint8_t ii=0; ii<4; ii++){
				for(uint8_t jj=0; jj<4; jj++){
					if(Grid[i+ii][j+jj] != NULL &&
					   Grid[i+ii][j+jj]->getClassID() != ITEM_SOLID_TERRAIN_ID){
						ok = false;
					}
				}
			}
			if(ok){
				hiddenLifes.pop();
				for(uint8_t ii=0; ii<4; ii++){
					for(uint8_t jj=0; jj<4; jj++){
						if(Grid[i+ii][j+jj] != NULL){
							staticRecycleBin.push_back(Grid[i+ii][j+jj]);
						}
						Grid[i+ii][j+jj] = bi;
					}
				}
				TerrainObjects[bi->getID()] = bi;
				bi->draw();
			}
		}
			break;
		case ITEM_BONUS_FATAL_ID : {
			if(hiddenSW.empty())
				return false;
			BonusItem *bi = hiddenSW.top();
			bool ok = true;
			struct BoundingBox *target = bi->getCoords();
			uint16_t i = (target->y_LT >> 4);
			uint16_t j = (target->x_LT >> 4);
			for(uint8_t ii=0; ii<4; ii++){
				for(uint8_t jj=0; jj<4; jj++){
					if(Grid[i+ii][j+jj] != NULL &&
					   Grid[i+ii][j+jj]->getClassID() != ITEM_SOLID_TERRAIN_ID){
						ok = false;
					}
				}
			}
			if(ok){
				hiddenSW.pop();
				for(uint8_t ii=0; ii<4; ii++){
					for(uint8_t jj=0; jj<4; jj++){
						if(Grid[i+ii][j+jj] != NULL){
							staticRecycleBin.push_back(Grid[i+ii][j+jj]);
						}
						Grid[i+ii][j+jj] = bi;
					}
				}
				TerrainObjects[bi->getID()] = bi;
				bi->draw();
			}
		}
			break;
	}
	return true;
}

void Terrain::printTerrain ( void ) {
	char buff[8];
	uint16_t x_16x16;
	uint16_t y_16x16;

	getTerrainTiles_16x16(GraphicsMode, &x_16x16, &y_16x16 );

	for(uint16_t i=0; i<y_16x16; i++){
		for(uint16_t j=0; j<x_16x16; j++){
			uint32_t item;
			if(Grid[i][j] == NULL)
				item = 0;
			else
				item = Grid[i][j]->getClassID();
			sprintf(buff, "%u", item);
			textout_centre_ex(
					terrainBuffer,
					font,
					buff,
					(j<<4)+8,
					(i<<4)+4,
					makecol(0,0,0), -1
			);
		}
	}
}
