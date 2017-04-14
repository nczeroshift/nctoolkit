
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 7 - Webcam
*/

#include "nckDemo_7.h"

Demo7::Demo7(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
	mutex = Core::CreateMutex();
	zRotation = 0;
	model = NULL;
	cameraDev = NULL;
	shape = NULL;
	videoWidth = 640;
	videoHeight = 480;
	videoTexture = NULL;
}

Demo7::~Demo7(){
	if(cameraDev)cameraDev->StopCapture();
	SafeDelete(videoTexture);
	SafeDelete(shape);
	SafeDelete(model);
	SafeDelete(mutex);
	SafeDelete(cameraDev);
}

void Demo7::Capture(Video::Camera * camera,const void * outputData, size_t outputSize){
	// Memory is managed by camera device and the pointer won't change outsite this 
	// mutex, except if the capture device instance is dealloced.
	mutex->Lock();
	targetData = (void*)outputData;
	changed = true;
	mutex->Unlock();
}

void Demo7::Load(){
	cameraDev = Video::OpenCamera("/dev/video0",videoWidth,videoHeight,Video::FORMAT_RGB_24);
	cameraDev->SetCallback(this);

	dev->Enable(Graph::STATE_BLEND);
	dev->Enable(Graph::STATE_DEPTH_TEST);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA,Graph::BLEND_INV_SRC_ALPHA);
	
	model = new Scene::Compound_Base(dev);
	model->Load("model://cube.nc");

	zRotation = 0;

	videoTexture = dynamic_cast<Graph::Texture2D*>(dev->CreateTexture(Graph::TEXTURE_2D,videoWidth,videoHeight,3,Graph::FORMAT_RGB_8B,false));

	cam = (Scene::Camera*)model->GetDatablock(Scene::DATABLOCK_CAMERA,"Camera");

	shape = new  Gui::ShapeRenderer(dev);

	cameraDev->StartCapture();
}


void Demo7::Render(float dt){
	dev->Clear();

	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();
	cam->Enable(Graph::MATRIX_PROJECTION);

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();
	cam->Enable(Graph::MATRIX_VIEW);
	dev->Enable(Graph::STATE_DEPTH_TEST);

	dev->Rotate(zRotation,1,1,1);

	videoTexture->Enable();
	model->Render();
	videoTexture->Disable();
	

	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();
	dev->Ortho2D(640,480,-1,1);

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();
	dev->Disable(Graph::STATE_DEPTH_TEST);

	videoTexture->Enable();
	shape->Square(0,0,160,120,Math::Color4ub());
	videoTexture->Disable();

	if(targetData!=NULL && changed){
		mutex->Lock();
		void * data = videoTexture->Lock();
		memcpy(data,targetData,videoWidth*videoHeight*3);
		videoTexture->Unlock();
		changed = false;
		mutex->Unlock();
	}

	dev->PresentAll();

	zRotation+=0.1;
}

void Demo7::UpdateWndEvents(){
	
}


Demo * CreateDemo_7(Core::Window * wnd, Graph::Device * dev){
	return new Demo7(wnd,dev);
}
