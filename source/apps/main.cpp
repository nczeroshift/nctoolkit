
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_Selector.h"

#include "nckDemo_1.h"
#include "nckDemo_2.h"
#include "nckDemo_3.h"
#include "nckDemo_4.h"
#include "nckDemo_5.h"
#include "nckDemo_6.h"
#include "nckDemo_7.h"
#include "nckDemo_8.h"
#include "nckDemo_9.h"
#include "nckDemo_11.h"
#include "nckDemo_10.h"
#include "nckDemo_12.h"
#include "nckDemo_13.h"

#define FRONTEND

class GraphicRendering : public virtual Core::Threadable, public virtual DemoSelector_Callback
{
public:
	GraphicRendering(Core::Window * window) : Threadable(){
		wnd = window;
		dev = NULL;
		mutex = Core::CreateMutex();
		
		demoSelector = NULL;
		demoActive = NULL;
		demoDestroy = NULL;
		demoLoad = NULL;
		pressedEscape = false;
	}
	
	~GraphicRendering(){
		SafeDelete(mutex);
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

		Demo * tmp = NULL;

		mutex->Lock();
		try{
#ifdef FRONTEND
			tmp = CreateDemo_Selector(wnd,dev);
			tmp->Load();
			((DemoSelector*)tmp)->SetCallback(this);
			demoSelector = tmp;
#else
			tmp = CreateDemo_10(wnd,dev);
			tmp->Load();
			demoActive = tmp;
#endif
		}catch(const Core::Exception & ex){
			ex.PrintStackTrace();
			SafeDelete(tmp);
			SafeDelete(dev);
		}
		mutex->Unlock();
				

		while(!IsTearingDown())
		{
			mutex->Lock();
			
			// Dealocate memory for previous demo.
			if(demoDestroy){
				SafeDelete(demoDestroy);
                wnd->SetTitle("nctoolkit");
			}

			// Load next demo in the graphics thread.
			if(demoLoad){
				try{
					demoLoad->Load();
					demoActive = demoLoad;
				}catch(const Core::Exception & ex){
					ex.PrintStackTrace();
					SafeDelete(demoLoad);
                    wnd->SetTitle("nctoolkit");
				}

				demoLoad = NULL;
			}


			if(demoActive) 
				demoActive->Render();
			else if(demoSelector)
				demoSelector->Render();

			mutex->Unlock();
		}

		mutex->Lock();
		SafeDelete(demoActive);
		SafeDelete(demoSelector);
		mutex->Unlock();

		SafeDelete(dev);
	}

	void UpdateWndInput()
	{
		mutex->Lock();

		if(demoActive)
			demoActive->UpdateWndEvents();
		else if(demoSelector)
			demoSelector->UpdateWndEvents();

		if(wnd->GetKeyStatus(Core::BUTTON_ESCAPE) != Core::BUTTON_STATUS_UP){
			if(!pressedEscape){
				if(demoActive){
					demoDestroy = demoActive;
					demoActive = NULL;
	#ifndef FRONTEND
					Teardown();
	#endif
				}
				else if(demoSelector){
					Teardown();
				}
			}
            pressedEscape = true;
		}
        else{
            pressedEscape = false;
        }

		mutex->Unlock();
	}

	void SelectDemo(int demoId)
	{
		Demo * tmp = NULL;
		try{
			switch(demoId)
			{
				case 1: tmp = new Demo1(wnd,dev); break;
				case 2: tmp = new Demo2(wnd,dev); break;
				case 3:	tmp = new Demo3(wnd,dev); break;
				case 4:	tmp = new Demo4(wnd,dev); break;
				case 5: tmp = new Demo5(wnd,dev); break;
				case 6: tmp = new Demo6(wnd,dev); break;
				case 7: tmp = new Demo7(wnd,dev); break;
				case 8: tmp = new Demo8(wnd,dev); break;
				case 9: tmp = new Demo9(wnd,dev); break;
                case 10: tmp = new Demo10(wnd,dev); break;
				case 11: tmp = new Demo11(wnd,dev); break;
                case 12: tmp = new Demo12(wnd, dev); break;
                case 13: tmp = new Demo13(wnd, dev); break;
			};
		}
		catch(const Core::Exception & ex){
			ex.PrintStackTrace();
			SafeDelete(tmp);
			tmp = NULL;
		}
		demoLoad = tmp;
	}

private:
	bool			pressedEscape;
	Demo			*demoSelector,*demoActive,*demoDestroy,*demoLoad;
	Core::Mutex		*mutex;
	Graph::Device	*dev;
	Core::Window	*wnd;
};

void Core::Application_Main(const std::vector<std::string> & CmdLine)
{
	// Create a window.
	Core::Window * wnd = Core::CreateWindow("nctoolkit",800,600,false);

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

