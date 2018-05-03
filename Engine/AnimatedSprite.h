#include "Sprite.h"
#include "Animator.h"
#include "AnimationFilmHolder.h"

#ifndef __ANIMATEDSPRITE_H__
#define __ANIMATEDSPRITE_H__

class AnimatedSprite : public Sprite , public Animator {
	AnimationFilmHolder *filmHolder;
public :
	AnimatedSprite ( ) ;
	void animate ( void ) ;
	~AnimatedSprite ( ) ;
};

#endif /* __ANIMATEDSPRITE_H__ */
