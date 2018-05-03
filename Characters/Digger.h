#include <stdint.h>
#include <allegro.h>
#include "Character.h"

#ifndef __DIGGER_H__
#define __DIGGER_H__

class Digger : public Character {
private :
	Sprite		**tmp[4];
	BITMAP		*grave;
	SAMPLE		*sound;
	uint64_t	lockTime;
	uint64_t	fatalPitch;
	uint32_t	score;
	bool		fatal;
	bool		terrainCollision;
	bool		lock;

public :
	void reset ( uint16_t x, uint16_t y, uint16_t dim, uint8_t direction ) ;
	void move (  uint8_t direction ) ;
	void setGrave (BITMAP *grave);
	bool isFatal ( void ) ;
	void die ( void ) ;
	void setLifes ( uint8_t lifes ) ;
	uint8_t getLifes ( void ) ;
	void setScore ( uint32_t score ) ;
	uint32_t getScore ( void) ;
	void setDeathSound ( SAMPLE *sound ) ;
	void animate ( void ) ;
	void setCheat ( void ) ;
	Digger ( Terrain *terrain, uint16_t ID ) ;
	~Digger ( ) ;
private :
	void animationFilmChange ( void ) ;
	bool collisionDetect ( uint8_t direction ) ;
	void handleCollision ( void ) ;
};

#endif /* __DIGGER_H__ */
