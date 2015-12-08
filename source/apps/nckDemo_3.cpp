
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 3 - Http Server
*/

#include "nckDemo_3.h"

class CustomRequestHandler : public Network::HttpCallbackHandler
{
	Core::Mutex * messageListMutex;
	int totalCounter;
	std::list<std::string> messagesList;
	bool backgroundYellow;
public:
	CustomRequestHandler()
	{
		totalCounter = 0;
		messageListMutex = Core::CreateMutex();
		backgroundYellow = false;
	}

	~CustomRequestHandler()
	{
		SafeDelete(messageListMutex);
	}

	bool isBackGroundYellow()
	{
		return backgroundYellow;
	}

    virtual bool GetRequest(const std::string & srcAddr,
                            const std::string & path,
                            const std::string & version,
                            const std::map<std::string, std::string> & headers)
	{
		AddMessage("GET { path : "+path+", version : "+version+" }");
        return true;
	}

    virtual void PostRequest(const std::string & srcAddr,
                             const std::string & path,
                             const std::string & version,
                             const std::map<std::string, std::string> & headers,
                             const std::map<std::string,std::string> & params)
    {
		AddMessage("POST { path : "+path+", version : "+version+" }");
	}

    virtual void UnsupportedRequest(const std::string & srcAddr,
                                    const std::string & path,
                                    const std::string & version,
                                    const std::string & headers)
    {
		AddMessage("UNSUPPORTED");
	}

	/*void SendFileEvent(const Network::Ipv6Address & src,
				const std::string & path,
				Network::MIMEType type){
		AddMessage("FILE DATA { path : "+path+" }");
	}*/

    virtual std::string JSONRequest(const std::string & srcAddr,
                                    std::map<std::string,std::string> params)
    {
		AddMessage("JSON webapp-serverapp");

		for(std::map<std::string,std::string>::iterator i = params.begin(); i != params.end();i++){
			AddMessage("  {"+(*i).first+":"+(*i).second.c_str()+"}");
		}

		if(params.find("background")!= params.end())
		{
			std::string retstate = "";

			if(backgroundYellow)
				retstate = "{\"state\":\"background was yellow\"}";
			else
				retstate = "{\"state\":\"background was gray\"}";

			if(params.find("background")->second=="default")
				backgroundYellow = false;
			else if(params.find("background")->second=="yellow")
				backgroundYellow = true;

			return retstate;
		}
		return "";
	}

	void AddMessage(const std::string & text)
	{
		totalCounter++;
		messageListMutex->Lock();
		messagesList.push_front(text);

		if(messagesList.size()>40)
			messagesList.pop_back();

		messageListMutex->Unlock();
	}

	void Render(int width, int height, Demo3 * d)
	{
		int y = height-16;
		int count = 0;
		messageListMutex->Lock();


		ListFor(std::string,messagesList,i)
		{
			Math::Color4ub col;
			if((++count +totalCounter) %2 == 0)
				col = Math::Color4ub(60,60,60,200);
			else
				col = Math::Color4ub(70,70,70,200);

			d->RenderMessage(y,width,(*i),col);

			y-=16;
		}

		messageListMutex->Unlock();
	}
};

Demo3::Demo3(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
	fontTexture = NULL;
	fontMap = NULL;

	server = NULL;
	try{
		server = Network::HttpServer::Create(8080);
	}
	catch (const Core::Exception & ex) {
		ex.PrintStackTrace();
		try{
			server = Network::HttpServer::Create(0); // Find an available port
		}
		catch (const Core::Exception & ex2) {
			ex.PrintStackTrace();
			return;
		}
	}

	creqHandler = new CustomRequestHandler();
	server->SetCallback(creqHandler);
}

Demo3::~Demo3(){
	SafeDelete(server);
	SafeDelete(creqHandler);
	SafeDelete(fontMap);
	SafeDelete(fontTexture);
}

void Demo3::Load(){
	dev->Enable(Graph::STATE_BLEND);
	dev->Disable(Graph::STATE_DEPTH_TEST);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA,Graph::BLEND_INV_SRC_ALPHA);
	
	fontTexture = dev->LoadTexture("texture://tex2d_font_ubuntu.png");
	fontTexture->SetFilter(Graph::FILTER_MAGNIFICATION,Graph::FILTER_NEAREST);
	fontTexture->SetFilter(Graph::FILTER_MINIFICATION,Graph::FILTER_NEAREST);
	fontTexture->SetFilter(Graph::FILTER_MIPMAPPING,Graph::FILTER_NEAREST);

	fontMap = new Gui::FontMap(dev);
	fontMap->Load("script://font_ubuntu.txt");

	wnd->SetTitle(wnd->GetTitle()+" (http://localhost:" + Math::IntToString(server->GetPort())+")");
}

void Demo3::Render(float dt){
	/*if(wnd->GetStatus() == Core::WINDOW_IDLE){
		Core::Thread::Wait(1);
		return;
	}*/

	if(creqHandler->isBackGroundYellow())
		dev->ClearColor(0.9,0.8,0.0,1);
	else
		dev->ClearColor(0.25,0.25,0.25,1);

	dev->Clear();
	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();

	dev->Ortho2D(wnd->GetWidth(),wnd->GetHeight());

	dev->MatrixMode(Graph::MATRIX_VIEW);
	dev->Identity();

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();

	creqHandler->Render(wnd->GetWidth(),wnd->GetHeight(),this);

	dev->PresentAll();
}

void Demo3::RenderMessage(float y, float width,const std::string & msg, const Math::Color4ub & col){
    RenderSquare2D(0,y,width,16,col);
    fontTexture->Enable();
    dev->Color(255,255,255,255);
    fontMap->Draw(10,y+8,16,msg);
    fontTexture->Disable();
}

void Demo3::UpdateWndEvents(){
	
}


Demo * CreateDemo_3(Core::Window * wnd, Graph::Device * dev){
	return new Demo3(wnd,dev);
}
