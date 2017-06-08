
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_ARMATURE_H_
#define _NCK_DEMO_ARMATURE_H_

#include "../nckDemo.h"

class Demo_Armature : public Demo{
public:
    Demo_Armature(Core::Window * wnd, Graph::Device * dev);
	~Demo_Armature();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    Graph::Program * program;
    Scene::Compound_Base * scene;
    Math::Mat44 * boneMatrix;
	float time;
};

Demo * CreateDemo_Armature(Core::Window * wnd, Graph::Device * dev);

#endif
