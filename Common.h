#include <stdint.h>

#ifndef __COMMON_H__
#define __COMMON_H__

#define DIRECTIONS			4

#define	DIRECTION_LEFT		0
#define	DIRECTION_RIGHT		1
#define	DIRECTION_UP		2
#define	DIRECTION_DOWN		3
#define	DIRECTION_STILL		4

#define MOVE_ANIM_STEP		2
#define MOVE_ANIM_STEPS_REQ	8

#define CHARACTER_PIXELS	64
#define TERRAIN_PIXELS		16
#define SUB_GRID_ELEMS_D	4


#define GRID_WIDTH			80
#define GRID_HEIGHT			50
#define TERRAIN_GRID_WIDTH	64
#define TERRAIN_GRID_HEIGHT	40

#define DIGGER_LIFES		3

#define CACHE_LINE			64

#define ASSERTIONS_ENABLED

struct BoundingBox{
	uint16_t x_LT;	//	Top Left x coordinate.
	uint16_t y_LT;	//	Top Left y coordinate.
	uint16_t x_RB;	//	Bottom Right x coordinate.
	uint16_t y_RB;	//	Bottom Right y coordinate.
} __attribute__ ((aligned(CACHE_LINE)));

struct uint32_comparator {
	bool operator()(const uint32_t Key1, const uint32_t Key2) const {
		return Key1 < Key2;
	}
} __attribute__ ((aligned(CACHE_LINE)));

struct uint16_comparator {
	bool operator()(const uint16_t Key1, const uint16_t Key2) const {
		return Key1 < Key2;
	}
} __attribute__ ((aligned(CACHE_LINE)));

#endif /* __COMMON_H__ */
