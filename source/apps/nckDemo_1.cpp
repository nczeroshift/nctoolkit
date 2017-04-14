
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 1 - Caravela Portuguesa
*/

#include "nckDemo_1.h"

Demo1::Demo1(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
	shader = NULL;
	model = NULL;
	camera = NULL;
}

Demo1::~Demo1(){
	SafeDelete(shader);
	SafeDelete(model);
}

void Demo1::Load(){
	dev->Enable(Graph::STATE_BLEND);
	dev->Enable(Graph::STATE_DEPTH_TEST);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA,Graph::BLEND_INV_SRC_ALPHA);
	
	// Load the shader program to render the caravela.
	shader = dev->LoadProgram("shader://caravela.cpp");

	// Create the compound where the caravela model will be loaded.
	model = new Scene::Compound_Base(dev);

	// Load the caravela compound model from the data/models folder.
	model->Load("model://caravela.nc");

	zRotation = 0;

	camera = (Scene::Camera*)model->GetDatablock(Scene::DATABLOCK_CAMERA,"Camera");
}

void Demo1::Render(float dt){
	// Clear graphics rendering buffer.
	dev->Clear();

	// Define the rendering area in window space.
	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

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

	// Rotate the caravela in the Z axis.
	dev->Rotate(zRotation,0,0,1);

	// Set the shader tex1 as the first enabled texture(use in the model material).
	shader->SetVariable1i("tex1",0);

	// Enable the shader.
	shader->Enable();

	// Render the caravela.
	model->Render();

	// Disable the shader.
	shader->Disable();

	// Keep increasing the rotation.
	zRotation += 0.1;

	// Finish rendering and present the graphics.
	dev->PresentAll();
}

void Demo1::UpdateWndEvents(){
	
}

Demo * CreateDemo_1(Core::Window * wnd, Graph::Device * dev){
	return new Demo1(wnd,dev);
}
