
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 9 - Serial port demo
*/

#include "nckDemo_9.h"

Demo9::Demo9(Core::Window * wnd, Graph::Device * dev) : serial(NULL){
	this->dev = dev;
	this->wnd = wnd;
    serial = NULL;
	mutex = Core::CreateMutex();
}

Demo9::~Demo9(){
	SafeDelete(shapes);
    if(serial) serial->Stop();
	SafeDelete(serial);
	SafeDelete(mutex);
}

void Demo9::ReceiveData(unsigned char * data,unsigned int size){
	if(size > 0 && size < 4096){
		char * tmp = new char[size+1];
		memset(tmp,0,size+1);
		memcpy(tmp,data,size);

		bufferedString += std::string(tmp);

		delete [] tmp;

		while(true)
		{
			size_t split = 0;
			if((split = bufferedString.find_first_of("\n\r")) != std::string::npos){
				std::string res = bufferedString.substr(0,split);
				std::string rem = bufferedString.substr(split+2,bufferedString.length()-2);
				HandleLine(res);
				bufferedString = rem;
			}
			else
				break;
		}
	}
}

void Demo9::HandleLine(const std::string & str){
	int potId = 0; 
	int angle = 0;
	// The atmega just has to print "0,512\n\r1,127\n\r" in this format to feed the program. 
	if(sscanf(str.c_str(),"%d,%d",&potId,&angle) == 2){
		mutex->Lock();
		potsMap.insert(std::pair<int,float>(potId,angle/1024.0f));
		mutex->Unlock();
	}
}

void Demo9::Load(){
	dev->Enable(Graph::STATE_BLEND);
	dev->Enable(Graph::STATE_DEPTH_TEST);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA,Graph::BLEND_INV_SRC_ALPHA);

	shapes = new Gui::ShapeRenderer(dev);

	serial = IO::OpenSerial("/dev/ttyUSB0",IO::BAUDRATE_9600);
    
    if(!serial)
        THROW_EXCEPTION("Error opening serial port");
        
    serial->SetCallback(this);
	serial->Start();
}

void Demo9::Render(float dt){
	dev->Clear();

	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();
	dev->Ortho2D((float)wnd->GetWidth(),(float)wnd->GetHeight());

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();
	dev->Disable(Graph::STATE_DEPTH_TEST);

	mutex->Lock();
	{
		float yVal = 10.0f;
		float xVal = 10.0f;
		float xMaxWidth = 256.0f;
		float yMaxHeight = 16.0f;

		Math::Color4ub colors[7] = {
			Math::Color4ub(255,100,0),
			Math::Color4ub(100,255,0),
			Math::Color4ub(0,100,255),
			Math::Color4ub(100,0,255),
			Math::Color4ub(0,255,0),
			Math::Color4ub(255,0,0),
			Math::Color4ub(0,0,255)
		};

		for(std::map<int,float>::iterator i = potsMap.begin();i!=potsMap.end();i++)
		{
			float alpha = i->second;
			shapes->Square(xVal,yVal,xMaxWidth*alpha,yMaxHeight,colors[i->first % 7]);
			yVal += yMaxHeight*1.5f;
		}
	}
	mutex->Unlock();

	dev->PresentAll();
}

void Demo9::UpdateWndEvents(){
	static bool btnStatusC = false;
	static bool btnStatusZ = false;
	if(!btnStatusC && wnd->GetKeyStatus(Core::BUTTON_C) == Core::BUTTON_STATUS_DOWN){
		const char * simpleString = "foo\n\r";
		size_t size = strlen(simpleString);
		serial->SendData((unsigned char*)simpleString,(unsigned int)size);
	}
	btnStatusC = wnd->GetKeyStatus(Core::BUTTON_C) == Core::BUTTON_STATUS_DOWN;

	if(!btnStatusZ && wnd->GetKeyStatus(Core::BUTTON_Z) == Core::BUTTON_STATUS_DOWN){
		const char * simpleString = "bar\n\r";
		size_t size = strlen(simpleString);
		serial->SendData((unsigned char*)simpleString,(unsigned int)size);
	}
	btnStatusZ = wnd->GetKeyStatus(Core::BUTTON_Z) == Core::BUTTON_STATUS_DOWN;
}


Demo * CreateDemo_9(Core::Window * wnd, Graph::Device * dev){
	return new Demo9(wnd,dev);
}
