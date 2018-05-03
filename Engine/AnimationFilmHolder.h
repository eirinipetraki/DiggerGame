#include <stdint.h>
#include <allegro.h>

#include <vector>
#include <map>

#include "../Common.h"

#ifndef __ANIMATION_FILM_HOLDER_H__
#define __ANIMATION_FILM_HOLDER_H__

class AnimationFilmHolder {
private :
	std::map<uint32_t, std::vector<BITMAP *> *, uint32_comparator> Films;
public :
	AnimationFilmHolder() ;
	void setFilm (	std::vector<BITMAP *> *film, uint16_t objectID, uint16_t filmID ) ;
	std::vector<BITMAP *> *getFilm ( uint16_t objectID, uint16_t filmID ) ;
	uint32_t size ( void ) ;
	~AnimationFilmHolder() ;
};

#endif	/*	__ANIMATION_FILM_HOLDER_H__	*/
