#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>

#include "AnimationFilmHolder.h"
#include "ObjectDefinitions.h"
#include "BitmapHolder.h"
#include "SoundHolder.h"
#include "../GFX/LevelHandler.h"
#include "../GFX/gfx.h"
#include "../Objects/Terrain.h"
#include "../Common.h"
#include "../GUI/ControlTile.h"
//#include "../GUI/LogoTile.h"
#include "Game.h"
#include "Timer.h"

uint64_t currTime;
static uint64_t startPause;
static uint64_t endPause;

Game::Game( uint16_t mode ) {
	uint16_t x, y;

	install_keyboard();

	set_color_depth(32);
	set_alpha_blender();

	getScreenResolution(mode, &x, &y);

	if (set_gfx_mode(GFX_AUTODETECT, x, y, 0, 0) != 0) {
		if (set_gfx_mode(GFX_SAFE, x, y, 0, 0) != 0) {
			set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
			allegro_message("Unable to set any graphic mode\n%s\n", allegro_error);
			exit(-1);
		}
	}

    if (install_sound(DIGI_AUTODETECT, MIDI_NONE, "") != 0) {
        allegro_message("Error initializing sound system");
        return;
    }

    GraphicsMode = mode;
	animFHolder = new AnimationFilmHolder();
	BitmapBuffer = new BitmapHolder();
	SoundBuffer = new SoundHolder();
	this->tmpDiggerLifes = DIGGER_LIFES;
	this->tmpScore = 0;
	this->currLevel = 0;
	overFlag = false;

	initBitMaps();
	initAnimationFilms();
	initSounds();

	controlGUI = new ControlTile(
			GraphicsMode,
			BitmapBuffer->getBitmap(ITEM_LABEL_LEVEL_ID, 0, 0),
			BitmapBuffer->getBitmap(ITEM_LABEL_SCORE_ID, 0, 0),
			BitmapBuffer->getBitmap(ITEM_LABEL_HEALTH_ID, 0, 0)
	);

	controlGUI->setScreenBuffer(screen);

	for(uint8_t i=0; i<10; i++){
		controlGUI->setNumberBuffer(
				i, BitmapBuffer->getBitmap(ITEM_LABEL_NUMBER_ID, 0, i)
		);
	}
/*
	logo = new LogoTile(GraphicsMode) ;
	logo->setScreenBuffer(screen);
	for(uint8_t i=0; i<3; i++){
		logo->setAuthor(
				i, BitmapBuffer->getBitmap(ITEM_AUTHORS_ID, 0, i)
		);
	}
*/
	loop_ticks = 0;
	screen_ticks = 0;
	game_loops = 0;
	screen_loops = 0;
	freq = cpufreq();
}

Game::~Game( ) {
	delete animFHolder;
	delete BitmapBuffer;
}

void Game::start ( void ) {
	do{
		terrain = new Terrain(this, GraphicsMode, currLevel, BitmapBuffer, animFHolder, SoundBuffer);
		terrain->setLifes(tmpDiggerLifes);
		terrain->setScore(tmpScore);
		backBuffer = terrain->getBackBuffer();
		terrainBX = terrain->getTerrainBX();
		IntroLoop();
		currTime = getTime();
		GameLoop();
		tmpDiggerLifes = terrain->getLifes();
		tmpScore = terrain->getScore();
		delete terrain;
		currLevel++;
	}while (currLevel < MAX_LEVELS && !overFlag) ;
	if(currLevel == MAX_LEVELS){
		GameOverLoop(ITEM_VICTORY_ID);
	}
	else{
		GameOverLoop(ITEM_GAME_OVER_ID);
	}
}

void Game::pause ( void ) {
	uint64_t offset;
	startPause = getTime();
	BITMAP *paused = BitmapBuffer->getBitmap(ITEM_PAUSED_ID, 0, 0);
	uint16_t x, y, tX, tY;
	getScreenResolution(GraphicsMode, &x, &y);
	tX = (x - paused->w)/2;
	tY = (y - paused->h)/2;
	do{
		clear_keybuf();
		acquire_screen();
		draw_trans_sprite( screen, paused, tX, tY);
		release_screen();
		rest(0);
		usleep(100);
	}while(key[KEY_PAUSE]);
	endPause = getTime();
	offset = (endPause - startPause);
	currTime += offset;
	terrain->timeShift(offset);
}

