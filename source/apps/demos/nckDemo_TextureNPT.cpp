
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_TextureNPT.h"

Demo_TextureNPT::Demo_TextureNPT(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
    texture = NULL;
}

Demo_TextureNPT::~Demo_TextureNPT(){
    SafeDelete(texture);
}

void Demo_TextureNPT::Load(){
    // Enable z test
    dev->Enable(Graph::STATE_DEPTH_TEST);

    // Set correct states for alpha blending.
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    texture = dev->LoadTexture("texture://tex2d_jardim_monserrate.jpg");
}

void Demo_TextureNPT::Render(float dt){
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
    
    const float size = (wnd->GetWidth() - 4 * 10) / 3.0;
    const float sizeH = size * (texture->GetHeight() / (float)texture->GetWidth());

    texture->SetAdressMode(Graph::ADRESS_CLAMP);

    // Top row, first Image: Trilinear Filtering (mipmap interpolation)
    texture->SetFilter(Graph::FILTER_MAGNIFICATION, Graph::FILTER_LINEAR);
    texture->SetFilter(Graph::FILTER_MINIFICATION, Graph::FILTER_LINEAR);
    texture->SetFilter(Graph::FILTER_MIPMAPPING, Graph::FILTER_LINEAR);
    
    texture->Enable();
    RenderSquare2D(10, 10, size, sizeH);
    texture->Disable();


    // Top row, second Image: Bilinear Filtering, with nearest mipmap.
    /*texture->SetFilter(Graph::FILTER_MAGNIFICATION, Graph::FILTER_LINEAR);
    texture->SetFilter(Graph::FILTER_MINIFICATION, Graph::FILTER_LINEAR);
    texture->SetFilter(Graph::FILTER_MIPMAPPING, Graph::FILTER_NEAREST);

    texture->Enable();
    RenderSquare2D(size + 20, 10, size, sizeH);
    texture->Disable();


    // Top row, second Image: Bilinear Filtering, no mipmap.
    texture->SetFilter(Graph::FILTER_MAGNIFICATION, Graph::FILTER_NEAREST);
    texture->SetFilter(Graph::FILTER_MINIFICATION, Graph::FILTER_NEAREST);
    texture->SetFilter(Graph::FILTER_MIPMAPPING, Graph::FILTER_NONE);

    texture->Enable();
    RenderSquare2D(size * 2 + 30, 10, size, sizeH);
    texture->Disable();*/

    time += dt;

	// Finish rendering and present the graphics.
	dev->PresentAll();
}

void Demo_TextureNPT::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_TextureNPT::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Basic");
    ret.push_back("Textures");
    return ret;
}

std::string Demo_TextureNPT::GetDescription() {
    return "NPT Textures";
}

Demo * CreateDemo_TextureNPT(Core::Window * wnd, Graph::Device * dev){
	return new Demo_TextureNPT(wnd,dev);
}
