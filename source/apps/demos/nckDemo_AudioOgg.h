
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_AUDIO_OGG_H_
#define _NCK_DEMO_AUDIO_OGG_H_

#include "../nckDemo.h"

class AudioEventCallback;

class Demo_AudioOgg : public Demo{
public:
    Demo_AudioOgg(Core::Window * wnd, Graph::Device * dev);
	~Demo_AudioOgg();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    Audio::OutputDevice * audio;
    AudioEventCallback * callback;
};

Demo * CreateDemo_AudioOgg(Core::Window * wnd, Graph::Device * dev);

#endif
