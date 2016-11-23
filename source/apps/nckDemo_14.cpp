
/**
* NCtoolKit © 2007-2016 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 14 - Shadow maps
*/

//http://ogldev.atspace.co.uk/www/tutorial49/tutorial49.html

#include "nckDemo_14.h"

Demo14::Demo14(Core::Window * wnd, Graph::Device * dev) {
    this->dev = dev;
    this->wnd = wnd;
    program = NULL;
    scene = NULL;
    time = 0;
    depth = NULL;
    rtManager = NULL;
    rtTexture = NULL;
}

Demo14::~Demo14() {
    SafeDelete(scene);
    SafeDelete(program);
    SafeDelete(depth);
    SafeDelete(rtManager);
    SafeDelete(rtTexture);
}

void Demo14::Load() {
    dev->ClearColor(0, 0, 0);
    dev->Enable(Graph::STATE_BLEND);
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    Core::FileReader * fr = Core::FileReader::Open("model://shadows.bxon");
    BXON::ReaderContext * frCtx = new BXON::ReaderContext(fr);
    BXON::Object * obj = BXON::Object::Parse(dynamic_cast<BXON::Context*>(frCtx));

    BXON::Map * map = dynamic_cast<BXON::Map*>(obj);
    scene = new Scene::Compound_Basic(dev);
    scene->Load(map);

    program = dev->LoadProgram("shader://material.cpp");
    depth = dev->LoadProgram("shader://depth.cpp");

    camera = (Scene::Camera*)scene->GetDatablock(Scene::DATABLOCK_CAMERA, "Camera");
    lamp = (Scene::Lamp*)scene->GetDatablock(Scene::DATABLOCK_LAMP, "Lamp");

    rtManager = dev->CreateRTManager(1024, 1024);
    rtTexture = dynamic_cast<Graph::Texture2D*>(
       dev->CreateTexture(Graph::TEXTURE_2D, 1024, 1024, 0, Graph::FORMAT_R_32F, true)
        );
    rtTexture->SetFilter(Graph::FILTER_MINIFICATION, Graph::FILTER_NEAREST);
    rtTexture->SetFilter(Graph::FILTER_MAGNIFICATION, Graph::FILTER_NEAREST);

    rtTexture->SetAdressMode(Graph::ADRESS_CLAMP);

    rtManager->Attach(0, rtTexture);
}

void Demo14::Render(float dt) {
    CaptureLightBuffer();

    dev->ClearColor(0, 0, 0, 0);
    dev->Clear();
    dev->Viewport(0, 0, wnd->GetWidth(), wnd->GetHeight());


    // Set projection matrix.
    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();

    // Use camera properties as projection matrix (fov & aspect).
    camera->Enable(Graph::MATRIX_PROJECTION);
  
    // Set the view/camera matrix.
    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();

    // Set camera properties as view matrix (rotation & position).
    Math::Mat44 mvMat = camera->GetMatrix();
    dev->LoadMatrix((float*)&mvMat);

    Math::Vec3 lampPos = lamp->GetObject()->GetPosition();//Math::Vec4(lamp->GetObject()->GetPosition(), 1.0) * Math::RotateZ(time);
    Math::Vec4 lamp_mv_pos = Math::Vec4(lamp->GetObject()->GetPosition(), 1.0) * mvMat; //Math::Vec4(lamp->GetObject()->GetPosition(), 1.0) *Math::RotateZ(-time) * mvMat;
    
    // Set the model matrix.
    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();

    dev->Enable(Graph::STATE_DEPTH_TEST);

    program->SetVariable4f("lamp_mv_pos", lamp_mv_pos.GetX(), lamp_mv_pos.GetY(), lamp_mv_pos.GetZ(), 0.0);
  
    program->SetMatrix("matLampProjView",(float*)&lampProjViewMat);
    program->SetVariable1f("texSize", 1024);


    program->Enable();
    rtTexture->Enable();
    scene->Render();
    rtTexture->Disable();
    program->Disable();
    

    dev->Begin(Graph::PRIMITIVE_LINES);
    dev->Color(255, 255, 0);
    dev->Vertex(lampPos.GetX(), lampPos.GetY(), lampPos.GetZ());
    dev->Vertex(lampPos.GetX(), lampPos.GetY(), lampPos.GetZ() + 1);
    dev->End();


    dev->Disable(Graph::STATE_DEPTH_TEST);
    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();
    dev->Ortho2D(wnd->GetWidth(), wnd->GetHeight());

    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();

    //rtTexture->Enable();
    //RenderSquare2D(0, 0, 256, 256, Math::Color4ub());
    //rtTexture->Disable();

    dev->PresentAll();

    time += 0.01;
}

void Demo14::RenderFromLight(float width, float height) {
    float zNear = 0.2;
    float zFar = 150.0;

    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Enable(Graph::STATE_FLIP_CULL_FACE);

    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();
    
    Math::Mat44 matLampProj = Math::Perspective(width / height, 90, zNear, zFar);
    //Math::Mat44 matLampProj = Math::Ortho(-20, -20, 20, 20, zNear, zFar);
    
    dev->MultMatrix((float*)&matLampProj);

    // Set the model matrix.
    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();

    Scene::Camera lampCamp(dev);
    lampCamp.SetObject(lamp->GetObject());
    Math::Mat44 matLampView = lampCamp.GetMatrix();
    //matLampView = Math::RotateZ(time) * matLampView;
    dev->MultMatrix((float*)&matLampView);

    lampProjViewMat = matLampView * matLampProj;

    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();

    depth->Enable();

    scene->Render();
    depth->Disable();

    dev->Disable(Graph::STATE_FLIP_CULL_FACE);
}

void Demo14::CaptureLightBuffer() {
    rtManager->Enable();
    dev->ClearColor(255, 255, 255, 255);
    dev->Clear();
    dev->Viewport(0, 0, rtTexture->GetWidth(), rtTexture->GetHeight());
    RenderFromLight(rtTexture->GetWidth(), rtTexture->GetHeight());
    rtManager->Disable();
}

void Demo14::UpdateWndEvents() {

}

Demo * CreateDemo_14(Core::Window * wnd, Graph::Device * dev) {
    return new Demo14(wnd, dev);
}
