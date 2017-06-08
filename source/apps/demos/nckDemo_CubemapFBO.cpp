
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_CubemapFBO.h"

Demo_CubemapFBO::Demo_CubemapFBO(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;

    material = NULL;
    material_cb = NULL;
    box = NULL;
    time = 0;
    cbRT = NULL;
    cbTex = NULL;

    faceSize = 256;
}

Demo_CubemapFBO::~Demo_CubemapFBO(){
    SafeDelete(cbRT);
    SafeDelete(cbTex);
    SafeDelete(box);
    SafeDelete(material);
    SafeDelete(material_cb);
}

void Demo_CubemapFBO::Load(){
    box = new Scene::Compound_Base(dev);
    box->Load("model://cubemap_fbo_scene.bxon");
    material = dev->LoadProgram("shader://cubemap_fbo_tex.cpp");
    material_cb = dev->LoadProgram("shader://cubemap_fbo_cb.cpp");

    dynamic_cast<Scene::Material*>(box->GetDatablock(Scene::DATABLOCK_MATERIAL, "Walls"))->SetProgram(material);
    dynamic_cast<Scene::Material*>(box->GetDatablock(Scene::DATABLOCK_MATERIAL, "Red"))->SetProgram(material);
    dynamic_cast<Scene::Material*>(box->GetDatablock(Scene::DATABLOCK_MATERIAL, "Green"))->SetProgram(material);
    dynamic_cast<Scene::Material*>(box->GetDatablock(Scene::DATABLOCK_MATERIAL, "Blue"))->SetProgram(material);
    dynamic_cast<Scene::Material*>(box->GetDatablock(Scene::DATABLOCK_MATERIAL, "Light"))->SetProgram(material);


    cbRT = dev->CreateRTManager(faceSize, faceSize);

    cbTex = dynamic_cast<Graph::TextureCubeMap*>(dev->CreateTexture(Graph::TEXTURE_CUBEMAP, faceSize, faceSize, 0, Graph::FORMAT_RGBA_8B, true));
    cbTex->SetAdressMode(Graph::ADRESS_WRAP);
    cbTex->SetFilter(Graph::FILTER_MAGNIFICATION, Graph::FILTER_LINEAR);
    cbTex->SetFilter(Graph::FILTER_MINIFICATION, Graph::FILTER_LINEAR);

    cbRT->Attach(0, cbTex);

    Scene::Material * mat = dynamic_cast<Scene::Material*>(box->GetDatablock(Scene::DATABLOCK_MATERIAL, "Sphere"));
    mat->SetProgram(material_cb);

    Scene::Texture * tex = new Scene::Texture(dev);
    tex->SetTexture(cbTex);

    mat->SetTextureLayer(0, new Scene::TextureLayer(tex, NULL, dev));
}

void Demo_CubemapFBO::Render(float dt){
    const float aspect = wnd->GetWidth() / (float)wnd->GetHeight();
    Scene::Camera * cam = dynamic_cast<Scene::Camera*>(box->GetDatablock(Scene::DATABLOCK_CAMERA, "Camera"));
    cam->SetAspect(aspect);

    Scene::Object * sphere = dynamic_cast<Scene::Object*>(box->GetDatablock(Scene::DATABLOCK_OBJECT, "Sphere"));
    for (int i = 0; i < 6; i++)
        RenderCubeMap(sphere->GetPosition(), i);

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
    dev->Rotate(sin(time), 0, 0, 1);

    dev->Enable(Graph::STATE_DEPTH_TEST);

    Scene::Lamp * lampData = dynamic_cast<Scene::Lamp*>(box->GetDatablock(Scene::DATABLOCK_LAMP, "Lamp"));
    Math::Vec3 lampPos = lampData->GetObject()->GetPosition() + Math::Vec3(sin(time), cos(time), 0) * 2;
    lamp_pos_wm = lampPos;
    Math::Vec3 lampPosMV = Math::Vec4(lampPos, 1.0) * cam->GetMatrix();

    //material_cb->SetVariable4f("lamp_mv_pos", lampPosMV.GetX(), lampPosMV.GetY(), lampPosMV.GetZ());
    material->SetVariable4f("lamp_mv_pos", lampPosMV.GetX(), lampPosMV.GetY(), lampPosMV.GetZ());

    Math::Vec3 camPos = cam->GetObject()->GetPosition();
    material_cb->SetVariable4f("gphCameraPos", camPos.GetX(), camPos.GetY(), camPos.GetZ());

    box->Render(NULL, NULL, Scene::LAYER_ALL);

    dev->PresentAll();

    time += dt;
}


void Demo_CubemapFBO::RenderCubeMap(const Math::Vec3 & position, int face)
{
    if (cbRT->Enable(face))
    {
        dev->ClearColor(0, 0, 0, 1);
        dev->Clear();
        dev->Viewport(0, 0, cbTex->GetWidth(), cbTex->GetHeight());

        dev->MatrixMode(Graph::MATRIX_PROJECTION);
        dev->Identity();
        dev->Perspective(1.0, 90, 0.01, 100);

        dev->MatrixMode(Graph::MATRIX_VIEW);
        dev->Identity();

        Math::Mat44 mv = Math::CubemapTransform(position, face);

        dev->LoadMatrix((float*)&mv);

        dev->MatrixMode(Graph::MATRIX_MODEL);
        dev->Identity();

        dev->Enable(Graph::STATE_ZBUFFER_WRITE);
        dev->Enable(Graph::STATE_DEPTH_TEST);

        Scene::Lamp * lampData = dynamic_cast<Scene::Lamp*>(box->GetDatablock(Scene::DATABLOCK_LAMP, "Lamp"));
        Scene::Camera * cam = dynamic_cast<Scene::Camera*>(box->GetDatablock(Scene::DATABLOCK_CAMERA, "Camera"));

        Math::Vec3 lampPosMV = Math::Vec4(lamp_pos_wm, 1.0) * mv;

        material->SetVariable4f("lamp_mv_pos", lampPosMV.GetX(), lampPosMV.GetY(), lampPosMV.GetZ());

        material->Enable();
        box->Render(NULL, NULL, Scene::LAYER_1);
        material->Disable();

        cbRT->Disable();
    }
}

void Demo_CubemapFBO::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_CubemapFBO::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Advanced");
    ret.push_back("Rendering");
    ret.push_back("Cubemap");
    ret.push_back("FBO");
    return ret;
}

std::string Demo_CubemapFBO::GetDescription() {
    return "Rendering to Cubemap";
}

Demo * CreateDemo_CubemapFBO(Core::Window * wnd, Graph::Device * dev){
	return new Demo_CubemapFBO(wnd,dev);
}
