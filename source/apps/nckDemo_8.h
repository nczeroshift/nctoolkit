
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 8 - Http & Screen capture stream
*/

#ifndef _NCK_DEMO_8_H_
#define _NCK_DEMO_8_H_

#include "nckDemo.h"
#include "nckHttpServer.h"

/**
* Demo 8 class.
*/
class Demo8 : public Demo, public Network::HttpCallbackHandler{
public:
	Demo8(Core::Window * wnd, Graph::Device * dev);
	~Demo8();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();

    virtual size_t GetImage(const std::string & srcAddr, Core::ImageType type,
                            std::map<std::string,std::string> params, unsigned char ** data );

	static void UpdateTitle(Demo8 * demo);

	Network::HttpServer * server;

	Scene::Compound_Basic * model;
	Graph::Program * shader;
	Scene::Camera * camera;
	float zRotation;

	Core::Image * rawBuffer;
	unsigned char * fileBuffer;

	bool renderWireframe;
	bool fetchPicture;
};

Demo * CreateDemo_8(Core::Window * wnd, Graph::Device * dev);

#endif
