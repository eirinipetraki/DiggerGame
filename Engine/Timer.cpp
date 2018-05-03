#include <iostream>
#include <fstream>
#include <string>

#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#include "Timer.h"

struct timeval tmp;

#if defined(__i386__)
uint64_t rdtsc(void) {
	uint64_t val;
	asm volatile (" rdtsc " : "=A" (val));
	return val;
}
#elif defined(__x86_64__)
uint64_t rdtsc(void)
{
	uint64_t hi, lo;
	__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
	return ( (uint64_t)lo)|( ((uint64_t)hi)<<32 );
}
#else
#error "*** Incompatible Architecture ***"
#endif

using namespace std;

uint32_t cpufreq( void ){
	ifstream ifs("/proc/cpuinfo");
	string tmpstr;
	string strfreq;
	float realfreq;

	if(ifs.fail()){
		cerr << "Failed to open file : /proc/cpuinfo" << endl;
	}
	while( getline( ifs, tmpstr ) ){
		if(strstr(tmpstr.c_str(), "cpu MHz")){
			strfreq = tmpstr.substr(tmpstr.find_last_of(": ")+1, tmpstr.length());
			realfreq = atof(strfreq.c_str());
			return (uint32_t)realfreq;
		}
	}
	return 0;
}

uint64_t getTime ( void ) {
	gettimeofday(&tmp, NULL);
	return (tmp.tv_sec * 1000000 + tmp.tv_usec);
}
