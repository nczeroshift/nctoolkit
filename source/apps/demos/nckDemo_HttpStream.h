
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_HTTP_STREAM_H_
#define _NCK_DEMO_HTTP_STREAM_H_

#include "../nckDemo.h"

class Demo_HttpStream : public Demo, public Network::HttpCallbackHandler {
public:
    Demo_HttpStream(Core::Window * wnd, Graph::Device * dev);
	~Demo_HttpStream();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    bool HandleRequest(Network::HttpRequest * request, Network::HttpResponse * response);
    static void UpdateTitle(Demo_HttpStream * demo);

    Network::HttpServer * server;

    Scene::Compound_Base * model;
    Graph::Program * shader;
    Scene::Camera * camera;
    float zRotation;

    Core::Image * rawBuffer;
    unsigned char * fileBuffer;

    bool renderWireframe;
    bool fetchPicture;
};

Demo * CreateDemo_HttpStream(Core::Window * wnd, Graph::Device * dev);

#endif
