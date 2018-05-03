#ifndef __GFX_H__
#define __GFX_H__

#define MAX_SCREEN_RESOLUTIONS	8

bool getBannerPosition (uint16_t mode, uint16_t *x, uint16_t *y);
bool getBannerResolution (uint16_t mode, uint16_t *x_LT, uint16_t *y_LT, uint16_t *x_RB, uint16_t *y_RB);
bool getScreenResolution (uint16_t mode, uint16_t *x, uint16_t *y) ;
bool getTerrainResolution (uint16_t mode, uint16_t *x, uint16_t *y) ;
bool getTerrainTiles_16x16 (uint16_t mode, uint16_t *x, uint16_t *y) ;
bool getTerrainTiles_64x64 (uint16_t mode, uint16_t *x, uint16_t *y) ;
bool getTerrainPosition (uint16_t mode, uint16_t *x, uint16_t *y) ;

#endif /* __GFX_H__ */
