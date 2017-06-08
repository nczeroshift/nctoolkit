
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_AudioOut.h"

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
