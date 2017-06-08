
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_HTTP_SERVER_H_
#define _NCK_DEMO_HTTP_SERVER_H_

#include "../nckDemo.h"

class CustomRequestHandler;

class Demo_HttpServer : public Demo{
public:
    Demo_HttpServer(Core::Window * wnd, Graph::Device * dev);
	~Demo_HttpServer();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

    void RenderMessage(float y, float width, const std::string & msg, const Math::Color4ub & col);
private:
    CustomRequestHandler * creqHandler;
    Graph::Texture * fontTexture;
    Gui::FontMap * fontMap;
    Network::HttpServer * server;
};

Demo * CreateDemo_HttpServer(Core::Window * wnd, Graph::Device * dev);

#endif
