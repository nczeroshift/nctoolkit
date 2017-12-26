
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_FBO.h"

Demo_FBO::Demo_FBO(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
    model = NULL;
    tex1 = NULL;
    tex2 = NULL;
    mng = NULL;
}

Demo_FBO::~Demo_FBO(){
    SafeDelete(tex1);
    SafeDelete(tex2);
    SafeDelete(mng);
}

void Demo_FBO::Load(){
    // Enable z test
    dev->Enable(Graph::STATE_DEPTH_TEST);

    // Set correct states for alpha blending.
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    mng = dev->CreateRTManager(512, 512);
    tex1 = dynamic_cast<Graph::Texture2D*>(dev->CreateTexture(Graph::TEXTURE_2D, 512, 512, 0, Graph::FORMAT_RGBA_8B, true));
    tex2 = dynamic_cast<Graph::Texture2D*>(dev->CreateTexture(Graph::TEXTURE_2D, 512, 512, 0, Graph::FORMAT_RGBA_8B, true));

    mng->Attach(0, tex1);
    mng->Attach(1, tex2);

    model = new Scene::Compound_Base(dev);
    model->Load("model://fbo_scene.bxon");

    prog = dev->LoadProgram("shader://fbo.cpp");
}

void Demo_FBO::Render(float dt){
    dev->ClearColor(0, 0, 0);

    mng->Enable();
    {
        dev->Viewport(0, 0, tex1->GetWidth(), tex1->GetHeight());
        dev->Clear();

        dev->MatrixMode(Graph::MATRIX_PROJECTION);
        dev->Identity();

        Scene::Camera * cam = model->GetCamera("Camera");
        cam->SetAspect(1.0);
        cam->Enable(Graph::MATRIX_PROJECTION);


        dev->MatrixMode(Graph::MATRIX_VIEW);
        dev->Identity();
        cam->Enable(Graph::MATRIX_VIEW);


        dev->MatrixMode(Graph::MATRIX_MODEL);
        dev->Identity();

        prog->Enable();
        model->Render();
        prog->Disable();
    }
    mng->Disable();
  

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
   
    tex1->Enable();
    RenderSquare2D(0, 0, 256, 256);
    tex1->Disable();

    tex2->Enable();
    RenderSquare2D(256, 0, 256, 256);
    tex2->Disable();
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
