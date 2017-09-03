
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_Texture2D.h"

Demo_Texture2D::Demo_Texture2D(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
    m_TextureJpg = NULL;
    m_TexturePng = NULL;
    m_RGBA16Tex  = NULL;
    m_RGBA32Tex  = NULL;
    m_RGBTex    = NULL;
    m_RGBATex   = NULL;
    m_R16Tex    = NULL;
    m_R32Tex    = NULL;
}

Demo_Texture2D::~Demo_Texture2D(){
    SafeDelete(m_TextureJpg);
    SafeDelete(m_TexturePng);
    SafeDelete(m_RGBA16Tex);
    SafeDelete(m_RGBA32Tex);
    SafeDelete(m_R16Tex);
    SafeDelete(m_R32Tex);
}

void Demo_Texture2D::Load(){
    // Enable z test
    dev->Enable(Graph::STATE_DEPTH_TEST);

    // Set correct states for alpha blending.
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    m_TextureJpg = dev->LoadTexture("texture://tex2d_caravela.jpg");
    m_TexturePng = dev->LoadTexture("texture://tex2d_tori.png");
    
    const int width = 256, height = 256;

    // RGB, 8 Bit per channel.
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

    // RGBA, 8 Bit per channel.
    m_RGBATex = dynamic_cast<Graph::Texture2D*>(dev->CreateTexture(Graph::TEXTURE_2D, width, height, 0, Graph::FORMAT_RGBA_8B));
    uint8_t * rgbaData = (uint8_t *)m_RGBATex->Lock();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int off = 4 * (x + y  * width);
            rgbaData[off] = x;
            rgbaData[off + 1] = y;
            rgbaData[off + 2] = height - y;
            rgbaData[off + 3] = 255-sqrt(x*y);
        }
    }
    m_RGBATex->Unlock();

    // Red, 32 bit per pixel, float.
    m_R32Tex = dynamic_cast<Graph::Texture2D*>(dev->CreateTexture(Graph::TEXTURE_2D, width, height, 0, Graph::FORMAT_R_32F));
    float * r32fdata = (float *)m_R32Tex->Lock();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int off = (x + y  * width);
            r32fdata[off] = y/(float)height;
        }
    }
    m_R32Tex->Unlock();

    // Red, 16 bit per pixel, float.
    m_R16Tex = dynamic_cast<Graph::Texture2D*>(dev->CreateTexture(Graph::TEXTURE_2D, width, height, 0, Graph::FORMAT_R_16F));
    // We don't have conversion to half float yet, TODO.
    short * r16fdata = (short *)m_R16Tex->Lock();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int off = (x + y  * width);
            r16fdata[off] = (short)(Math::RandomValue(0,1) * 65536);
        }
    }
    m_R16Tex->Unlock();

    // RGBA, 32 Bit per channel, float.
    m_RGBA32Tex = dynamic_cast<Graph::Texture2D*>(dev->CreateTexture(Graph::TEXTURE_2D, width, height, 0, Graph::FORMAT_RGBA_32F));
    float * rgba32fdata = (float *)m_RGBA32Tex->Lock();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int off = (x + y  * width);
            rgba32fdata[off*4] = x / (float)width;
            rgba32fdata[off*4+1] = y / (float)height;
            rgba32fdata[off*4+2] = 1.0 - y/(float)height;
            rgba32fdata[off*4+3] = 255 - sqrt(x*y);
        }
    }
    m_RGBA32Tex->Unlock();

    // RGBA, 16 Bit per channel, float.
    m_RGBA16Tex = dynamic_cast<Graph::Texture2D*>(dev->CreateTexture(Graph::TEXTURE_2D, width, height, 0, Graph::FORMAT_RGBA_16F));
    short * rgba16fdata = (short *)m_RGBA16Tex->Lock();
    // We don't have conversion to half float yet, TODO.
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int off = (x + y  * width);
            rgba16fdata[off * 4] = Math::RandomValue()*65536;
            rgba16fdata[off * 4 + 1] = Math::RandomValue() * 65536;
            rgba16fdata[off * 4 + 2] = Math::RandomValue() * 65536;
            rgba16fdata[off * 4 + 3] = Math::RandomValue() * 65536;
        }
    }
    m_RGBA16Tex->Unlock();
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
   
    dev->Disable(Graph::STATE_DEPTH_TEST);
    dev->Disable(Graph::STATE_ZBUFFER_WRITE);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);
    dev->Color(255, 255, 255);

    m_TextureJpg->Enable();
    RenderSquare2D(0, 0, 512, 512);
    m_TextureJpg->Disable();

    m_TexturePng->Enable();
    RenderSquare2D(0, 256, 256, 256);
    m_TexturePng->Disable();

    m_RGBTex->Enable();
    RenderSquare2D(512, 0, 256, 256);
    m_RGBTex->Disable();

    m_RGBATex->Enable();
    RenderSquare2D(512, 256, 256, 256);
    m_RGBATex->Disable();

    m_R32Tex->Enable();
    RenderSquare2D(512+256, 0, 256, 256);
    m_R32Tex->Disable();

    m_R16Tex->Enable();
    RenderSquare2D(512+256, 256, 256, 256);
    m_R16Tex->Disable();

    m_RGBA32Tex->Enable();
    RenderSquare2D(512, 512, 256, 256);
    m_RGBA32Tex->Disable();

    m_RGBA16Tex->Enable();
    RenderSquare2D(512+256, 512, 256, 256);
    m_RGBA16Tex->Disable();

    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Enable(Graph::STATE_ZBUFFER_WRITE);


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
