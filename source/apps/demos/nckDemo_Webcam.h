
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_WEBCAM_H_
#define _NCK_DEMO_WEBCAM_H_

#include "../nckDemo.h"

class Demo_Webcam : public Demo, public virtual Video::CameraCallback{
public:
    Demo_Webcam(Core::Window * wnd, Graph::Device * dev);
	~Demo_Webcam();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    void Capture(Video::Camera * camera, const void * outputData, size_t outputSize);

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

Demo * CreateDemo_Webcam(Core::Window * wnd, Graph::Device * dev);

#endif
