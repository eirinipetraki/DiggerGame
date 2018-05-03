#include <allegro.h>
#include <stdint.h>

#include "../Engine/TerrainAI.h"

#ifndef __EMERALD_H__
#define __EMERALD_H__

class Emerald : public TerrainAI {
private :
	uint16_t value;
	BITMAP	*consumeMask;
	SAMPLE  *sound;
public :
	Emerald ( Terrain *terrain, uint16_t ID, uint16_t value ) ;
	void setConsumeMask ( BITMAP *mask ) ;
	void setConsumeSound ( SAMPLE *sound ) ;
	uint16_t consume ( void ) ;
	~Emerald ( ) ;
};

#endif	/*	__EMERALD_H__	*/
