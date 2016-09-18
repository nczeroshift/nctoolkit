
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 11 - BXON demo
*/

#include "nckDemo_12.h"


Demo12::Demo12(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    t1 = NULL;
    t2 = NULL;
    shape = NULL;
}

Demo12::~Demo12(){
    SafeDelete(shape);
    SafeDelete(t1);
    SafeDelete(t2);
}

void Demo12::Load(){
    dev->ClearColor(0, 0, 0);
	dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    shape = new Gui::ShapeRenderer(dev);

    t1 = dev->LoadTexture("texture://tex2d_caravela_1.png");
    t2 = dev->LoadTexture("texture://tex2d_caravela_2.png");

    time = 0;
}

void Demo12::Render(float dt){
	dev->Clear();

	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

    dev->Disable(Graph::STATE_DEPTH_TEST);

    // Set projection matrix.
    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();
    dev->Ortho2D(wnd->GetWidth(), wnd->GetHeight());

    // Set the model matrix.
    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();

    t1->Enable(0);
    t2->Enable(1);
    shape->Square(0, 0, 128, 128,Math::Color4ub());
    t1->Disable(0);
    t2->Disable(1);

    t2->Enable(0);
    shape->Square(256, 0, 128, 128, Math::Color4ub());
    t2->Disable(0);

    dev->PresentAll();
}

void Demo12::UpdateWndEvents(){

}

Demo * CreateDemo_12(Core::Window * wnd, Graph::Device * dev){
	return new Demo12(wnd,dev);
}