void Game::over ( void ) {
	overFlag = true;
}

void Game::printStatistics ( void ) {
	uint64_t loop_ticks_per_loop = loop_ticks/game_loops;
	uint64_t screen_ticks_per_loop = loop_ticks/screen_loops;
	double	 screen_time_per_loop = (double)screen_ticks_per_loop/(freq*1000000);
	double   loop_time_per_loop = (double)loop_ticks_per_loop/(freq*1000000);

	std::cout << "CPU Frequency in MHz              : ";
	std::cout << freq;
	std::cout << std::endl;
	std::cout << "Average Game Ticks / Loop         : ";
	std::cout << loop_ticks_per_loop;
	std::cout << std::endl;
	std::cout << "Average Real Time / Loop (usec)   : ";
	std::cout << loop_time_per_loop*1000000;
	std::cout << std::endl;
	std::cout << "Average Screen Ticks / Loop       : ";
	std::cout << screen_ticks_per_loop;
	std::cout << std::endl;
	std::cout << "Average Screen Time / Loop (msec) : ";
	std::cout << screen_time_per_loop*1000;
	std::cout << std::endl;
	std::cout << "Average Screen Loops / second     : ";
	std::cout << 1/screen_time_per_loop;
	std::cout << std::endl;
	std::cout << "% Game Logic Free Time            : ";
	std::cout << ((double)screen_ticks/loop_ticks)*100.0;
	std::cout << std::endl;
}

void Game::GameLoop ( void ) {
	uint8_t direction = DIRECTION_STILL;
	bool	cheat = false;

	while ( terrain->getTerrainProgress() && !overFlag){
		clear_keybuf();
		currTime = getTime();
		if ( key[KEY_ESC]){
			overFlag = true;
			break;
		}
		else if (key[KEY_UP]){
			direction = DIRECTION_UP;
		}
		else if (key[KEY_DOWN]){
			direction = DIRECTION_DOWN;
		}
		else if (key[KEY_RIGHT]){
			direction = DIRECTION_RIGHT;
		}
		else if (key[KEY_LEFT]){
			direction = DIRECTION_LEFT;
		}
		else if (key[KEY_PAUSE]){
			this->pause();
			direction = DIRECTION_STILL;
		}
		else if (key[KEY_C]){
			cheat = true;
			direction = DIRECTION_STILL;
		}

		uint64_t t_start = rdtsc();
		terrain->setCheatMode(&cheat);
		terrain->getKeyboard(direction);
		terrain->scheduler();
		terrain->gravityCheck();
		terrain->move();
		terrain->animate();
		terrain->render();

		tmpDiggerLifes = terrain->getLifes();
		tmpScore = terrain->getScore();

		controlGUI->update(tmpScore, tmpDiggerLifes, currLevel+1);
//		logo->animate();

		uint64_t t_screen_st = rdtsc();
		acquire_screen();
		controlGUI->draw();
//		logo->draw();
		draw_sprite( screen, backBuffer, terrainBX->x_LT, terrainBX->y_LT);
		release_screen();
		rest(0);
		uint64_t t_screen_end = rdtsc();
		screen_ticks += (t_screen_end - t_screen_st);
		++screen_loops;
		uint64_t t_end = rdtsc();
		loop_ticks += (t_end - t_start);
		++game_loops;
		direction = DIRECTION_STILL;
	}
}

void Game::GameOverLoop ( uint32_t flag ) {
	BITMAP *tmp = BitmapBuffer->getBitmap(flag, 0, 0);
	uint16_t x, y, tX, tY;
	getScreenResolution(GraphicsMode, &x, &y);
	tX = (x - tmp->w)/2;
	tY = (y - tmp->h)/2;
	do{
		clear_keybuf();
		acquire_screen();
		draw_trans_sprite( screen, tmp, tX, tY);
		release_screen();
		rest(0);
		sleep(3);
	}while(key[KEY_ESC]);
}

