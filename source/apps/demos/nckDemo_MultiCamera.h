
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_MULTI_CAMERA_H_
#define _NCK_DEMO_MULTI_CAMERA_H_

#include "../nckDemo.h"

class Demo_MultiCamera : public Demo, Scene::Processor {
public:
    Demo_MultiCamera(Core::Window * wnd, Graph::Device * dev);
	~Demo_MultiCamera();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    virtual void HandleFinish(BXON::Map * map, Scene::Compound * compound);
    Scene::Object* findNearestCamera(int keyframe);

    std::vector<std::pair<float, Scene::Object*>> camerasPerMarker;
    Scene::Compound_Base * scene;
	float time;
};

Demo * CreateDemo_MultiCamera(Core::Window * wnd, Graph::Device * dev);

#endif
