
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckDeviceAudioQueue.h"
#include "../nckOggStream.h"

#if defined(NCK_AUDIOQUEUE)

_AUDIO_BEGIN

static void audioQueueCallback(void * userData, AudioQueueRef queue, AudioQueueBufferRef buffer){
    DeviceAudioQueue * daq = (DeviceAudioQueue*)userData;
    buffer->mAudioDataByteSize = 0;
    
    int size = daq->GetCallback()->Buffer(daq,buffer->mPacketDescriptionCapacity,(uint8_t*)buffer->mAudioData+buffer->mAudioDataByteSize);
    
    buffer->mAudioDataByteSize += size;
    
    OSStatus stat = AudioQueueEnqueueBuffer(queue, buffer, 0, NULL);
    if(stat != noErr){
        printf("AudioQueueEnqueueBuffer error\n");
    }
};


DeviceAudioQueue::DeviceAudioQueue(int sampleRate, int channels, Format format,int bufferSize, int bufferCount) {
	m_SampleRate = sampleRate;
	m_Channels = channels;
	m_Format = format;
	m_BufferCount = bufferCount;
    m_BufferSize = bufferSize;
    mBuffers = NULL;
    mPacketDescs = NULL;
    mQueue = NULL;
    m_CurrentState = STATE_STOPPED;

}

DeviceAudioQueue::~DeviceAudioQueue(){
	Stop();
    
    if(mBuffers != NULL)
        for(int i = 0;i<m_BufferCount;i++)
            if(mBuffers[i] != NULL){
                OSStatus err = AudioQueueFreeBuffer(mQueue, mBuffers[i]);
                if(err != noErr)
                    printf("AudioQueueFreeBuffer error\n");
            }
    
    SafeArrayDelete(mBuffers);
    
    if(mQueue != NULL){
        OSStatus err = AudioQueueDispose(mQueue, true);
        if(err != noErr)
            printf("AudioQueueDispose error\n");
    }
    
    mQueue = NULL;
    mBuffers = NULL;
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

int64_t DeviceAudioQueue::GetTime(){
    if(m_CurrentState == STATE_PLAYING || m_CurrentState == STATE_PAUSED){
        AudioQueueTimelineRef timeLine;
        OSStatus status = AudioQueueCreateTimeline(mQueue, &timeLine);
        if(status == noErr) {
            AudioTimeStamp timeStamp;
            AudioQueueGetCurrentTime(mQueue, timeLine, &timeStamp, NULL);
            return 1e6 * timeStamp.mSampleTime / mDataFormat.mSampleRate;
        }
    }
    return 0;
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

    memset(&mDataFormat,0,sizeof(AudioStreamBasicDescription));
    
    mDataFormat.mSampleRate = m_SampleRate;
    mDataFormat.mFormatID = kAudioFormatLinearPCM;
    mDataFormat.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    mDataFormat.mFramesPerPacket = 1;
    mDataFormat.mChannelsPerFrame = m_Channels;
    mDataFormat.mBitsPerChannel = bits;
    
    mDataFormat.mBytesPerFrame = (mDataFormat.mBitsPerChannel / 8) * mDataFormat.mChannelsPerFrame;
    mDataFormat.mBytesPerPacket = mDataFormat.mBytesPerFrame;
    mDataFormat.mReserved = 0;
    
    OSStatus err = AudioQueueNewOutput(&mDataFormat, audioQueueCallback, this, NULL, NULL, 0, &mQueue);
    
    if(err != noErr)
        THROW_EXCEPTION("Unable to create output AudioQueue");
}

void DeviceAudioQueue::Play(){

    if(m_CurrentState == STATE_PAUSED)
    {
        OSStatus err = AudioQueueStart(mQueue, NULL);
        
        if(err != noErr)
            THROW_EXCEPTION("Unable to start audioQueue");

        m_CurrentState = STATE_PLAYING;
    }
    else if(m_CurrentState == STATE_PLAYING){
        return;
    }
    
    const uint32_t one_second = m_SampleRate * mDataFormat.mBytesPerPacket;
    
    if(mBuffers == NULL)
    {
        mBuffers = new AudioQueueBufferRef[m_BufferCount];
        for(int i = 0; i < m_BufferCount; i++)
            mBuffers[i] = NULL;
        
        for(int i = 0; i < m_BufferCount; i++) {
            OSStatus err = AudioQueueAllocateBuffer(mQueue, one_second, &mBuffers[i]);
            if(err != noErr)
                THROW_EXCEPTION("Unable to start audioQueue");
        }
    }
    
    for(int i = 0; i < m_BufferCount; i++) {
        audioQueueCallback(this, mQueue, mBuffers[i]);
    }
    
    OSStatus err = AudioQueueStart(mQueue, NULL);
    
    if(err != noErr)
        THROW_EXCEPTION("Unable to start audioQueue");
    
    m_CurrentState = STATE_PLAYING;
}

void DeviceAudioQueue::Stop(){
    if(m_CurrentState != STATE_STOPPED){
        OSStatus err = AudioQueueStop(mQueue, TRUE);
        if(err != noErr)
            THROW_EXCEPTION("Unable to stop audioQueue");
        
        if(mBuffers != NULL)
            for(int i = 0; i < m_BufferCount; i++) {
                if(mBuffers[i]!=NULL){
                    mBuffers[i]->mAudioDataByteSize = 0;
                    memset(mBuffers[i]->mAudioData, 0, mBuffers[i]->mAudioDataBytesCapacity);
                }
            }
    }
	m_CurrentState = STATE_STOPPED;
}

void DeviceAudioQueue::Pause(){
	if(m_CurrentState == STATE_PLAYING){
		m_CurrentState = STATE_PAUSED;
        OSStatus err = AudioQueuePause(mQueue);
        if(err != noErr)
            THROW_EXCEPTION("Unable to pause audioQueue");
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
