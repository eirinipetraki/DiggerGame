#include <stdint.h>
#include <allegro.h>

#include "../Common.h"

#ifndef __SPRITE_H__
#define __SPRITE_H__

class Sprite {
protected :
	struct BoundingBox *BX;
	BITMAP	*spriteBitmap;
	BITMAP	*terrainMap;
	uint8_t ClassID;
	bool	refresh;
	bool	CD_Aware;

public :
	Sprite ( ) ;
	void setClassID ( uint8_t ID );
	uint8_t getClassID ( void );
	void set ( uint16_t x, uint16_t y, uint16_t dim );
	void registerMap ( BITMAP *newBitmap ) ;
	void setBitmap ( BITMAP *newBitmap ) ;
	bool CD_Enabled ( void ) ;
	void set_CD ( bool CD_Aware ) ;
	virtual void draw ( void ) ;
	~Sprite( ) ;
};

#endif /* __SPRITE_H__ */
