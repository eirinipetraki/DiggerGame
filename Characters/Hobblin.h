#include <stdint.h>

#include "../Common.h"
#include "Bot.h"

#ifndef __HOBBLIN_H__
#define __HOBBLIN_H__

class Hobblin : public Bot {
private :
	Sprite		**tmp[4];
	bool		terrainCollision;

public :
	Hobblin ( Terrain *terrain, uint16_t ID ) ;
	~Hobblin ( ) ;
private :
	void animationFilmChange ( void ) ;
	bool collisionDetect ( uint8_t direction ) ;
	void handleCollision ( void ) ;
};

#endif /* __HOBBLIN_H__ */
