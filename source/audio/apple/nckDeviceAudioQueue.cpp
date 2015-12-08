
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckDeviceAudioQueue.h"
#include "../nckOggStream.h"

#if defined(NCK_AUDIOQUEUE)

_AUDIO_BEGIN

DeviceAudioQueue::DeviceAudioQueue() : Threadable() {
	m_SampleRate = 44100;
	m_Channels = 2;
	m_Format = FORMAT_S16;
}

DeviceAudioQueue::DeviceAudioQueue(int sampleRate, int channels, Format format,int bufferSize, int bufferCount) : Threadable() {
	m_SampleRate = sampleRate;
	m_Channels = channels;
	m_Format = format;
	m_BufferCount = bufferCount;
	m_BufferSize = bufferSize;
}

DeviceAudioQueue::~DeviceAudioQueue(){
	Stop();
}
	
int DeviceAudioQueue::GetSampleRate(){
	return m_SampleRate;
}

int DeviceAudioQueue::GetChannelsCount(){
	return m_Channels;
}

Format DeviceAudioQueue::GetDataFormat(){
	return m_Format;
}

State DeviceAudioQueue::GetState(){
	return m_CurrentState;
}

void DeviceAudioQueue::Create(){
	int block = 0;
	int bits = 0;
	switch(m_Format){
		case FORMAT_S8:
			block = m_Channels;
			bits = 8;
			break;
		case FORMAT_U8:
			block = m_Channels;
			bits = 8;
			break;
		case FORMAT_S16:
			block = 2*m_Channels;
			bits = 16;
			break;
		case FORMAT_U16:
			block = 2*m_Channels;
			bits = 16;
			break;
		case FORMAT_S32:
			block = 4*m_Channels;
			bits = 32;
			break;
		case FORMAT_U32:
			block = 4*m_Channels;
			bits = 32;
			break;
		case FORMAT_FLOAT:
			block = 4*m_Channels;
			bits = 32;
			break;
	};

}

void DeviceAudioQueue::Play(){
	if(IsRunning())
	{
		if(m_CurrentState == STATE_PAUSED){
			// Resume playback.
            
            m_CurrentState = STATE_PLAYING;
		}

		return;
	}



	Start();
	m_CurrentState = STATE_PLAYING;
}

void DeviceAudioQueue::Stop(){
	TearDownAndJoin();


    
	m_CurrentState = STATE_STOPPED;
}

void DeviceAudioQueue::Pause(){
	if(m_CurrentState == STATE_PLAYING){
		m_CurrentState = STATE_PAUSED;
	}
}

void DeviceAudioQueue::Run(){
	while(!IsTearingDown()){
		if(m_CurrentState == STATE_PLAYING){
        
        }
		Core::Thread::Wait(1);
	}
}

Stream * DeviceAudioQueue::LoadStream(const std::string & filename){
	return OggStream::Load(filename);
}

Stream * DeviceAudioQueue::LoadStreamFromReader(Core::DataReader * reader){
	return OggStream::Load(reader);
}

_AUDIO_END

#endif // #if defined(NCK_AUDIOQUEUE)
