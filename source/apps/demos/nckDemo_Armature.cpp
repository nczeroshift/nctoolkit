
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_Armature.h"

Demo_Armature::Demo_Armature(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
    scene = NULL;
    program = NULL;
    boneMatrix = NULL;
}

Demo_Armature::~Demo_Armature(){
    SafeArrayDelete(boneMatrix);
    SafeDelete(scene);
    SafeDelete(program);
}

void Demo_Armature::Load(){
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    scene = new Scene::Compound_Base(dev);
    scene->Load("model://armature.bxon");

    program = dev->LoadProgram("shader://armature_mat.cpp");
    scene->GetMaterial("Material")->SetProgram(program);

    Scene::Armature* arm = scene->GetArmature("Armature");
    boneMatrix = new Math::Mat44[arm->GetBones().size()];
}

void Demo_Armature::Render(float dt){

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
    dev->Rotate(time*10.0, 0, 0, 1);

    dev->Enable(Graph::STATE_DEPTH_TEST);

    float bFrameRate = 24.0;
    float duration = 200;

    float keyframe_t = time * bFrameRate - floor((time *  bFrameRate) / duration) * duration;

    Scene::Armature* arm = dynamic_cast<Scene::Armature*>(scene->GetDatablock(Scene::DATABLOCK_ARMATURE, "Armature"));
    arm->Play("", keyframe_t, 0.0, true);

    std::vector<Scene::Bone*> bones = arm->GetBones();
    for (int i = 0; i < bones.size(); i++) {
        boneMatrix[i] = Math::Translate(-bones[i]->m_RestPos) * bones[i]->m_Matrix;
    }

    program->SetMatrixArray("bones_matrix", bones.size(), (float*)boneMatrix);

    scene->Render();
 
    dev->Color(255, 150, 10);
    dev->Disable(Graph::STATE_DEPTH_TEST);
    Scene::Object* obj = dynamic_cast<Scene::Object*>(scene->GetDatablock(Scene::DATABLOCK_OBJECT, "Plane"));

    dev->PushMatrix();
    obj->Bind();
    arm->Update();
    arm->Render();
    dev->PopMatrix();

    time += dt;

    dev->PresentAll();
}

void Demo_Armature::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_Armature::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("TODO");
    //ret.push_back("Advanced");
    //ret.push_back("Animation");
    //ret.push_back("Armature");
    return ret;
}

std::string Demo_Armature::GetDescription() {
    return "Armature and Skinning";
}

Demo * CreateDemo_Armature(Core::Window * wnd, Graph::Device * dev){
	return new Demo_Armature(wnd,dev);
}
