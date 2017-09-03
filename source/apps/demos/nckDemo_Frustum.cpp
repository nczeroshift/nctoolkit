
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_Frustum.h"

Demo_Frustum::Demo_Frustum(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    models = NULL;
    fontTexture = NULL;
    fontMap = NULL;
    time = 0;
}

Demo_Frustum::~Demo_Frustum(){
    SafeDelete(models);
}

void Demo_Frustum::Load(){
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    models = new Scene::Compound_Base(dev);
    models->Load("model://frustum_scene.bxon");

    fontTexture = dev->LoadTexture("texture://tex2d_font_ubuntu.png");

    fontMap = new Gui::FontMap(dev);
    fontMap->Load("script://font_ubuntu.txt");
}

void Demo_Frustum::Render(float dt){
    Scene::Camera * camera = models->GetCamera("Camera");
    camera->SetAspect(wnd->GetWidth() / (float)wnd->GetHeight());

    Math::Mat44 projectionMatrix = camera->GetMatrix(Graph::MATRIX_PROJECTION);
    Math::Mat44 viewMatrix = camera->GetMatrix(Graph::MATRIX_VIEW);
    viewMatrix = Math::RotateZ(time) * viewMatrix;

    dev->ClearColor(0.5, 0.5, 0.5);

    dev->Clear();

    dev->Viewport(0, 0, wnd->GetWidth(), wnd->GetHeight());

    // Render models with frustum culling
    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();
    dev->LoadMatrix((float*)&projectionMatrix);

    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();
    dev->LoadMatrix((float*)&viewMatrix);

    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();

    Math::Frustum frustum;
    frustum.Update(viewMatrix, projectionMatrix);
    dev->FillMode(Graph::FILL_WIREFRAME);

    Scene::RenderStatistics stats = models->Render(&frustum);

    // Draw rendering info.
    dev->Disable(Graph::STATE_DEPTH_TEST);
    dev->FillMode(Graph::FILL_SOLID);

    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();
    dev->Ortho2D(wnd->GetWidth(), wnd->GetHeight());

    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();

    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();

    dev->Color(255, 255, 255);

    fontTexture->Enable();
    fontMap->Draw(10, 20, 16, std::string("Checked: ") + Math::IntToString(stats.GetCheckedObjects()), false);
    fontMap->Draw(10, 44, 16, std::string("Rendered: ") + Math::IntToString(stats.GetRenderedObjects()), false);

    fontTexture->Disable();

    time += dt;

    // Finish rendering and present the graphics.
    dev->PresentAll();
}

void Demo_Frustum::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_Frustum::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Basic");
    ret.push_back("Rendering");
    ret.push_back("Compound");
    ret.push_back("Frustum");
    return ret;
}

std::string Demo_Frustum::GetDescription() {
    return "Rendering with Frustum";
}

Demo * CreateDemo_Frustum(Core::Window * wnd, Graph::Device * dev){
	return new Demo_Frustum(wnd,dev);
}
