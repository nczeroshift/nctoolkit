
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_Texture3D.h"

Demo_Texture3D::Demo_Texture3D(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
    m_Texture = NULL;
}

Demo_Texture3D::~Demo_Texture3D(){
    SafeDelete(m_Texture);
}

void Demo_Texture3D::Load(){
    // Enable z test
    dev->Enable(Graph::STATE_DEPTH_TEST);

    // Set correct states for alpha blending.
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    m_Texture = dynamic_cast<Graph::Texture3D*>(dev->LoadTexture("texture://tex3d_scan.tar"));
    m_Texture3DProg = dev->LoadProgram("shader://texture3d.cpp");
}

void Demo_Texture3D::Render(float dt){
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
    
    float z = (time - floor(time / m_Texture->GetDepth()) *  m_Texture->GetDepth())/(m_Texture->GetDepth());

    m_Texture3DProg->Enable();
    m_Texture3DProg->SetVariable1f("depth", z);

    float scale = 2.0;

    dev->PushMatrix();
    dev->Translate(wnd->GetWidth() * 0.5 - scale * m_Texture->GetWidth()*0.5
        , wnd->GetHeight() * 0.5 - scale* m_Texture->GetWidth()*0.5, 0.0);
    dev->Scale(scale, scale, 1);

    m_Texture->Enable();
    dev->Color(255, 255, 255, 255);
    dev->Begin(Graph::PRIMITIVE_QUADS);
    dev->TexCoord(0, 0);
    dev->Vertex(0, 0);
    dev->TexCoord(0, 1);
    dev->Vertex(0, m_Texture->GetHeight());
    dev->TexCoord(1, 1);
    dev->Vertex(m_Texture->GetWidth(), m_Texture->GetHeight());
    dev->TexCoord(1, 0);
    dev->Vertex(m_Texture->GetWidth(), 0);
    dev->End();
    m_Texture->Disable();

    dev->PopMatrix();

    m_Texture3DProg->Disable();

    time += dt;

	// Finish rendering and present the graphics.
	dev->PresentAll();
}

void Demo_Texture3D::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_Texture3D::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Basic");
    ret.push_back("Rendering");
    ret.push_back("Textures 3D");
    return ret;
}

std::string Demo_Texture3D::GetDescription() {
    return "3D Textures";
}

Demo * CreateDemo_Texture3D(Core::Window * wnd, Graph::Device * dev){
	return new Demo_Texture3D(wnd,dev);
}
