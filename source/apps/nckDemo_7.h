
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 7 - Webcam
*/

#ifndef _NCK_DEMO_7_H_
#define _NCK_DEMO_7_H_

#include "nckDemo.h"
#include "nckCameraDevice.h"

/**
* Demo 7 class.
*/
class Demo7 : public Demo, public virtual Video::CameraCallback{
public:
	Demo7(Core::Window * wnd, Graph::Device * dev);
	~Demo7();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
	void Capture(Video::Camera * camera,const void * outputData, size_t outputSize);

	Graph::Texture2D * videoTexture;
	Core::Mutex * mutex;
	Video::Camera * cameraDev;
	Gui::ShapeRenderer * shape;
	Scene::Compound_Base * model;
	Scene::Camera * cam;

	void * targetData;
	bool changed;
	float zRotation;
	int videoWidth;
	int videoHeight;
};

Demo * CreateDemo_7(Core::Window * wnd, Graph::Device * dev);

#endif
