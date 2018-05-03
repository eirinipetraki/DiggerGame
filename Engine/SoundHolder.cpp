#include <allegro.h>
#include <stdint.h>
#include <stdio.h>
#include <map>

#include "SoundHolder.h"
#include "../Common.h"

void SoundHolder::setSample ( const char *samplePath, uint16_t sampleID ){
#if defined(ASSERTIONS_ENABLED)
	SAMPLE *tmp = load_sample( samplePath);
	if(tmp == NULL){
		fprintf(stderr, "Error Load Sound : Path=[%s], SampleID=[%u]\n",
				samplePath, sampleID
		);
	}
	SoundBitmaps[sampleID] = tmp;
#else
	SAMPLE *tmp = load_sample( samplePath);
	SoundBitmaps[sampleID] = tmp;
#endif
}

SAMPLE *SoundHolder::getSample ( uint16_t sampleID ){
	return SoundBitmaps[sampleID];
}
