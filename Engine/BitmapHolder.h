#include <allegro.h>
#include <stdint.h>
#include <map>

#include "../Common.h"

#ifndef __BITMAP_HOLDER_H__
#define __BITMAP_HOLDER_H__

class BitmapHolder {
private :
	std::map<uint32_t, BITMAP *, uint32_comparator> GameBitmaps;
public :
	void setBitmap (BITMAP *bitmap,uint16_t objectID,uint8_t filmID,uint8_t frameID);
	void setBitmap (const char *bitmapPath, uint16_t objectID, uint8_t filmID, uint8_t frameID);
	BITMAP *getBitmap (uint16_t objectID, uint8_t filmID, uint8_t frameID);
	uint32_t size ( void ) ;
	BitmapHolder ( ) ;
	~BitmapHolder ( ) ;
};

#endif /* __BITMAP_HOLDER_H__ */
