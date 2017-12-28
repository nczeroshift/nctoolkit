
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_AudioOut.h"
#include "nckWavStream.h"

Demo_AudioOut::Demo_AudioOut(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
}

Demo_AudioOut::~Demo_AudioOut(){

}

void Demo_AudioOut::Load(){
    // Enable z test
    dev->Enable(Graph::STATE_DEPTH_TEST);

    // Set correct states for alpha blending.
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    Audio::OutputDevice * dev = dynamic_cast<Audio::OutputDevice*>(Audio::CreateDevice(Audio::DEVICE_OUTPUT, 44100, 2, Audio::FORMAT_S16, 8192, 4));
    Audio::Stream * stream = dev->LoadStream("audio://visinin-fourfold.ogg");
    
    Core::FileWriter * writer = Core::FileWriter::Open("audio://out.wav");

    Audio::WavWriter * output = new Audio::WavWriter(writer);

    uint8_t * buffer = new uint8_t[stream->GetSampleRate()*stream->GetChannelsCount()*sizeof(float)];
    int amountToRead = 1024;
    int decodedSize = 0;
    
    output->SetChannelsCount(stream->GetChannelsCount());
    output->SetSampleRate(stream->GetSampleRate());
    output->SetFormat(Audio::FORMAT_S16);

    while ((decodedSize = stream->Read(amountToRead, buffer)) > 0) {
        output->Write(buffer, decodedSize);
    }

    output->Finish();
    SafeDelete(output);
    SafeDelete(writer);
}

void Demo_AudioOut::Render(float dt){
	// Clear graphics rendering buffer.
	dev->Clear();

	// Define the rendering area in window space.
	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

	// Set projection matrix.
	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();

    dev->Ortho2D(wnd->GetWidth(),wnd->GetHeight());
    
    // Set the view (camera) matrix.
    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();
   

	// Set the model matrix.
	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();
    

    time += dt;

	// Finish rendering and present the graphics.
	dev->PresentAll();
}

void Demo_AudioOut::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_AudioOut::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("TODO");
    //ret.push_back("Audio");
    return ret;
}

std::string Demo_AudioOut::GetDescription() {
    return "Audio Output Device";
}

Demo * CreateDemo_AudioOut(Core::Window * wnd, Graph::Device * dev){
	return new Demo_AudioOut(wnd,dev);
}
