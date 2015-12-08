
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckWindow.h"
#include "nckThread.h"
#include "nckGraphics.h"
#include "nckVec3.h"
#include "nckMat44.h"
#include "nckException.h"
#include "nckUtils.h"

class GraphicRendering : public virtual Core::Threadable
{
public:
	GraphicRendering(Core::Window * window) : Threadable(){
		wnd = window;
		dev = NULL;
	}
	
	~GraphicRendering(){
	}
	
	void Run(){
		wnd->SetTitle("nctoolkit");
		
		try{
			dev = Graph::CreateDevice(wnd,Graph::DEVICE_AUTO,wnd->GetWidth(),wnd->GetHeight());
			dev->ClearColor(0.5,0.5,0.5,1);
			dev->ClearFlags(Graph::BUFFER_COLOR_BIT|Graph::BUFFER_DEPTH_BIT);
		}
		catch(const Core::Exception & ex){
			ex.PrintStackTrace();
			SafeDelete(dev);
			return;
		}

		while(!IsTearingDown())
		{
			dev->Clear();

			dev->PresentAll();
		}

		SafeDelete(dev);
	}

	void UpdateWndInput()
	{

		if (wnd->GetKeyStatus(Core::BUTTON_ESCAPE) != Core::BUTTON_STATUS_UP)
			Teardown();

	}

	void SelectDemo(int demoId)
	{
		
	}

private:
	Graph::Device	*dev;
	Core::Window	*wnd;
};

void Core::Application_Main(const std::vector<std::string> & CmdLine)
{
	// Create a window.
	Core::Window * wnd = Core::CreateWindow("nctoolkit", 1024, 768, false);

	// Create the thread where the graphics rendering will run.
	GraphicRendering * grThread = new GraphicRendering(wnd);

	// Start graphics thread.
	grThread->Start();

	// Get window events.
	while(wnd->Peek(true))
	{
		// If on selector frontend, escape will tell
		// the thread to enter shutdown.
		if(!grThread->IsRunning())
			break;

		grThread->UpdateWndInput();
	}

	// Tell the thread to end and wait.
	grThread->TearDownAndJoin();

	// Release stuff from memory.
	SafeDelete(grThread);
	SafeDelete(wnd);
}

