
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_MultiCamera.h"

Demo_MultiCamera::Demo_MultiCamera(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
    scene = NULL;
}

Demo_MultiCamera::~Demo_MultiCamera(){
    SafeDelete(scene);
}

void Demo_MultiCamera::Load(){
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    scene = new Scene::Compound_Base(dev);
    scene->Load("model://timeline_scene.bxon");
}

void Demo_MultiCamera::Render(float dt){
    Scene::Camera * cam = scene->GetCameraForKeyframe(time * 25.0);
    if (!cam)
        return;

    dev->Clear();

    cam->SetAspect(wnd->GetWidth() / (float)wnd->GetHeight());

	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();
    cam->Enable(Graph::MATRIX_PROJECTION);
    
    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();
    cam->Enable(Graph::MATRIX_VIEW);

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();
    
    time += dt;

	dev->PresentAll();
}

void Demo_MultiCamera::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_MultiCamera::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("TODO");
    //ret.push_back("Basic");
    //ret.push_back("Compound");
    //ret.push_back("Camera");
    //ret.push_back("Keyframes");
    return ret;
}

std::string Demo_MultiCamera::GetDescription() {
    return "Camera Switching with Markers";
}

Demo * CreateDemo_MultiCamera(Core::Window * wnd, Graph::Device * dev){
	return new Demo_MultiCamera(wnd,dev);
}
