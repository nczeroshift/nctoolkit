
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_Curves.h"

Demo_Curves::Demo_Curves(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    curves = NULL;
}

Demo_Curves::~Demo_Curves(){
    SafeDelete(curves);
}

void Demo_Curves::Load(){
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    curves = new Scene::Compound_Base(dev);
    curves->Load("model://curves_scene.bxon");
}

void Demo_Curves::Render(float dt){
    Scene::Camera * cam = curves->GetCamera("Camera");
    cam->SetAspect(wnd->GetWidth()/(float)wnd->GetHeight());

	dev->Clear();
	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();
    cam->Enable(Graph::MATRIX_PROJECTION);
    
    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();
    cam->Enable(Graph::MATRIX_VIEW);

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();
    
    Scene::Curve * cv = curves->GetCurve("Bezier");
    cv->Render();

	dev->PresentAll();
}

void Demo_Curves::UpdateWndEvents(){

}

std::vector<std::string> Demo_Curves::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Basic");
    ret.push_back("Curves");
    return ret;
}

std::string Demo_Curves::GetDescription() {
    return "Bezier Curves";
}

Demo * CreateDemo_Curves(Core::Window * wnd, Graph::Device * dev){
	return new Demo_Curves(wnd,dev);
}
