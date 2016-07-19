
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_AUDIO_DEVICE_H_
#define NCK_AUDIO_DEVICE_H_

#include "nckAudioConfig.h"
#include "nckDataIO.h"
#include <vector>

_AUDIO_BEGIN

enum State{
	STATE_PLAYING = 0,
	STATE_PAUSED,
	STATE_STOPPED,
};

enum Format{
	FORMAT_S8 = 0,
	FORMAT_U8,
	FORMAT_S16,
	FORMAT_U16,
	FORMAT_S32,
	FORMAT_U32,
	FORMAT_FLOAT,
};

enum Type{
	DEVICE_OUTPUT = 0,
	DEVICE_INPUT
};

class Device;

/**
* Abstract class for an audio data stream.
*/
class Stream{
public:
	virtual ~Stream(){};
	virtual int GetSampleRate() = 0;
	virtual int GetChannelsCount() = 0;
	virtual Format GetFormat() = 0;
	virtual int Read(int dataSize, uint8_t * dataBuffer) = 0;
};

class SeekStream{
public:
	virtual ~SeekStream(){};
	virtual int64_t GetDuration() = 0;
	virtual int64_t GetTime() = 0;
	virtual void SetTime(int64_t position) = 0;
};

/**
* Abstract class for an audio device callback event.
*/
class DeviceCallback{
public:
	virtual ~DeviceCallback(){};
	virtual int Buffer(Device * dev,int bufferSize,uint8_t * bufferData){return 0;};
};

/**
* Abstract class for an audio device.
*/
class Device{
public:
	virtual ~Device(){};
	
    virtual int64_t GetTime() = 0;
	virtual int GetSampleRate() = 0;
	virtual int GetChannelsCount() = 0;
	virtual Format GetDataFormat() = 0;

	virtual void Stop() = 0;
	virtual void Pause() = 0;

	virtual State GetState() = 0;

	void SetCallback(DeviceCallback * delegate);
	DeviceCallback * GetCallback();

	void GetBlockAndBitsPerSample(Format format, int channels, int * block, int * bits);
protected:
	DeviceCallback * m_Callback;
};

/**
* Abstract class for an audio output device.
*/
class OutputDevice : public Device{
public:
	virtual ~OutputDevice(){};

	virtual void Play() = 0;
	
	virtual Stream * LoadStream(const std::string & filename) = 0;
	virtual Stream * LoadStreamFromReader(Core::DataReader * reader) = 0;
};

/**
* Factory constructor for the audio devices.
*/
Device * CreateDevice(Type type, 
						int samplerate = 48000, 
						int channels = 2, 
						Format format = FORMAT_S16, 
						int bufferSize = 4096, 
						int bufferCount = 2);

_AUDIO_END 

#endif // #ifndef NCK_AUDIO_DEVICE_H_
