
/**
* NCtoolKit © 2007-2016 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 15 - Shadow maps
*/

#include "nckDemo_15.h"

Demo15::Demo15(Core::Window * wnd, Graph::Device * dev) {
    this->dev = dev;
    this->wnd = wnd;
    scene = NULL;
    time = 0;

}

Demo15::~Demo15() {
    SafeDelete(scene);
}

void Demo15::Load() {
    dev->ClearColor(0, 0, 0);
    dev->Enable(Graph::STATE_BLEND);
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);
 
    scene = new Scene::Compound_Basic(dev);
    scene->Load("model://timeline.bxon", this);
}

void Demo15::HandleFinish(BXON::Map * map, Scene::Compound * compound) {
    if (map->HasKey("tl_markers")) {
        BXON::Array * markers = map->GetArray("tl_markers");
        for (size_t i = 0; i < markers->GetSize(); i++) {
            BXON::Map * entry = markers->GetMap(i);
            int frameValue = entry->GetInteger("frame");
            if (entry->HasKey("camera")) {
                std::string camName = entry->GetString("camera");
                Core::DebugLog(Math::IntToString(frameValue)+">"+camName+"\n");
            }
        }
    }
}

void Demo15::Render(float dt) {

    dev->ClearColor(0, 0, 0, 0);
    dev->Clear();
    dev->Viewport(0, 0, wnd->GetWidth(), wnd->GetHeight());
    
    scene->Render();

    dev->PresentAll();

    time += 0.01;
}


void Demo15::UpdateWndEvents() {

}

Demo * CreateDemo_15(Core::Window * wnd, Graph::Device * dev) {
    return new Demo15(wnd, dev);
}
