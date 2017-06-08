
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_Texture2D.h"

Demo_Texture2D::Demo_Texture2D(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
    m_Texture = NULL;
    m_RGBTex = NULL;
    m_R32Tex = NULL;
}

Demo_Texture2D::~Demo_Texture2D(){
    SafeDelete(m_Texture);
    SafeDelete(m_RGBTex);
    SafeDelete(m_R32Tex);
}

void Demo_Texture2D::Load(){
    // Enable z test
    dev->Enable(Graph::STATE_DEPTH_TEST);

    // Set correct states for alpha blending.
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    // TODO
    // Load other formats (PNG/RGB/RGBA)
    m_Texture = dev->LoadTexture("texture://tex2d_caravela.jpg");
    

    // TODO
    // The rest of texture formats.
    const int width = 256, height = 256;

    m_RGBTex = dynamic_cast<Graph::Texture2D*>(dev->CreateTexture(Graph::TEXTURE_2D, width, height, 0, Graph::FORMAT_RGB_8B));
    uint8_t * rgbData = (uint8_t *)m_RGBTex->Lock();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int off = 3 * (x + y  * width);
            rgbData[off] = x;
            rgbData[off+1] = y;
            rgbData[off+2] = height-y;
        }
    }
    m_RGBTex->Unlock();

    m_R32Tex = dynamic_cast<Graph::Texture2D*>(dev->CreateTexture(Graph::TEXTURE_2D, width, height, 0, Graph::FORMAT_R_32F));
    float * r32data = (float *)m_R32Tex->Lock();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int off = (x + y  * width);
            r32data[off] = y/(float)height;
        }
    }
    m_R32Tex->Unlock();
}

void Demo_Texture2D::Render(float dt){
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
   
    m_Texture->Enable();
    RenderSquare2D(0, 0, m_Texture->GetWidth(), m_Texture->GetHeight());
    m_Texture->Disable();

    m_RGBTex->Enable();
    RenderSquare2D(512, 0, m_RGBTex->GetWidth(), m_RGBTex->GetHeight());
    m_RGBTex->Disable();

    m_R32Tex->Enable();
    RenderSquare2D(512, 256, m_RGBTex->GetWidth(), m_RGBTex->GetHeight());
    m_R32Tex->Disable();

	// Finish rendering and present the graphics.
	dev->PresentAll();
}

void Demo_Texture2D::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_Texture2D::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Basic");
    ret.push_back("Rendering");
    ret.push_back("Textures");
    return ret;
}

std::string Demo_Texture2D::GetDescription() {
    return "2D Textures";
}

Demo * CreateDemo_Texture2D(Core::Window * wnd, Graph::Device * dev){
	return new Demo_Texture2D(wnd,dev);
}
