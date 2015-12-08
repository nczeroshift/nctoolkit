
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 3 - Http Server
*/

#ifndef _NCK_DEMO_3_H_
#define _NCK_DEMO_3_H_

#include "nckDemo.h"
#include "nckHttpServer.h"

class CustomRequestHandler;

/**
* Demo 3 class.
*/
class Demo3 : public Demo{
public:
	Demo3(Core::Window * wnd, Graph::Device * dev);
	~Demo3();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();

    void RenderMessage(float y, float width,const std::string & msg, const Math::Color4ub & col);
    
	CustomRequestHandler * creqHandler;
	Graph::Texture * fontTexture;
	Gui::FontMap * fontMap;
	Network::HttpServer * server;
};

Demo * CreateDemo_3(Core::Window * wnd, Graph::Device * dev);

#endif
