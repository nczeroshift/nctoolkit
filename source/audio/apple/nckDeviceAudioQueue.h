
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_DEVICE_AUDIO_QUEUE_H
#define NCK_DEVICE_AUDIO_QUEUE_H

#include "../nckAudioDevice.h"

#if defined(NCK_AUDIOQUEUE)

#include "nckDataIO.h"
#include "nckException.h"
#include "nckThread.h"
#include "nckUtils.h"

#import <AudioToolbox/AudioToolbox.h>

_AUDIO_BEGIN

class DeviceAudioQueue : public virtual OutputDevice{
public:
	DeviceAudioQueue(int sampleRate, int channels, Format format,int buffers, int buffersSize);
	~DeviceAudioQueue();
	
	int GetSampleRate();
	int GetChannelsCount();
	Format GetDataFormat();
	void Create();
	void Stop();
	void Pause();
	State GetState();
	void Play();	

    int64_t GetTime();
	Stream * LoadStream(const std::string & filename);
	Stream * LoadStreamFromReader(Core::DataReader * reader);

private:

	int m_SampleRate;
	int m_Channels;
	Format m_Format;
	
	int m_BufferCount;
	int m_BufferSize;

	State m_CurrentState;
    
    AudioStreamBasicDescription   mDataFormat;
    AudioQueueRef                 mQueue;
    AudioQueueBufferRef           *mBuffers;
    AudioStreamPacketDescription  *mPacketDescs;
};

_AUDIO_END

#endif // #if defined(NCK_AUDIOQUEUE)

#endif