void Game::IntroLoop ( void ) {
	uint8_t cnt = 3;
	uint64_t introStartTime;
	uint16_t x, y, tX, tY;
	BITMAP *tmp[3] = {
			BitmapBuffer->getBitmap(ITEM_INTRO_ID, 0, 0),
			BitmapBuffer->getBitmap(ITEM_INTRO_ID, 0, 1),
			BitmapBuffer->getBitmap(ITEM_INTRO_ID, 0, 2)
	};
	getScreenResolution(GraphicsMode, &x, &y);
	introStartTime = getTime();
	tX = (x - tmp[2]->w)/2;
	tY = (y - tmp[2]->h)/2;
	do{
		currTime = getTime();
		if(currTime - introStartTime > 1000000){
			tX = (x - tmp[cnt-1]->w)/2;
			tY = (y - tmp[cnt-1]->h)/2;
			introStartTime = getTime();
			introStartTime = currTime;
			--cnt;
		}
		terrain->animate();
		terrain->render();
		acquire_screen();
		controlGUI->draw();
		draw_sprite( screen, backBuffer, terrainBX->x_LT, terrainBX->y_LT);
		if(cnt){
			draw_trans_sprite( screen, tmp[cnt-1], tX, tY);
		}
		release_screen();
		rest(0);
	}while(cnt);
	currTime = getTime();
}

