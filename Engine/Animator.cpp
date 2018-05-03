#include <stdint.h>
#include <allegro.h>

#include <vector>
#include "Animator.h"

void Animator::setFilm ( std::vector<BITMAP *> *newFilm ) {
	animationFilm = newFilm;
}

void Animator::start ( void ) {
	animatorActive = true;
}

void Animator::stop ( void ) {
	animatorActive = false;
}

void Animator::reset ( void ) {
	animatorCurrent = 0;
	animatorActive = false;
}

Animator::Animator ( ) {
	animatorCurrent = 0;
	animatorActive = false;
}

Animator::~Animator ( ) {}
