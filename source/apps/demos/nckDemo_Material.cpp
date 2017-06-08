
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_Material.h"

Demo_Material::Demo_Material(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
    scene = NULL;
    program = NULL;
}

Demo_Material::~Demo_Material(){
    SafeDelete(program);
    SafeDelete(scene);
}

void Demo_Material::Load(){
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    scene = new Scene::Compound_Base(dev);
    scene->Load("model://multi_material_scene.bxon");
    program = dev->LoadProgram("shader://multi_material_mat.cpp");
}

void Demo_Material::Render(float dt){
    Scene::Camera * cam = scene->GetCamera("Camera");
    cam->SetAspect(wnd->GetWidth() / (float)wnd->GetHeight());

    dev->ClearColor(0, 0, 0, 0);
    dev->Clear();
    dev->Viewport(0, 0, wnd->GetWidth(), wnd->GetHeight());

    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();
    cam->Enable(Graph::MATRIX_PROJECTION);

    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();
    cam->Enable(Graph::MATRIX_VIEW);

    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();

    dev->Enable(Graph::STATE_DEPTH_TEST);

    program->Enable();
    scene->Render();
    program->Disable();

    dev->PresentAll();
}

void Demo_Material::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_Material::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Basic");
    ret.push_back("Material");
    return ret;
}

std::string Demo_Material::GetDescription() {
    return "Materials and Multi-Material";
}

Demo * CreateDemo_Material(Core::Window * wnd, Graph::Device * dev){
	return new Demo_Material(wnd,dev);
}
