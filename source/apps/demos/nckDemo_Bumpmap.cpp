
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_Bumpmap.h"
#include "nckMaterialToProgram.h"

Demo_Bumpmap::Demo_Bumpmap(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
    material = NULL;
    scene = NULL;
}

Demo_Bumpmap::~Demo_Bumpmap(){
    SafeDelete(material);
    SafeDelete(scene);
}

void Demo_Bumpmap::Load(){
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    scene = new Scene::Compound_Base(dev);
    scene->Load("model://tori_scene.bxon");

    std::vector<Scene::Material*> mats;
    scene->GetAllMaterials(&mats);

    Scene::MaterialToProgram mtp(dev);
  
    for (int i = 0; i < mats.size(); i++) {
        Scene::Material * mat = mats[i];
        mat->SetProgram(mtp.Generate(mat));
    }
}

void Demo_Bumpmap::Render(float dt){
    Scene::Camera * cam = scene->GetCamera("Camera");

    dev->Clear();
    dev->Viewport(0, 0, wnd->GetWidth(), wnd->GetHeight());

    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();

    cam->SetAspect(wnd->GetWidth() / (float)wnd->GetHeight());
    cam->Enable(Graph::MATRIX_PROJECTION);

    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();

    cam->Enable(Graph::MATRIX_VIEW);

    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();
    
    dev->Rotate(time * 4.0, 0, 0, 1);

    std::vector<Scene::Object*> lampObjs;
    scene->GetObjectsWithLayer(&lampObjs, Scene::DATABLOCK_LAMP);

    Scene::LampUniforms lUniforms;
    Scene::Lamp::GenerateUniforms(lampObjs, cam->GetMatrix(), &lUniforms);

    std::vector<Scene::Material*> mats;
    scene->GetAllMaterials(&mats);

    for (int i = 0; i < mats.size(); i++) {
        Scene::Material * mat = mats[i];
        lUniforms.Bind(mat->GetProgram());
    }

    scene->Render();
    
    dev->PresentAll();

    time += dt;
}

void Demo_Bumpmap::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_Bumpmap::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Advanced");
    ret.push_back("Rendering");
    ret.push_back("Textures");
    ret.push_back("Bumpmap");
    return ret;
}

std::string Demo_Bumpmap::GetDescription() {
    return "Rendering with Bumpmaps";
}

Demo * CreateDemo_Bumpmap(Core::Window * wnd, Graph::Device * dev){
	return new Demo_Bumpmap(wnd,dev);
}
