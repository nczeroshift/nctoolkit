
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_AUDIO_STREAM_H_
#define NCK_AUDIO_STREAM_H_

#include "nckAudioDevice.h"

#ifdef NCK_OGG

#include <vorbis/codec.h>

_AUDIO_BEGIN

class OggStream:public virtual Stream, public virtual SeekStream{
public:
	OggStream(Core::DataReader * srcReader);
	virtual ~OggStream();
	void Init();
	int GetSampleRate();
	int GetChannelsCount();
	Format GetFormat();
	int64_t GetDuration();
	int64_t GetTime();
	void SetTime(int64_t position);
	int64_t Seek(int64_t position);
	int Read(int dataSize, uint8_t * dataBuffer); 
	static OggStream * Load(const std::string & filename);
    static OggStream * Load(Core::DataReader * reader);
private:
	int64_t currentTime;
	int64_t fLength;
	Core::DataReader * reader;
	ogg_sync_state   oy;
	ogg_stream_state os;
	ogg_page         og;
	ogg_packet       op;

	vorbis_info      vi;
	vorbis_comment   vc;
	vorbis_dsp_state vd;
	vorbis_block     vb;
};

_AUDIO_END 

#endif	// #ifdef NCK_OGG

#endif // #ifndef NCK_AUDIO_STREAM_H_
