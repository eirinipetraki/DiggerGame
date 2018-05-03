#include <stdint.h>
#include <allegro.h>

#include <vector>
#include <map>

#include "AnimationFilmHolder.h"

std::vector<BITMAP *> *AnimationFilmHolder::getFilm ( uint16_t objectID, uint16_t filmID ) {
	uint32_t key = ((uint32_t)objectID << 16) | (uint32_t)filmID;
	return Films[key];
}

void AnimationFilmHolder::setFilm (
		std::vector<BITMAP *> *film,
		uint16_t objectID,
		uint16_t filmID )
{
	uint32_t key = ((uint32_t)objectID << 16) | (uint32_t)filmID;
	Films[key] = film;
}

uint32_t AnimationFilmHolder::size ( void ) {
	return Films.size();
}

AnimationFilmHolder::AnimationFilmHolder ( ) { }

AnimationFilmHolder::~AnimationFilmHolder ( ) {
	std::map<uint32_t, std::vector<BITMAP *> *, uint32_comparator>::iterator iter;

	for( iter=Films.begin(); iter!=Films.end(); iter++ ){
		iter->second->clear();
	}
	Films.clear();
}
