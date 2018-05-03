#include <stdint.h>

#include "../Characters/Character.h"
#include "../Engine/TerrainAI.h"

#ifndef __GOLD_H__
#define __GOLD_H__

class Gold : public TerrainAI {
private :
	Sprite	**tmp[4];
	BITMAP	*darkenTerrain;
	BITMAP	*consumeMask;
	SAMPLE  *c_sound;
	SAMPLE  *s_sound;
	uint16_t value;
	uint16_t direction;
	uint16_t startFallingY;
	uint8_t  mvAnimationProgress;
	uint8_t  fallAnimationProgress;
	bool	 packed;
	bool	 isFalling;
	bool	 triggerFall;

public :
	Gold ( Terrain *terrain, uint16_t ID, uint16_t value ) ;
	bool	 isPacked ( void ) ;
	uint16_t consume ( void ) ;
	void	 setDarkTerrainMask ( BITMAP *mask );
	void	 setConsumeMask ( BITMAP *mask );
	void	 setConsumeSound ( SAMPLE *sound ) ;
	void	 setUnpackSound ( SAMPLE *sound ) ;
	bool	 isPushable ( Character *character, uint16_t direction ) ;
	void	 push ( Character *character, uint16_t direction ) ;
	void	 animate ( void ) ;
	void	 draw ( void ) ;
	void	 startFall ( void ) ;
	void	 fall ( void ) ;
	~Gold ( ) ;
private :
	bool	 CollisionDetect ( void ) ;
	bool	 groundCollisionDetect ( void ) ;
	void	 HandleCollision ( void  ) ;
	void	 unPack ( void ) ;
};

#endif	/*	__GOLD_H__	*/
