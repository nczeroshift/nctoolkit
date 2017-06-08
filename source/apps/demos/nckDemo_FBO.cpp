
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_FBO.h"

Demo_FBO::Demo_FBO(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
}

Demo_FBO::~Demo_FBO(){

}

void Demo_FBO::Load(){
    // Enable z test
    dev->Enable(Graph::STATE_DEPTH_TEST);

    // Set correct states for alpha blending.
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

}

void Demo_FBO::Render(float dt){
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

void Demo_FBO::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_FBO::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Basic");
    ret.push_back("Rendering");
    ret.push_back("FBO");
    return ret;
}

std::string Demo_FBO::GetDescription() {
    return "Frame Buffer Object";
}

Demo * CreateDemo_FBO(Core::Window * wnd, Graph::Device * dev){
	return new Demo_FBO(wnd,dev);
}
