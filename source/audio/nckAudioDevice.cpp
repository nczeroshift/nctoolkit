
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckAudioDevice.h"
#include "nckException.h"
#include "nckUtils.h"

#if defined (NCK_WAVEOUT)
    #include "windows/nckDeviceWaveOut.h"
#elif defined(NCK_ALSA)
    #include "alsa/nckDeviceAlsa.h"
#endif

_AUDIO_BEGIN

void Device::SetCallback(DeviceCallback * callback){
	m_Callback = callback;
}

DeviceCallback * Device::GetCallback(){
	return m_Callback;
}

Device * CreateDevice(Type type, int samplerate, int channels, Format format, int bufferSize, int bufferCount)
{
	 Device * dev = NULL;
	
	 try{
#if defined (NCK_WAVEOUT)
         DeviceWaveOut * tmp = new DeviceWaveOut(samplerate,channels,format,bufferSize,bufferCount);
         tmp->Create();
         dev = tmp;
#elif defined (NCK_ALSA)
         DeviceAlsaOutput * tmp = new DeviceAlsaOutput("default",samplerate,channels,format,bufferSize,bufferCount);
         tmp->Create();
         dev = tmp;
#endif	
	}
	catch(const Core::Exception & ex)
	{
		SafeDelete(dev);
		THROW_EXCEPTION_STACK("Unable to create audio device",ex);			
	}

	return dev;
}

void Device::GetBlockAndBitsPerSample(Format format, int channels, int * block, int * bits){
	switch(format){
		case FORMAT_S8:
			*block = channels;
			*bits = 8;
			break;
		case FORMAT_U8:
			*block = channels;
			*bits = 8;
			break;
		case FORMAT_S16:
			*block = 2*channels;
			*bits = 16;
			break;
		case FORMAT_U16:
			*block = 2*channels;
			*bits = 16;
			break;
		case FORMAT_S32:
			*block = 4*channels;
			*bits = 32;
			break;
		case FORMAT_U32:
			*block = 4*channels;
			*bits = 32;
			break;
		case FORMAT_FLOAT:
			*block = 4*channels;
			*bits = 32;
			break;
	};
}
 
_AUDIO_END


