#include <iostream>
#include <stdint.h>
#include <allegro.h>

#include <vector>

#include "AnimatedSprite.h"
#include "Sprite.h"
#include "Animator.h"

AnimatedSprite::AnimatedSprite ( ) {}

AnimatedSprite::~AnimatedSprite ( ) {}

void AnimatedSprite::animate ( void ) {
	if(animatorActive){
		animatorCurrent = ANIM_SUCCESSOR(animatorCurrent, animationFilm->size());
		spriteBitmap = animationFilm->at(animatorCurrent);
		refresh = true;
	}
}
