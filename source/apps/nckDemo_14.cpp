
/**
* NCtoolKit © 2007-2016 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 14 - Normal Map
*/

//http://ogldev.atspace.co.uk/www/tutorial49/tutorial49.html

#include "nckDemo_14.h"

Demo14::Demo14(Core::Window * wnd, Graph::Device * dev) {
    this->dev = dev;
    this->wnd = wnd;
    basic = NULL;
    scene = NULL;
    time = 0;
}

Demo14::~Demo14() {
    SafeDelete(scene);
    SafeDelete(basic);
}

void Demo14::Load() {
    dev->ClearColor(0, 0, 0);
    dev->Enable(Graph::STATE_BLEND);
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

   /* Core::FileReader * fr2 = Core::FileReader::Open("model://tori.bxon");
    BXON::ReaderContext * frCtx2 = new BXON::ReaderContext(fr2);
    BXON::Object * obj2 = BXON::Object::Parse(dynamic_cast<BXON::Context*>(frCtx2));
    BXON::Map * map2 = dynamic_cast<BXON::Map*>(obj2);
    scene2 = new Scene::Compound_Basic(dev);
    scene2->Load(map2);



    SafeDelete(frCtx2);
    SafeDelete(fr2);
    SafeDelete(map2);

    */
    Core::FileReader * fr = Core::FileReader::Open("model://shadows.bxon");
    BXON::ReaderContext * frCtx = new BXON::ReaderContext(fr);
    BXON::Object * obj = BXON::Object::Parse(dynamic_cast<BXON::Context*>(frCtx));

    BXON::Map * map = dynamic_cast<BXON::Map*>(obj);
    scene = new Scene::Compound_Basic(dev);
    scene->Load(map);

    //basic = dev->LoadProgram("shader://bumpmap.cpp");

    //SafeDelete(map);
    //SafeDelete(frCtx);
    //SafeDelete(fr);


    camera = (Scene::Camera*)scene->GetDatablock(Scene::DATABLOCK_CAMERA, "Camera");
}

void Demo14::Render(float dt) {
    dev->Clear();


    
    /*Core::FileReader * fr2 = Core::FileReader::Open("model://tori.bxon");
    BXON::ReaderContext * frCtx2 = new BXON::ReaderContext(fr2);
    BXON::Object * obj2 = BXON::Object::Parse(dynamic_cast<BXON::Context*>(frCtx2));
    BXON::Map * map2 = dynamic_cast<BXON::Map*>(obj2);
    scene2 = new Scene::Compound_Basic(dev);
    scene2->Load(map2);*/
    

    //camera = (Scene::Camera*)scene2->GetDatablock(Scene::DATABLOCK_CAMERA, "Camera");

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

    dev->Enable(Graph::STATE_DEPTH_TEST);

    //basic->Enable();
    scene->Render();
   // scene2->Render();
    //basic->Disable();


    //scene2->Render();



    //camera->GetObject()->Play(time);

    dev->PresentAll();

    time += 0.4;


    //if (time > 100)
    //    time = 0.0;
}

void Demo14::UpdateWndEvents() {

}

Demo * CreateDemo_14(Core::Window * wnd, Graph::Device * dev) {
    return new Demo14(wnd, dev);
}
