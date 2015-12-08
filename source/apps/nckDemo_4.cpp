
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 4 - Audio Playback
*/

#include "nckDemo_4.h"
#include "nckQueueBuffer.h"

class AudioEventCallback : public virtual Audio::DeviceCallback{
public:
	AudioEventCallback(Audio::OutputDevice * adev) : stream(NULL){
		stream = adev->LoadStream("audio://visinin-fourfold.ogg");
        
		int64_t duration = dynamic_cast<Audio::SeekStream*>(stream)->GetDuration();
		int total_seconds = duration / 1e6;
		int minutes = total_seconds / 60.0;
		int secs = total_seconds - minutes * 60;

        if(!stream)
            THROW_EXCEPTION("Error loading audio stream");
        
		reachedEOF = false;
		buffer = new Core::QueueBuffer();
		tmpBuffer = new uint8_t[1024*1024];
	}

	virtual ~AudioEventCallback(){
		SafeDelete(stream);
		SafeDelete(buffer);
		SafeDelete(tmpBuffer);
	}

	int Buffer(Audio::Device * dev,int bufferSize,uint8_t * bufferData){
		int dataToRead = dev->GetChannelsCount() * 2 * 4000;
		if(buffer->Size()>=dataToRead)
		{
			return (int)buffer->Pop(bufferData,dataToRead);
		}

		int readSize = stream->Read(dataToRead,tmpBuffer);
		
		if(!reachedEOF && !readSize){
            Core::DebugLog("End of audio file!\n");
			reachedEOF = true;
		}
		else{
			buffer->Push(tmpBuffer,readSize);
			readSize = (int)buffer->Pop(bufferData,dataToRead);
		}

		return readSize;
        
		//return stream->Read(8192,bufferData);
	}

	bool HasReachedEOF(){
		return reachedEOF;
	}
private:
	uint8_t * tmpBuffer;
	Core::QueueBuffer * buffer;
	Audio::Stream * stream;
	bool reachedEOF;
};


Demo4::Demo4(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
	callback = NULL;
	audio = NULL;
}

Demo4::~Demo4(){
	if(audio)audio->Stop();
	SafeDelete(audio);
	SafeDelete(callback);
}

void Demo4::Load(){
	dev->Enable(Graph::STATE_BLEND);
	dev->Disable(Graph::STATE_DEPTH_TEST);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA,Graph::BLEND_INV_SRC_ALPHA);

	audio = dynamic_cast<Audio::OutputDevice*>(Audio::CreateDevice(Audio::DEVICE_OUTPUT, 44100,2,Audio::FORMAT_S16,8192,4));
    
    if(!audio)
        THROW_EXCEPTION("Error creating audio device");
    
	callback = new AudioEventCallback(audio);

	audio->SetCallback(callback);
	audio->Play();
}

void Demo4::Render(float dt){
	dev->Clear();
	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();

	dev->Ortho2D(wnd->GetWidth(),wnd->GetHeight());

	dev->MatrixMode(Graph::MATRIX_VIEW);
	dev->Identity();

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();

	dev->PresentAll();

	if(audio && callback && audio->GetState() == Audio::STATE_PLAYING && callback->HasReachedEOF())
		audio->Stop();
}

void Demo4::UpdateWndEvents(){
	static bool wasDown = false;
	bool last = wnd->GetKeyStatus(Core::BUTTON_SPACE) == Core::BUTTON_STATUS_DOWN;

	if(!wasDown && last){
		if(audio->GetState() == Audio::STATE_PLAYING)
			audio->Pause();
		else
			audio->Play();
	}

	wasDown = last;
}


Demo * CreateDemo_4(Core::Window * wnd, Graph::Device * dev){
	return new Demo4(wnd,dev);
}
