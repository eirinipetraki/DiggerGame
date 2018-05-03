#include <allegro.h>
#include <stdint.h>

#include "../Common.h"

#ifndef __LOGOTILE_H__
#define __LOGOTILE_H__

#define ANIM_SUCCESSOR(INDEX,MAX) ((INDEX+1)%(MAX))

class LogoTile{
private :
	struct BoundingBox *BX;
	BITMAP   *Authors[3];
	BITMAP   *screenBuffer;
	BITMAP   *backBuffer;
	uint64_t  animationTimer;
	uint16_t GraphicsMode;
	uint16_t X, Y;
	uint8_t  currAnimation;
	bool     refresh;

public :
	LogoTile ( uint16_t mode );
	void setAuthor ( uint8_t index, BITMAP *image );
	void setScreenBuffer ( BITMAP *ScreenBuffer ) ;
	void animate ( void ) ;
	void draw ( void ) ;
	~LogoTile( void ) ;
};
#endif /* __LOGOTILE_H__ */
