
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_AUDIO_FFT_H_
#define _NCK_DEMO_AUDIO_FFT_H_

#include "../nckDemo.h"

class Demo_AudioFFT : public Demo{
public:
    Demo_AudioFFT(Core::Window * wnd, Graph::Device * dev);
	~Demo_AudioFFT();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    Graph::Texture * texture;
	float time;
};

Demo * CreateDemo_AudioFFT(Core::Window * wnd, Graph::Device * dev);

#endif
