
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
    scene->Load("model://timeline_scene.bxon", this);
}

void Demo_MultiCamera::HandleFinish(BXON::Map * map, Scene::Compound * compound) {
    camerasPerMarker = Scene::Compound_Base::fetchCamerasWithKeyframes(map, dynamic_cast<Scene::Compound_Base*>(compound));
}

Scene::Object * Demo_MultiCamera::findNearestCamera(int keyframe) {
    auto vec = camerasPerMarker;

    if (vec.size() == 0)
        return NULL;

    if (keyframe <= vec[0].first)
        return vec[0].second;
    else if (keyframe >= vec[vec.size() - 1].first)
        return vec[vec.size() - 1].second;

    int left = 0;
    int right = vec.size() - 1;
    int half;

    while (left <= right) {
        half = (left + right) / 2;
        if (vec[half].first == keyframe)
            return vec[half].second;
        else if (keyframe < vec[half].first)
            right = half - 1;
        else
            left = half + 1;
    }

    if (half >= 0 && half <= vec.size() - 1)
        return vec[half].second;

    return NULL;
}

void Demo_MultiCamera::Render(float dt){
	dev->Clear();

    Scene::Object * camObj = findNearestCamera(time * 25.0);
    Scene::Camera * cam = dynamic_cast<Scene::Camera*>(camObj->GetData());
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