void Game::initBitMaps ( void ) {
	char tmpBuff[256];
	BITMAP *gapBitmap;
	BITMAP *gapBitmapBig;
	const char *directionPrefix[4] = {
		"_left",
		"_right",
		"_up",
		"_down"
	};

	const char *goldStatePrefix[3] = {
		"_packed",
		"_falling",
		"_unpacked",
	};

	gapBitmap = create_bitmap( 16, 16 );
	clear_to_color(gapBitmap, makecol(80, 50, 0));
	BitmapBuffer->setBitmap(gapBitmap,ITEM_GAP_ID,0,0);

	gapBitmapBig = create_bitmap( 64, 64 );
	clear_to_color(gapBitmapBig, makecol(80, 50, 0));
	BitmapBuffer->setBitmap(gapBitmapBig,ITEM_GAP_ID, 0, 1);

	sprintf(tmpBuff,"%s%s", ITEM_STR_GRAVE_PREFIX, ITEM_STR_POSTFIX);
	BitmapBuffer->setBitmap(tmpBuff, ITEM_GRAVE_ID, 0, 0);

	sprintf(tmpBuff,"%s%s", ITEM_STR_ALPHA_MASK_PREFIX, ITEM_STR_POSTFIX);
	BitmapBuffer->setBitmap(tmpBuff, ITEM_ALPHA_MASK_ID, 0, 0);

	sprintf(tmpBuff,"%s%s%s", ITEM_STR_CNTRL_PREFIX,ITEM_STR_CNTRL_LEVEL_PREFIX,ITEM_STR_POSTFIX);
	BitmapBuffer->setBitmap(tmpBuff, ITEM_LABEL_LEVEL_ID, 0, 0);

	sprintf(tmpBuff,"%s%s%s", ITEM_STR_CNTRL_PREFIX,ITEM_STR_CNTRL_SCORE_PREFIX,ITEM_STR_POSTFIX);
	BitmapBuffer->setBitmap(tmpBuff, ITEM_LABEL_SCORE_ID, 0, 0);

	sprintf(tmpBuff,"%s%s%s", ITEM_STR_CNTRL_PREFIX,ITEM_STR_CNTRL_HEALTH_PREFIX,ITEM_STR_POSTFIX);
	BitmapBuffer->setBitmap(tmpBuff, ITEM_LABEL_HEALTH_ID, 0, 0);

	for(uint8_t i=0; i<10; i++){
		sprintf(tmpBuff,"%s%u%s", ITEM_STR_CNTRL_PREFIX,i,ITEM_STR_POSTFIX);
		BitmapBuffer->setBitmap(tmpBuff, ITEM_LABEL_NUMBER_ID, 0, i);
	}

	sprintf(tmpBuff,"%s%s%s", ITEM_STR_CNTRL_PREFIX,ITEM_STR_GAME_OVER_PREFIX,ITEM_STR_POSTFIX);
	BitmapBuffer->setBitmap(tmpBuff, ITEM_GAME_OVER_ID, 0, 0);

	sprintf(tmpBuff,"%s%s%s", ITEM_STR_CNTRL_PREFIX,ITEM_STR_VICTORY_PREFIX,ITEM_STR_POSTFIX);
	BitmapBuffer->setBitmap(tmpBuff, ITEM_VICTORY_ID, 0, 0);

	sprintf(tmpBuff,"%s%s%s", ITEM_STR_CNTRL_PREFIX,ITEM_STR_PAUSED_PREFIX,ITEM_STR_POSTFIX);
	BitmapBuffer->setBitmap(tmpBuff, ITEM_PAUSED_ID, 0, 0);

	for(uint8_t i=1; i<4; i++){
		sprintf(tmpBuff,"%s%s%u%s", ITEM_STR_CNTRL_PREFIX,ITEM_STR_INTRO_PREFIX,i,ITEM_STR_POSTFIX);
		BitmapBuffer->setBitmap(tmpBuff, ITEM_INTRO_ID, 0, i-1);
	}

	for(uint8_t i=0; i<4; i++){
		for(uint8_t j=0; j<4; j++){
			sprintf(tmpBuff,"%s%u_%u%s", ITEM_STR_SOLID_TERRAIN_PREFIX, i, j, ITEM_STR_POSTFIX);
			BitmapBuffer->setBitmap(tmpBuff,ITEM_SOLID_TERRAIN_ID,i,j);
		}
	}

	for(uint8_t i=ITEM_ROCK_NUM_MIN; i<ITEM_ROCK_NUM_MAX; i++){
		sprintf(tmpBuff,"%s%u%s", ITEM_STR_ROCK_PREFIX, i, ITEM_STR_POSTFIX);
		BitmapBuffer->setBitmap(tmpBuff,ITEM_ROCK_ID,i,0);
	}

	sprintf(tmpBuff,"%s%s%s", ITEM_STR_GOLD_PREFIX, goldStatePrefix[0], ITEM_STR_POSTFIX);
	BitmapBuffer->setBitmap(tmpBuff,ITEM_GOLD_ID,ITEM_GOLD_PACKED_STATE,0);
	for(uint8_t i=ITEM_GOLD_NUM_MIN; i<ITEM_GOLD_NUM_MAX; i++){
		sprintf(tmpBuff,"%s%u%s%s", ITEM_STR_GOLD_PREFIX, i, goldStatePrefix[1], ITEM_STR_POSTFIX);
		BitmapBuffer->setBitmap(tmpBuff,ITEM_GOLD_ID,ITEM_GOLD_FALLING_STATE,i);
	}
	sprintf(tmpBuff,"%s%s%s", ITEM_STR_GOLD_PREFIX, goldStatePrefix[2], ITEM_STR_POSTFIX);
	BitmapBuffer->setBitmap(tmpBuff,ITEM_GOLD_ID,ITEM_GOLD_UNPACKED_STATE,0);

	for(uint8_t i=ITEM_EMERALD_NUM_MIN; i<ITEM_EMERALD_NUM_MAX; i++){
		sprintf(tmpBuff,"%s%u%s", ITEM_STR_EMERALD_PREFIX, i, ITEM_STR_POSTFIX);
		BitmapBuffer->setBitmap(tmpBuff,ITEM_EMERALD_ID,0,i);
	}

	for(uint8_t i=ITEM_RUBY_NUM_MIN; i<ITEM_RUBY_NUM_MAX; i++){
		sprintf(tmpBuff,"%s%u%s", ITEM_STR_RUBY_PREFIX, i, ITEM_STR_POSTFIX);
		BitmapBuffer->setBitmap(tmpBuff,ITEM_RUBY_ID,0,i);
	}

	for(uint8_t i=ITEM_SAPHIRE_NUM_MIN; i<ITEM_SAPHIRE_NUM_MAX; i++){
		sprintf(tmpBuff,"%s%u%s", ITEM_STR_SAPHIRE_PREFIX, i, ITEM_STR_POSTFIX);
		BitmapBuffer->setBitmap(tmpBuff,ITEM_SAPHIRE_ID,0,i);
	}

	for(uint8_t i=0; i<DIRECTIONS; i++){
		for(uint8_t j=ITEM_DIGGER_NUM_MIN; j<ITEM_DIGGER_NUM_MAX; j++){
			sprintf(tmpBuff,"%s%u%s%s", ITEM_STR_DIGGER_PREFIX, j,
					directionPrefix[i], ITEM_STR_POSTFIX);
			BitmapBuffer->setBitmap(tmpBuff,ITEM_DIGGER_ID,i,j);
		}
	}

	for(uint8_t i=0; i<DIRECTIONS; i++){
		for(uint8_t j=ITEM_HOBBLIN_NUM_MIN; j<ITEM_HOBBLIN_NUM_MAX; j++){
			sprintf(tmpBuff,"%s%u%s%s", ITEM_STR_HOBBLIN_PREFIX, j,
					directionPrefix[i], ITEM_STR_POSTFIX);
			BitmapBuffer->setBitmap(tmpBuff,ITEM_HOBBLIN_ID,i,j);
		}
	}

	for(uint8_t i=0; i<DIRECTIONS; i++){
		for(uint8_t j=ITEM_NOBBLIN_NUM_MIN; j<ITEM_NOBBLIN_NUM_MAX; j++){
			sprintf(tmpBuff,"%s%u%s%s", ITEM_STR_NOBBLIN_PREFIX, j,
					directionPrefix[i], ITEM_STR_POSTFIX);
			BitmapBuffer->setBitmap(tmpBuff,ITEM_NOBBLIN_ID,i,j);
		}
	}

	sprintf(tmpBuff,"%s%s", ITEM_STR_BONUS_LIFE_PREFIX, ITEM_STR_POSTFIX);
	BitmapBuffer->setBitmap(tmpBuff,ITEM_BONUS_LIFE_ID,0,0);

	sprintf(tmpBuff,"%s%s", ITEM_STR_BONUS_FATAL_PREFIX, ITEM_STR_POSTFIX);
	BitmapBuffer->setBitmap(tmpBuff,ITEM_BONUS_FATAL_ID,0,0);
/*
	for(uint8_t i=0; i<3; i++){
		sprintf(tmpBuff,"%s%u%s", ITEM_STR_LOGO_PREFIX, i, ITEM_STR_POSTFIX);
		BitmapBuffer->setBitmap(tmpBuff,ITEM_AUTHORS_ID,0,i);
	}
*/
}

