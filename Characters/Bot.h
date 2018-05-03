#include <stdint.h>

#include "Character.h"
#include "../Common.h"

#ifndef __BOT_H__
#define __BOT_H__

class Bot : public Character {
public :
	uint8_t selectDirection ( struct BoundingBox *target, uint8_t dirs[4] ) ;
	bool isOpposite ( uint8_t direction) ;
	void move ( struct BoundingBox *target ) ;
	void die ( void ) ;
	Bot ( Terrain *terrain, uint16_t ID ) ;
	~Bot ( ) ;
private :
	virtual bool collisionDetect ( uint8_t direction ) = 0;
	virtual void animationFilmChange ( void ) = 0;
	virtual void handleCollision ( void ) = 0;
};

#endif /* __BOT_H__ */
