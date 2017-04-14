
/**
* NCtoolKit © 2007-2016 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 15 - Timeline
*/

#ifndef _NCK_DEMO_15_H_
#define _NCK_DEMO_15_H_

#include "nckDemo.h"
#include <map>
#include "bxonDataIO.h"

/**
* Demo 15 class.
*/
class Demo15 : public Demo, Scene::Processor{
public:
	Demo15(Core::Window * wnd, Graph::Device * dev);
	~Demo15();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    
private:
    virtual void HandleFinish(BXON::Map * map, Scene::Compound * compound);
    std::map<int, Scene::Camera*> cameras;
    Scene::Compound_Base * scene;
    float time;
};

Demo * CreateDemo_15(Core::Window * wnd, Graph::Device * dev);

#endif
