
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 13 - Normal Map
*/

#include "nckDemo_13.h"

Demo13::Demo13(Core::Window * wnd, Graph::Device * dev) {
    this->dev = dev;
    this->wnd = wnd;
    basic = NULL;
    scene = NULL;
    time = 0;
}

Demo13::~Demo13() {
    SafeDelete(scene);
    SafeDelete(basic);
}

void Demo13::Load() {
    dev->ClearColor(0, 0, 0);
    dev->Enable(Graph::STATE_BLEND);
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    Core::FileReader * fr = Core::FileReader::Open("model://tori.bxon");
    BXON::ReaderContext * frCtx = new BXON::ReaderContext(fr);
    BXON::Object * obj = BXON::Object::Parse(dynamic_cast<BXON::Context*>(frCtx));

    BXON::Map * map = dynamic_cast<BXON::Map*>(obj);
    scene = new Scene::Compound_Basic(dev);
    scene->Load(map);

    basic = dev->LoadProgram("shader://bumpmap.cpp");

    SafeDelete(map);

    camera = (Scene::Camera*)scene->GetDatablock(Scene::DATABLOCK_CAMERA, "Camera");
}

void Demo13::Render(float dt) {
    dev->Clear();

    dev->Viewport(0, 0, wnd->GetWidth(), wnd->GetHeight());

    // Set projection matrix.
    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();

    // Use camera properties as projection matrix (fov & aspect).
    camera->Enable(Graph::MATRIX_PROJECTION);

    // Set the model matrix.
    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();



    // Set camera properties as view matrix (rotation & position).
    camera->Enable(Graph::MATRIX_VIEW);
    dev->Rotate(time, 0, 0, 1);

    basic->Enable();
    scene->Render();
    basic->Disable();

    //camera->GetObject()->Play(time);

    dev->PresentAll();

    time += 0.4;

    //if (time > 100)
    //    time = 0.0;
}

void Demo13::UpdateWndEvents() {

}

Demo * CreateDemo_13(Core::Window * wnd, Graph::Device * dev) {
    return new Demo13(wnd, dev);
}