void Game::initAnimationFilms ( void ) {
	std::vector<BITMAP *> *animationFilm = new std::vector<BITMAP *>();
	for(uint8_t i=ITEM_EMERALD_NUM_MIN; i<ITEM_EMERALD_NUM_MAX; i++){
		animationFilm->push_back(
				BitmapBuffer->getBitmap(ITEM_EMERALD_ID,0,i)
		);
	}
	animFHolder->setFilm(animationFilm, ITEM_EMERALD_ID, 0);

	animationFilm = new std::vector<BITMAP *>();
	for(uint8_t i=ITEM_RUBY_NUM_MIN; i<ITEM_RUBY_NUM_MAX; i++){
		animationFilm->push_back(
				BitmapBuffer->getBitmap(ITEM_RUBY_ID,0,i)
		);
	}
	animFHolder->setFilm(animationFilm, ITEM_RUBY_ID, 0);

	animationFilm = new std::vector<BITMAP *>();
	for(uint8_t i=ITEM_SAPHIRE_NUM_MIN; i<ITEM_SAPHIRE_NUM_MAX; i++){
		animationFilm->push_back(
				BitmapBuffer->getBitmap(ITEM_SAPHIRE_ID,0,i)
		);
	}
	animFHolder->setFilm(animationFilm, ITEM_SAPHIRE_ID, 0);

	for(uint8_t i=0; i<DIRECTIONS; i++){
		animationFilm = new std::vector<BITMAP *>();
		uint8_t cnt;
		for(uint8_t j=ITEM_DIGGER_NUM_MIN; j<ITEM_DIGGER_NUM_MAX; j++){
			animationFilm->push_back(
					BitmapBuffer->getBitmap(ITEM_DIGGER_ID,i,j)
			);
		}
		cnt = ITEM_DIGGER_NUM_MAX-1;
		for(uint8_t j=ITEM_DIGGER_NUM_MIN; j<ITEM_DIGGER_NUM_MAX; j++, cnt--){
			animationFilm->push_back(
					BitmapBuffer->getBitmap(ITEM_DIGGER_ID,i,cnt)
			);
		}
		animFHolder->setFilm(animationFilm, ITEM_DIGGER_ID, i);
	}

	for(uint8_t i=0; i<DIRECTIONS; i++){
		animationFilm = new std::vector<BITMAP *>();
		for(uint8_t j=ITEM_HOBBLIN_NUM_MIN; j<ITEM_HOBBLIN_NUM_MAX; j++){
			animationFilm->push_back(
					BitmapBuffer->getBitmap(ITEM_HOBBLIN_ID,i,j)
			);
		}
		animFHolder->setFilm(animationFilm, ITEM_HOBBLIN_ID, i);
	}

	for(uint8_t i=0; i<DIRECTIONS; i++){
		animationFilm = new std::vector<BITMAP *>();
		for(uint8_t j=ITEM_NOBBLIN_NUM_MIN; j<ITEM_NOBBLIN_NUM_MAX; j++){
			animationFilm->push_back(
					BitmapBuffer->getBitmap(ITEM_NOBBLIN_ID,i,j)
			);
		}
		animFHolder->setFilm(animationFilm, ITEM_NOBBLIN_ID, i);
	}

	animationFilm = new std::vector<BITMAP *>();
	animationFilm->push_back(
			BitmapBuffer->getBitmap(ITEM_GOLD_ID,ITEM_GOLD_PACKED_STATE,0)
	);
	animFHolder->setFilm(animationFilm, ITEM_GOLD_ID, ITEM_GOLD_PACKED_STATE);
	animationFilm = new std::vector<BITMAP *>();
	for(uint8_t i=ITEM_GOLD_NUM_MIN; i<ITEM_GOLD_NUM_MAX; i++){
		animationFilm->push_back(
				BitmapBuffer->getBitmap(ITEM_GOLD_ID,ITEM_GOLD_FALLING_STATE,i)
		);
	}
	for(uint8_t i=ITEM_GOLD_NUM_MIN, cnt=ITEM_GOLD_NUM_MAX-1; i<ITEM_GOLD_NUM_MAX; i++, cnt--){
		animationFilm->push_back(
				BitmapBuffer->getBitmap(ITEM_GOLD_ID,ITEM_GOLD_FALLING_STATE,cnt)
		);
	}
	animFHolder->setFilm(animationFilm, ITEM_GOLD_ID, ITEM_GOLD_FALLING_STATE);
	animationFilm = new std::vector<BITMAP *>();
	animationFilm->push_back(
			BitmapBuffer->getBitmap(ITEM_GOLD_ID,ITEM_GOLD_UNPACKED_STATE,0)
	);
	animFHolder->setFilm(animationFilm, ITEM_GOLD_ID, ITEM_GOLD_UNPACKED_STATE);
}

