
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckDeviceAlsa.h"

#if defined(NCK_ALSA)

#include "nckException.h"
#include "nckUtils.h"
#include "../nckOggStream.h"

_AUDIO_BEGIN

AlsaBuffer::AlsaBuffer(int capacity){
	this->capacity = capacity;
	data = new uint8_t[capacity];
	size = 0;	
}

AlsaBuffer::~AlsaBuffer(){
	SafeDelete(data);
}

int AlsaGetFrameSize(int channels,Format format)
{
	int32_t frameSize=2*channels;

	if(format==FORMAT_S8 || format==FORMAT_U8)
		frameSize = channels;
	else if(format==FORMAT_S32 || format==FORMAT_U32 || format==FORMAT_FLOAT)
		frameSize = 4 * channels;

	return frameSize;
}

DeviceAlsaOutput::DeviceAlsaOutput() : Threadable() {
	m_BufferCount = 2;
	m_CurrentState = STATE_STOPPED;
	m_MinBufferSize = 0;
	m_AlsaHandle = NULL;
	m_SampleRate = 44100;
	m_Channels = 2;
	m_Format = FORMAT_S16;
	m_DeviceName = "default";
	m_BufferWrite = 0;
	m_BufferRead = 0;
}

DeviceAlsaOutput::DeviceAlsaOutput(const std::string & deviceName,int samplerate, 
				int channels,Format format, int bufferSize, int bufferCount) : Threadable()
{
	m_CurrentState = STATE_STOPPED;
	m_DeviceName = deviceName;
	m_MinBufferSize = bufferSize;
	m_BufferCount = bufferCount;
	m_Format = format;
	m_SampleRate = samplerate;
	m_Channels = channels;
	m_AlsaHandle = NULL;
	m_BufferWrite = 0;
	m_BufferRead = 0;
}

