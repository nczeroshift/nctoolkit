
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_TEXTURE_3D_H_
#define _NCK_DEMO_TEXTURE_3D_H_

#include "../nckDemo.h"

class Demo_Texture3D : public Demo{
public:
    Demo_Texture3D(Core::Window * wnd, Graph::Device * dev);
	~Demo_Texture3D();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    Graph::Program * m_Texture3DProg;
    Graph::Texture3D * m_Texture;
	float time;
};

Demo * CreateDemo_Texture3D(Core::Window * wnd, Graph::Device * dev);

#endif
