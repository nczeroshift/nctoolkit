
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_Compound.h"

Demo_Compound::Demo_Compound(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    shader = NULL;
    model = NULL;
    time = 0;
}

Demo_Compound::~Demo_Compound(){
    SafeDelete(shader);
    SafeDelete(model);
}

void Demo_Compound::Load(){
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    // Load the shader program to render the caravela.
    shader = dev->LoadProgram("shader://caravela_mat.cpp");

    // Create the compound where the caravela model will be loaded.
    model = new Scene::Compound_Base(dev);

    // Load the caravela compound model from the data/models folder.
    model->Load("model://caravela_scene.bxon");
}

void Demo_Compound::Render(float dt){
    // Get camera included in compound.
    Scene::Camera * camera = model->GetCamera("Camera");
    camera->SetAspect(wnd->GetWidth() / (float)wnd->GetHeight());

    dev->ClearColor(0.5, 0.5, 0.5);

    // Clear graphics rendering buffer.
    dev->Clear();

    // Define the rendering area in window space.
    dev->Viewport(0, 0, wnd->GetWidth(), wnd->GetHeight());

    // Set projection matrix.
    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();

    // Use camera properties as projection matrix (fov & aspect).
    camera->Enable(Graph::MATRIX_PROJECTION);


    // Set the view (camera) matrix.
    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();

    // Set camera properties as view matrix (rotation & position).
    camera->Enable(Graph::MATRIX_VIEW);


    // Set the model matrix.
    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();

    // Rotate the caravela in the Z axis.
    dev->Rotate(time, 0, 0, 1);

    // Set the shader tex1 as the first enabled texture(use in the model material).
    shader->SetVariable1i("tex1", 0);

    // Enable the shader.
    shader->Enable();

    // Render the caravela.
    model->Render();

    // Disable the shader.
    shader->Disable();

    time += dt;

    // Finish rendering and present the graphics.
    dev->PresentAll();
}

void Demo_Compound::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_Compound::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Basic");
    ret.push_back("Rendering");
    ret.push_back("Compound");
    ret.push_back("BXON");
    return ret;
}

std::string Demo_Compound::GetDescription() {
    return "Loading and Rendering Compound";
}

Demo * CreateDemo_Compound(Core::Window * wnd, Graph::Device * dev){
	return new Demo_Compound(wnd,dev);
}
