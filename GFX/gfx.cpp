#include <stdint.h>

#include "gfx.h"

const uint16_t screenResolution[MAX_SCREEN_RESOLUTIONS][2] = {
		{800, 600},		// Complete
		{1024, 768},	// Complete
		{1280, 800},	// Complete
		{1280, 1024},
		{1600, 1200},
		{1920, 1080},
		{1920, 1200},
		{2560, 1600}
};

const uint16_t terrainResolution[MAX_SCREEN_RESOLUTIONS][2] = {
		{768, 448},		// Complete
		{1024, 640},	// Complete
		{1024, 640},	// Complete
		{1280, 1024},
		{1600, 1200},
		{1920, 1080},
		{1920, 1200},
		{2560, 1600}
};

const uint16_t terrainTiles_64x64[MAX_SCREEN_RESOLUTIONS][2] = {
		{12, 7},		// Complete
		{16, 10},		// Complete
		{16, 10},		// Complete
		{1280, 1024},
		{1600, 1200},
		{1920, 1080},
		{1920, 1200},
		{2560, 1600}
};

const uint16_t terrainTiles_16x16[MAX_SCREEN_RESOLUTIONS][2] = {
		{48, 28},		// Complete
		{64, 40},		// Complete
		{64, 40},		// Complete
		{1280, 1024},
		{1600, 1200},
		{1920, 1080},
		{1920, 1200},
		{2560, 1600}
};

const uint16_t terrainPosition[MAX_SCREEN_RESOLUTIONS][2] = {
		{16, 76},		// Complete
		{0, 64},		// Complete
		{128, 80},		// Complete
		{1280, 1024},
		{1600, 1200},
		{1920, 1080},
		{1920, 1200},
		{2560, 1600}
};

const uint16_t bannerPositionTable[MAX_SCREEN_RESOLUTIONS][2] = {
		{16, 12},		// Complete
		{0, 0},			// Complete
		{128, 16},		// Complete
		{1280, 1024},
		{1600, 1200},
		{1920, 1080},
		{1920, 1200},
		{2560, 1600}
};

const uint16_t bannerDimensionTable[MAX_SCREEN_RESOLUTIONS][4] = {
		{0, 0, 768, 64},		// Complete
		{0, 0, 1024, 64},		// Complete
		{0, 0, 1024, 64},		// Complete
		{1280, 1024},
		{1600, 1200},
		{1920, 1080},
		{1920, 1200},
		{2560, 1600}
};

bool getBannerPosition (uint16_t mode, uint16_t *x, uint16_t *y) {
	if(mode <= MAX_SCREEN_RESOLUTIONS){
		*x = bannerPositionTable[mode][0];
		*y = bannerPositionTable[mode][1];
		return true;
	}
	return false;
}

bool getBannerResolution (uint16_t mode, uint16_t *x_LT, uint16_t *y_LT, uint16_t *x_RB, uint16_t *y_RB) {
	if(mode <= MAX_SCREEN_RESOLUTIONS){
		*x_LT = bannerDimensionTable[mode][0];
		*y_LT = bannerDimensionTable[mode][1];
		*x_RB = bannerDimensionTable[mode][2];
		*y_RB = bannerDimensionTable[mode][3];
		return true;
	}
	return false;
}

bool getScreenResolution (uint16_t mode, uint16_t *x, uint16_t *y){
	if(mode <= MAX_SCREEN_RESOLUTIONS){
		*x = screenResolution[mode][0];
		*y = screenResolution[mode][1];
		return true;
	}
	return false;
}

bool getTerrainResolution (uint16_t mode, uint16_t *x, uint16_t *y){
	if(mode <= MAX_SCREEN_RESOLUTIONS){
		*x = terrainResolution[mode][0];
		*y = terrainResolution[mode][1];
		return true;
	}
	return false;
}

bool getTerrainTiles_64x64 (uint16_t mode, uint16_t *x, uint16_t *y){
	if(mode <= MAX_SCREEN_RESOLUTIONS){
		*x = terrainTiles_64x64[mode][0];
		*y = terrainTiles_64x64[mode][1];
		return true;
	}
	return false;
}

bool getTerrainTiles_16x16 (uint16_t mode, uint16_t *x, uint16_t *y){
	if(mode <= MAX_SCREEN_RESOLUTIONS){
		*x = terrainTiles_16x16[mode][0];
		*y = terrainTiles_16x16[mode][1];
		return true;
	}
	return false;
}

bool getTerrainPosition (uint16_t mode, uint16_t *x, uint16_t *y){
	if(mode <= MAX_SCREEN_RESOLUTIONS){
		*x = terrainPosition[mode][0];
		*y = terrainPosition[mode][1];
		return true;
	}
	return false;
}
