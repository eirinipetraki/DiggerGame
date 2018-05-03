#include <stdint.h>

#include "../Engine/TerrainAI.h"

#ifndef __BONUS_ITEM_H__
#define __BONUS_ITEM_H__

#define ITEM_TYPE_LIFE		0
#define ITEM_TYPE_FATAL		1

class BonusItem : public TerrainAI {
private :
	uint16_t type;
	BITMAP	*consumeMask;
public :
	BonusItem ( Terrain *terrain, uint16_t ID, uint16_t type ) ;
	void setConsumeMask ( BITMAP *mask ) ;
	uint16_t consume ( void ) ;
	void draw ( void ) ;
	~BonusItem ( ) ;
};

#endif	/*	__BONUS_ITEM_H__	*/