void Game::initSounds ( void ) {
	char tmpBuff[256];

	sprintf(tmpBuff,"%s%s%s", ITEM_STR_SOUND_PREFIX, ITEM_STR_SOUND_DEATH, ITEM_STR_SOUND_POSTFIX);
	SoundBuffer->setSample(tmpBuff, ITEM_SOUND_DEATH_ID);

	sprintf(tmpBuff,"%s%s%s", ITEM_STR_SOUND_PREFIX, ITEM_STR_SOUND_SMASH, ITEM_STR_SOUND_POSTFIX);
	SoundBuffer->setSample(tmpBuff, ITEM_SOUND_SMASH_ID);

	sprintf(tmpBuff,"%s%s%s", ITEM_STR_SOUND_PREFIX, ITEM_STR_SOUND_PICK_GOLD, ITEM_STR_SOUND_POSTFIX);
	SoundBuffer->setSample(tmpBuff, ITEM_SOUND_PICK_GOLD_ID);

	sprintf(tmpBuff,"%s%s%s", ITEM_STR_SOUND_PREFIX, ITEM_STR_SOUND_PICK_EMERALD, ITEM_STR_SOUND_POSTFIX);
	SoundBuffer->setSample(tmpBuff, ITEM_SOUND_PICK_EMERALD_ID);
}
