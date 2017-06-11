
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_AUDIO_DEVICE_ALSA_H_
#define NCK_AUDIO_DEVICE_ALSA_H_

#include "../nckAudioDevice.h"

#if defined(NCK_ALSA)
#include "nckThread.h"
#include <stdint.h>
#include <errno.h>
#include <poll.h>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <alsa/pcm.h>

_AUDIO_BEGIN

class AlsaBuffer{
public:
	AlsaBuffer(int capacity);
 	~AlsaBuffer();
	
	int size;
	int capacity;
	uint8_t * data;
};

class DeviceAlsaOutput : public virtual OutputDevice, public virtual Core::Threadable
{
public:
	DeviceAlsaOutput();
	DeviceAlsaOutput(const std::string & deviceName,int samplerate, 
				int channels,Format format, int bufferSize, int bufferCount);
	virtual ~DeviceAlsaOutput();

	void Create();
    int64_t GetTime();
	int GetSampleRate();
	int GetChannelsCount();
	Format GetDataFormat();
	void Play();
	void Stop();
	void Pause();
	State GetState();
	Stream * LoadStream(const std::string & filename);
	Stream * LoadStreamFromReader(Core::DataReader * reader);
	void Run();

private:
	std::string m_DeviceName;

	int m_MinBufferSize;
	int m_BufferCount;

	int m_SampleRate;
	int m_Channels;
	Format m_Format;
	
	std::vector<AlsaBuffer*> m_Buffer;
	int m_BufferRead;
	int m_BufferWrite;

	snd_pcm_t *m_AlsaHandle;
	State m_CurrentState;
};

_AUDIO_END

#endif // #if defined(NCK_ALSA)

#endif // #ifndef NCK_AUDIO_DEVICE_ALSA_H_
