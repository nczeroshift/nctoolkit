
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 4 - Audio Playback
*/

#ifndef _NCK_DEMO_4_H_
#define _NCK_DEMO_4_H_

#include "nckDemo.h"
#include "nckAudioDevice.h"

class AudioEventCallback;

 /**
* Demo 4 class.
*/
class Demo4 : public Demo{
public:
	Demo4(Core::Window * wnd, Graph::Device * dev);
	~Demo4();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();

	Audio::OutputDevice * audio;
	AudioEventCallback * callback;
};

Demo * CreateDemo_4(Core::Window * wnd, Graph::Device * dev);

#endif