void DeviceAlsaOutput::Create()
{
	snd_pcm_t *alsaHandle;	
	snd_pcm_hw_params_t *hwParams;
	snd_pcm_sw_params_t *swParams;
	int err;	

	std::string device = m_DeviceName;

	if ((err = snd_pcm_open(&m_AlsaHandle, device.c_str() , SND_PCM_STREAM_PLAYBACK, 0)) < 0){
		THROW_EXCEPTION(std::string("Cannot open audio device \"") + device + std::string("\" - ") + std::string(snd_strerror(err)));
	}

	// Hardware parameters
	if ((err = snd_pcm_hw_params_malloc(&hwParams)) < 0){
		snd_pcm_close(m_AlsaHandle);
		m_AlsaHandle = NULL;
		THROW_EXCEPTION(std::string("Cannot allocate hardware parameter structure - ") + std::string(snd_strerror(err)));
	}

	try{
		if ((err = snd_pcm_hw_params_any(m_AlsaHandle, hwParams)) < 0){
			THROW_EXCEPTION(std::string("Cannot initialize hardware parameter structure - ") + std::string(snd_strerror(err)));
		}

		if ((err = snd_pcm_hw_params_set_access(m_AlsaHandle, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0){
			THROW_EXCEPTION(std::string("Cannot set access type - ") + std::string(snd_strerror(err)));
		}

		snd_pcm_format_t targetFormat;
		switch(m_Format){
			case FORMAT_S8:
				targetFormat = SND_PCM_FORMAT_S8;
				break;
			case FORMAT_U8:
				targetFormat = SND_PCM_FORMAT_U8;
				break;
			case FORMAT_S16:
				targetFormat = SND_PCM_FORMAT_S16_LE;
				break;
			case FORMAT_U16:
				targetFormat = SND_PCM_FORMAT_U16_LE;
				break;
			case FORMAT_S32:
				targetFormat = SND_PCM_FORMAT_S32_LE;
				break;
			case FORMAT_U32:
				targetFormat = SND_PCM_FORMAT_U32_LE;
				break;
			case FORMAT_FLOAT:
				targetFormat = SND_PCM_FORMAT_FLOAT_LE;
				break;
			default:
				targetFormat = SND_PCM_FORMAT_S16_LE;
				break;
		};

		if ((err = snd_pcm_hw_params_set_format(m_AlsaHandle, hwParams,targetFormat)) < 0){
			THROW_EXCEPTION(std::string("Cannot set sample format - ") + std::string(snd_strerror(err)));
		}

		uint32_t tempSampleRate = m_SampleRate;
		if ((err = snd_pcm_hw_params_set_rate_near(m_AlsaHandle, hwParams, &(tempSampleRate), 0)) < 0){
			THROW_EXCEPTION(std::string("Cannot set sample rate - ") + std::string(snd_strerror(err)));
		}
		m_SampleRate = tempSampleRate;

		if ((err = snd_pcm_hw_params_set_channels(m_AlsaHandle, hwParams, m_Channels)) < 0){
			THROW_EXCEPTION(std::string("Cannot set channel count - ") + std::string(snd_strerror(err)));
		}

		if ((err = snd_pcm_hw_params(m_AlsaHandle, hwParams)) < 0){
			THROW_EXCEPTION(std::string("Cannot set parameters - ") + std::string(snd_strerror(err)));
		}

		snd_pcm_hw_params_free(hwParams);
	}
	catch(const Core::Exception & ex)
	{
		snd_pcm_hw_params_free(hwParams);
		snd_pcm_close(m_AlsaHandle);
		m_AlsaHandle = NULL;
		THROW_EXCEPTION_STACK("Unable to initialize hardware parameters",ex);
	}



	// Software parameters
	if ((err = snd_pcm_sw_params_malloc(&swParams)) < 0){
		snd_pcm_close(m_AlsaHandle);
		m_AlsaHandle = NULL;
		THROW_EXCEPTION(std::string("Cannot allocate software parameters structure - ") + std::string(snd_strerror(err)));
	}

	try{
		if ((err = snd_pcm_sw_params_current(m_AlsaHandle, swParams)) < 0){
			THROW_EXCEPTION(std::string("Cannot initialize software parameters structure - ") + std::string(snd_strerror(err)));
		}

		uint32_t bufferFrameSize = m_MinBufferSize / AlsaGetFrameSize(m_Channels,m_Format);

		if ((err = snd_pcm_sw_params_set_avail_min (m_AlsaHandle, swParams, bufferFrameSize)) < 0){
			THROW_EXCEPTION(std::string("Cannot set minimum available count - ") + std::string(snd_strerror(err)));
		}

		if ((err = snd_pcm_sw_params_set_start_threshold (m_AlsaHandle, swParams, 0U)) < 0){
			THROW_EXCEPTION(std::string("Cannot set start mode - ") + std::string(snd_strerror(err)));
		}

		if ((err = snd_pcm_sw_params (m_AlsaHandle, swParams)) < 0){
			THROW_EXCEPTION(std::string("Cannot set software parameters - ") + std::string(snd_strerror(err)));
		}

		if ((err = snd_pcm_nonblock(m_AlsaHandle,0)) < 0){
			THROW_EXCEPTION(std::string("Cannot set non blocking audio interface - ") + std::string(snd_strerror(err)));
		}

		snd_pcm_sw_params_free(swParams);
	}
	catch(const Core::Exception & ex)
	{
		snd_pcm_sw_params_free(swParams);
		snd_pcm_close(m_AlsaHandle);
		m_AlsaHandle = NULL;
		THROW_EXCEPTION_STACK("Unable to initialize software parameters",ex);
	}

	int oneSecondDataSize = AlsaGetFrameSize(m_Channels,m_Format) * m_SampleRate;

	// 1.34 Mibps for 44.1kHz S16 Stereo (138kiB per buffer)
	for(int i = 0;i<m_BufferCount;i++){
		m_Buffer.push_back(new AlsaBuffer(oneSecondDataSize));	
	}
}

DeviceAlsaOutput::~DeviceAlsaOutput(){
	Stop();
	if(m_AlsaHandle)
		snd_pcm_close(m_AlsaHandle);
}

int DeviceAlsaOutput::GetSampleRate(){
	return m_SampleRate;
}

int DeviceAlsaOutput::GetChannelsCount(){
	return m_Channels;
}

Format DeviceAlsaOutput::GetDataFormat(){
	return m_Format;
}

void DeviceAlsaOutput::Play(){
	if(IsRunning())
	{
		if(m_CurrentState == STATE_PAUSED)
			snd_pcm_pause(m_AlsaHandle,0);

		return; // Playing or paused
	}

	int err;
	if ((err = snd_pcm_prepare (m_AlsaHandle)) < 0) {
		THROW_EXCEPTION(std::string("Cannot prepare audio interface for playback - ") + std::string(snd_strerror(err)));
	}

	Start();

	m_CurrentState = STATE_PLAYING;
}

void DeviceAlsaOutput::Stop(){
	TearDownAndJoin();
	snd_pcm_drop(m_AlsaHandle);
	m_CurrentState = STATE_STOPPED;
}

void DeviceAlsaOutput::Pause(){
	if(!IsRunning())
		return;//Stopped
	snd_pcm_pause(m_AlsaHandle,1);
	m_CurrentState = STATE_PAUSED;
}

State DeviceAlsaOutput::GetState(){
	return m_CurrentState;
}

Stream * DeviceAlsaOutput::LoadStream(const std::string & filename){
	return OggStream::Load(filename);
}

Stream * DeviceAlsaOutput::LoadStreamFromReader(Core::DataReader * reader){
	return OggStream::Load(reader);
}

void DeviceAlsaOutput::Run()
{
	int err;
	int frames_to_deliver = 0; 

	const int frameSize = AlsaGetFrameSize(m_Channels,m_Format);
	const int totalSamples = m_MinBufferSize/frameSize;

	const int maxAvail = snd_pcm_avail(m_AlsaHandle);

	// No buffering mode
	if(m_BufferCount == 1)
	{
		while(!IsTearingDown())
		{
			int size = m_Callback->Buffer(this,m_Buffer[0]->capacity,m_Buffer[0]->data);
			m_Buffer[0]->size = size;

			if(size<m_MinBufferSize)
				printf("Warning:Alsa buffer hasn't filled the minimum size !\n");

			snd_pcm_state_t state = snd_pcm_state(m_AlsaHandle);

			if(state == SND_PCM_STATE_DISCONNECTED || state == SND_PCM_STATE_DRAINING)
				break;

			if(state==SND_PCM_STATE_PAUSED || state == SND_PCM_STATE_SUSPENDED)
				continue;

			if ((err = snd_pcm_wait (m_AlsaHandle, 1000)) < 0){
				fprintf(stderr, "Poll failed (%s)\n", strerror (errno));
				break;
			}

			nonBuffAvailCheck:if ((frames_to_deliver = snd_pcm_avail_update (m_AlsaHandle)) < 0)
			{
				if (frames_to_deliver == -EPIPE){
					fprintf (stderr, "An xrun occured\n");
					break;
				}
				else{
					fprintf (stderr, "Unknown ALSA avail update return value (%d)\n",frames_to_deliver);
					break;
				}
			}

			if(maxAvail - frames_to_deliver > totalSamples){
				usleep(1000);
				goto nonBuffAvailCheck;
			}

			frames_to_deliver = frames_to_deliver > totalSamples ? totalSamples : frames_to_deliver;

			int consumed_frames = m_Buffer[0]->size/frameSize;

			if((err = snd_pcm_writei(m_AlsaHandle, m_Buffer[0]->data, consumed_frames)) < 0) {
				fprintf (stderr, "Write failed (%s)\n", snd_strerror (err));
			}
		}
		return;
	}

	// Multiple buffers playback mode.
	while(!IsTearingDown())
	{

		while( ((m_BufferWrite+1) % m_Buffer.capacity() != m_BufferRead) )
		{
			if(m_Callback)
			{
				int size = m_Callback->Buffer(this,m_Buffer[m_BufferWrite]->capacity,m_Buffer[m_BufferWrite]->data);
				m_Buffer[m_BufferWrite]->size = size;

				if(size<m_MinBufferSize)
					printf("Warning:Alsa buffer hasn't filled the minimum size !\n");

				m_BufferWrite = (m_BufferWrite+1) % m_Buffer.capacity();
			}
			else
				break;
		}

		snd_pcm_state_t state = snd_pcm_state(m_AlsaHandle);

		if(state == SND_PCM_STATE_DISCONNECTED || state == SND_PCM_STATE_DRAINING)
			break;

		if(state==SND_PCM_STATE_PAUSED || state == SND_PCM_STATE_SUSPENDED)
			continue;

		if ((err = snd_pcm_wait (m_AlsaHandle, 1000)) < 0){
			fprintf(stderr, "Poll failed (%s)\n", strerror (errno));
			break;
		}

		recheck:if ((frames_to_deliver = snd_pcm_avail_update (m_AlsaHandle)) < 0)
		{
			if (frames_to_deliver == -EPIPE){
				fprintf (stderr, "An xrun occured\n");
				break;
			} 
			else{
				fprintf (stderr, "Unknown ALSA avail update return value (%d)\n",frames_to_deliver);
				break;
			}
		}

		// If alsa buffer usage is twice the minimum number of samples, wait to empty a bit
		if(maxAvail - frames_to_deliver > totalSamples * 2){
			usleep(1000);
			goto recheck;
		}

		if(m_BufferWrite != m_BufferRead)
		{
			int consumed_frames = m_Buffer[m_BufferRead]->size/frameSize;

			if((err = snd_pcm_writei(m_AlsaHandle, m_Buffer[m_BufferRead]->data, consumed_frames)) < 0) {
				fprintf (stderr, "Write failed (%s)\n", snd_strerror (err));
			}

			m_BufferRead = (m_BufferRead+1) % m_Buffer.capacity(); 
		}

	}
}

_AUDIO_END

#endif // #if defined(NCK_ALSA)


