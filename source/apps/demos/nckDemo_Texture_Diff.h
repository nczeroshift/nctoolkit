
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_TEXTURE_DIFF_H_
#define _NCK_DEMO_TEXTURE_DIFF_H_

#include "../nckDemo.h"

class Demo_TextureDiff : public Demo{
public:
	Demo_TextureDiff(Core::Window * wnd, Graph::Device * dev);
	~Demo_TextureDiff();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    Graph::Texture2D * m_Normal;
    Graph::Texture2D * m_Laser;
	
	float time;
};

Demo * CreateDemo_Texture_Diff(Core::Window * wnd, Graph::Device * dev);

#endif
