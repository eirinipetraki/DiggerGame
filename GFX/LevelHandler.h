#include <stdint.h>

#ifndef __LEVEL_HANDLER_H__
#define __LEVEL_HANDLER_H__

#define MAX_LEVELS 8

void getBotsPreCached (uint16_t level, uint8_t *n, uint8_t *h) ;
uint8_t getItemAt (uint16_t mode, uint16_t level, uint16_t x, uint16_t y) ;

#endif /* __LEVEL_HANDLER_H__ */
