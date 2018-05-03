#include "../Engine/Sprite.h"

#ifndef __CONTROL_TILE_H__
#define __CONTROL_TILE_H__

class ControlTile {
private:
	struct BoundingBox *BX;
	BITMAP   *screenBuffer;
	BITMAP   *backBuffer;
	BITMAP   *LevelBitmap;
	BITMAP   *ScoreBitmap;
	BITMAP   *LifeBitmap;
	BITMAP   *Numbers[10];
	BITMAP   *BlackTile;
	uint16_t X, Y;
	uint16_t GraphicsMode;
	uint16_t offset;
	uint16_t levelPos;
	uint32_t score;
	uint8_t  lifes;
	uint8_t  level;
	bool	 refresh;

public :
	ControlTile (
			uint16_t mode,
			BITMAP *LevelBitmap,
			BITMAP *ScoreBitmap,
			BITMAP *LifeBitmap
	);
	void update ( uint32_t score, uint8_t lifes, uint8_t level ) ;
	void setNumberBuffer ( uint8_t index, BITMAP *number ) ;
	void setScreenBuffer ( BITMAP *ScreenBuffer ) ;
	void draw ( void ) ;
	~ControlTile ( ) ;
private :
	void updateLifes ( bool flag ) ;
	void updateScore ( bool flag ) ;
	void updateLevel ( bool flag ) ;
};

#endif	/*	__CONTROL_TILE_H__	*/
