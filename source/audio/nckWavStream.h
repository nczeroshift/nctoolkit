
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_WAV_AUDIO_STREAM_H_
#define NCK_WAV_AUDIO_STREAM_H_

#include "nckAudioDevice.h"

_AUDIO_BEGIN

struct WavHeader;

class WavStream:public virtual Stream, public virtual SeekStream{
public:
    WavStream(Core::DataReader * srcReader);
	virtual ~WavStream();
	void Init();
	int GetSampleRate();
	int GetChannelsCount();
	Format GetFormat();
	int64_t GetDuration();
	int64_t GetTime();
	void SetTime(int64_t position);
	int64_t Seek(int64_t position);
	int Read(int dataSize, uint8_t * dataBuffer); 
	static WavStream * Load(const std::string & filename);
    static WavStream * Load(Core::DataReader * reader);
private:
    void updateGranulate(int dataSize);
    int64_t headerOffset;
	int64_t currentTime;
	int64_t fLength;
	Core::DataReader * reader;
    WavHeader * header;
};

class WavWriter {
public:
    WavWriter(Core::DataWriter * writer);
    ~WavWriter();

    void SetSampleRate(int sampleRate);
    void SetChannelsCount(int count);
    void SetFormat(Format format);
    void Write(uint8_t * data, int size);
    void Finish();
private:
    uint64_t headerOffset;
    Format format;
    WavHeader * header;
    Core::DataWriter * writer;
    int64_t length;
};

_AUDIO_END 

#endif // #ifndef NCK_WAV_AUDIO_STREAM_H_
