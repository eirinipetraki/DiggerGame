#include <stdint.h>
#include <allegro.h>

#include "../Common.h"
#include "../Engine/TerrainAI.h"

#ifndef __CHARACTER_H__
#define __CHARACTER_H__

class Character : public TerrainAI {
protected :
	BITMAP		*darkenTerrain;
	uint8_t		mvAnimationProgress;
	uint8_t		direction;
	uint8_t		lifes;

public :
	Character ( Terrain *terrain, uint16_t ID ) ;
	void setPosition ( uint16_t x, uint16_t y ) ;
	void setDirection ( uint8_t direction ) ;
	uint8_t getDirection ( void ) ;
	void setLifes ( uint8_t lifes ) ;
	void setDarkTerrainMask ( BITMAP *mask ) ;
	virtual void animate ( void ) ;
	void draw ( void ) ;
	virtual void die ( void ) = 0;
	~Character ( ) ;

protected :
	virtual bool collisionDetect ( uint8_t direction ) = 0 ;
	virtual void animationFilmChange ( void ) = 0 ;
	virtual void handleCollision ( void ) = 0 ;
};

#endif /* __CHARACTER_H__ */
