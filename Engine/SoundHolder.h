#include <allegro.h>
#include <stdint.h>
#include <map>

#include "../Common.h"

#ifndef __SOUND_HOLDER_H__
#define __SOUND_HOLDER_H__

class SoundHolder {
private :
	std::map<uint16_t, SAMPLE *, uint16_comparator> SoundBitmaps;
public :
	void setSample ( const char *samplePath, uint16_t sampleID );
	SAMPLE *getSample ( uint16_t sampleID );
};

#endif /* __SOUND_HOLDER_H__ */
