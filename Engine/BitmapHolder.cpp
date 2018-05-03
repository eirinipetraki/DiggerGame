#include <allegro.h>
#include <stdint.h>
#include <stdio.h>
#include <map>

#include "../Common.h"
#include "BitmapHolder.h"

void BitmapHolder::setBitmap (
		const char *bitmapPath,
		uint16_t objectID,
		uint8_t filmID,
		uint8_t frameID
){
#if defined(ASSERTIONS_ENABLED)
	uint32_t key = (((((uint32_t)objectID << 8) | (uint32_t)filmID) << 8) | (uint32_t)frameID);
	BITMAP *tmp = load_bitmap( bitmapPath, NULL);
	GameBitmaps[key] = tmp;
	if(tmp == NULL){
		fprintf(stderr, "Error Load Bitmap : Path=[%s],"
				"ObjectID=[%u], FilmID=[%u], FrameID=[%u]\n",
				bitmapPath, objectID, filmID, frameID
		);
	}
#else
	uint32_t key = (((((uint32_t)objectID << 8) | (uint32_t)filmID) << 8) | (uint32_t)frameID);
	GameBitmaps[key] = load_bitmap( bitmapPath, NULL);
#endif
}

void BitmapHolder::setBitmap (
		BITMAP *bitmap,
		uint16_t objectID,
		uint8_t filmID,
		uint8_t frameID
){
	uint32_t key = (((((uint32_t)objectID << 8) | (uint32_t)filmID) << 8) | (uint32_t)frameID);
	GameBitmaps[key] = bitmap;
}

BITMAP *BitmapHolder::getBitmap (
		uint16_t objectID,
		uint8_t filmID,
		uint8_t frameID
){
	uint32_t key = (((((uint32_t)objectID << 8) | (uint32_t)filmID) << 8) | (uint32_t)frameID);
	return GameBitmaps[key];
}

uint32_t BitmapHolder::size ( void ) {
	return GameBitmaps.size();
}

BitmapHolder::BitmapHolder ( ) { }

BitmapHolder::~BitmapHolder ( ) {
	std::map<uint32_t, BITMAP *, uint32_comparator>::iterator iter;

	for( iter=GameBitmaps.begin(); iter!=GameBitmaps.end(); iter++ ){
		destroy_bitmap(iter->second);
	}
	GameBitmaps.clear();
}
