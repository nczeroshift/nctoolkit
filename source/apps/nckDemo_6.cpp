
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 6 - 3D Texture
*/

#include "nckDemo_6.h"


Demo6::Demo6(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
	shader = NULL;
	model = NULL;
	camera = NULL;
	volume = NULL;
}

Demo6::~Demo6(){
	SafeDelete(shader);
	SafeDelete(model);
	SafeDelete(volume);
}

void Demo6::Load(){
	dev->Enable(Graph::STATE_BLEND);
	dev->Enable(Graph::STATE_DEPTH_TEST);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA,Graph::BLEND_INV_SRC_ALPHA);
	
	shader = dev->LoadProgram("shader://dotpattern.cpp");
	model = new Scene::Compound_Base(dev);
	model->Load("model://caravela.nc");

	volume = dynamic_cast<Graph::Texture3D*>(dev->LoadTexture("texture://tex3d_mangadots.tar",false));
	volume->SetFilter(Graph::FILTER_MIPMAPPING,Graph::FILTER_NONE);
		
	zRotation = 0;
	camera = (Scene::Camera*)model->GetDatablock(Scene::DATABLOCK_CAMERA,"Camera");
}

void Demo6::Render(float dt){
	dev->Clear();

	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();

	camera->Enable(Graph::MATRIX_PROJECTION);

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();

	camera->Enable(Graph::MATRIX_VIEW);

	dev->Rotate(zRotation,0,0,1);

	volume->Enable(1);
	shader->SetVariable1i("diffuse",0);
	shader->SetVariable1i("volume",1);
	shader->Enable();
	model->Render();
	shader->Disable();
	volume->Disable(1);

	zRotation+=0.1;

	dev->PresentAll();
}

void Demo6::UpdateWndEvents(){
	
}


Demo * CreateDemo_6(Core::Window * wnd, Graph::Device * dev){
	return new Demo6(wnd,dev);
}
