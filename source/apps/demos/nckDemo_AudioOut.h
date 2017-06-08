
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_AUDIO_OUT_H_
#define _NCK_DEMO_AUDIO_OUT_H_

#include "../nckDemo.h"

class Demo_AudioOut : public Demo{
public:
    Demo_AudioOut(Core::Window * wnd, Graph::Device * dev);
	~Demo_AudioOut();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
	float time;
};

Demo * CreateDemo_AudioOut(Core::Window * wnd, Graph::Device * dev);

#endif
