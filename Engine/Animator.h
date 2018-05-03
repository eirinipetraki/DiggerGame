#include <stdint.h>
#include <allegro.h>

#include <vector>

#ifndef __ANIMATOR_H__
#define __ANIMATOR_H__

#define ANIM_SUCCESSOR(INDEX,MAX) ((INDEX+1)%(MAX))
#define ANIM_PREDECESSOR(INDEX,MAX) ((INDEX+MAX-1)%(MAX))

class Animator {
protected :
	std::vector<BITMAP *> *animationFilm;
	uint8_t animatorCurrent;
	bool animatorActive;

public :
	Animator ( ) ;
	void setFilm ( std::vector<BITMAP *> *newFilm ) ;
	void start ( void ) ;
	void stop ( void ) ;
	void reset ( void ) ;
	~Animator ( ) ;

	virtual void animate ( void ) = 0;
};

#endif /* __ANIMATOR_H__ */
