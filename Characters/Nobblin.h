#include <stdint.h>

#include "../Common.h"
#include "Bot.h"

#ifndef __NOBBLIN_H__
#define __NOBBLIN_H__

class Nobblin : public Bot {
private :
	Sprite		**tmp[4];
	bool		terrainCollision;

public :
	Nobblin ( Terrain *terrain, uint16_t ID ) ;
	~Nobblin ( ) ;
private :
	void animationFilmChange ( void ) ;
	bool collisionDetect ( uint8_t direction ) ;
	void handleCollision ( void ) ;
};

#endif /* __NOBBLIN_H__ */
