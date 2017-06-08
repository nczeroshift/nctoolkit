
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_AudioFFT.h"

Demo_AudioFFT::Demo_AudioFFT(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
    texture = NULL;
}

Demo_AudioFFT::~Demo_AudioFFT(){
    SafeDelete(texture);
}

void Demo_AudioFFT::Load(){
    // Enable z test
    dev->Enable(Graph::STATE_DEPTH_TEST);

    // Set correct states for alpha blending.
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    texture = dev->LoadTexture("texture://tex2d_jardim_monserrate.jpg");
}

void Demo_AudioFFT::Render(float dt){
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

void Demo_AudioFFT::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_AudioFFT::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("TODO");
    //ret.push_back("Advanced");
    //ret.push_back("Audio");
    //ret.push_back("FFT");
    return ret;
}

std::string Demo_AudioFFT::GetDescription() {
    return "Audio Playback with FFT Analysis";
}

Demo * CreateDemo_AudioFFT(Core::Window * wnd, Graph::Device * dev){
	return new Demo_AudioFFT(wnd,dev);
}
