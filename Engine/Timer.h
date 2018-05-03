#include <stdint.h>

#ifndef __TIMER_H__
#define __TIMER_H__

uint64_t rdtsc ( void ) ;
uint32_t cpufreq ( void ) ;

/*
 * Game time in u-sec's.
 * CPU frequency independent.
 */
uint64_t getTime ( void ) ;

#endif /* __TIMER_H__ */
