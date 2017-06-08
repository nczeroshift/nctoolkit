
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_TEXTURE_2D_H_
#define _NCK_DEMO_TEXTURE_2D_H_

#include "../nckDemo.h"

class Demo_Texture2D : public Demo{
public:
    Demo_Texture2D(Core::Window * wnd, Graph::Device * dev);
	~Demo_Texture2D();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    Graph::Texture2D * m_R32Tex;
    Graph::Texture2D * m_RGBTex;
    Graph::Texture * m_Texture;
	float time;
};

Demo * CreateDemo_Texture2D(Core::Window * wnd, Graph::Device * dev);

#endif
