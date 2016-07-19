
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_DEVICE_WAVEOUT_H
#define NCK_DEVICE_WAVEOUT_H

#include "../nckAudioDevice.h"

#if defined(NCK_WAVEOUT)
#include <Windows.h>
#include <mmsystem.h>

#include "nckDataIO.h"
#include "nckException.h"
#include "nckThread.h"
#include "nckUtils.h"

_AUDIO_BEGIN

class DeviceWaveOut : public virtual OutputDevice, public virtual Core::Threadable{
public:
	DeviceWaveOut();
	DeviceWaveOut(int sampleRate, int channels, Format format,int buffers, int buffersSize);
	~DeviceWaveOut();
	
    int64_t GetTime();
	int GetSampleRate();
	int GetChannelsCount();
	Format GetDataFormat();
	void Create();
	void Stop();
	void Pause();
	State GetState();
	void Play();	
	void Run();

	Stream * LoadStream(const std::string & filename);
	Stream * LoadStreamFromReader(Core::DataReader * reader);

private:
	HWAVEOUT m_Device;
    std::vector<WAVEHDR*> m_Buffers;

	int m_SampleRate;
	int m_Channels;
	Format m_Format;
	
	int m_BufferCount;
	int m_BufferSize;

	State m_CurrentState;
};

_AUDIO_END

#endif // #if defined(NCK_WAVEOUT)

#endif
