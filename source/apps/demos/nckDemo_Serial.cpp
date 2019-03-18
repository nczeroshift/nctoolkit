
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_Serial.h"
#include <string.h>

Demo_Serial::Demo_Serial(Core::Window * wnd, Graph::Device * dev) {
    this->dev = dev;
    this->wnd = wnd;
    serial = NULL;
    mutex = NULL;
    shapes = NULL;
}

Demo_Serial::~Demo_Serial(){
    SafeDelete(shapes);
    if (serial) serial->Stop();
    SafeDelete(serial);
    SafeDelete(mutex);
}

void Demo_Serial::Load(){
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    mutex = Core::CreateMutex();

    shapes = new Gui::ShapeRenderer(dev);

    serial = IO::OpenSerial("COM3", IO::BAUDRATE_115200);

    if (!serial)
        THROW_EXCEPTION("Error opening serial port");

    serial->SetCallback(this);
    serial->Start();
}

void Demo_Serial::Render(float dt){
    dev->Clear();

    dev->Viewport(0, 0, wnd->GetWidth(), wnd->GetHeight());

    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();
    dev->Ortho2D((float)wnd->GetWidth(), (float)wnd->GetHeight());

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

        for (std::map<int, float>::iterator i = potsMap.begin(); i != potsMap.end(); i++)
        {
            float alpha = i->second;
            shapes->Square(xVal, yVal, xMaxWidth*alpha, yMaxHeight, colors[i->first % 7]);
            yVal += yMaxHeight*1.5f;
        }
    }
    mutex->Unlock();

    dev->PresentAll();
}


void Demo_Serial::ReceiveData(unsigned char * data, unsigned int size) {
    if (size > 0 && size < 4096) {
        char * tmp = new char[size + 1];
        memset(tmp, 0, size + 1);
        memcpy(tmp, data, size);

        bufferedString += std::string(tmp);

        delete[] tmp;

        while (true)
        {
            size_t split = 0;
            if (split = bufferedString.find_first_of("\n") != std::string::npos) {
                std::string res = bufferedString.substr(0, split);

				std::string rem = "";
				if (bufferedString.length() > 1) {
					rem = bufferedString.substr(split + 1, bufferedString.length() - 1);
				}

                HandleLine(res);
                bufferedString = rem;
            }
            else
                break;
        }
    }
}

void Demo_Serial::HandleLine(const std::string & str) {
    int potId = 0;
    int angle = 0;
	Core::DebugLog(str+"\n");
    // The atmega just has to print "0,512\n\r1,127\n\r" in this format to feed the program. 
    /*if (sscanf(str.c_str(), "%d,%d", &potId, &angle) == 2) {
        mutex->Lock();
        potsMap.insert(std::pair<int, float>(potId, angle / 1024.0f));
        mutex->Unlock();
    }*/
}

void Demo_Serial::UpdateWndEvents(){
    static bool btnStatusC = false;
    static bool btnStatusZ = false;
    if (!btnStatusC && wnd->GetKeyStatus(Core::BUTTON_C) == Core::BUTTON_STATUS_DOWN) {
        const char * simpleString = "foo\n\r";
        size_t size = strlen(simpleString);
        serial->SendData((unsigned char*)simpleString, (unsigned int)size);
    }
    btnStatusC = wnd->GetKeyStatus(Core::BUTTON_C) == Core::BUTTON_STATUS_DOWN;

    if (!btnStatusZ && wnd->GetKeyStatus(Core::BUTTON_Z) == Core::BUTTON_STATUS_DOWN) {
        const char * simpleString = "bar\n\r";
        size_t size = strlen(simpleString);
        serial->SendData((unsigned char*)simpleString, (unsigned int)size);
    }
    btnStatusZ = wnd->GetKeyStatus(Core::BUTTON_Z) == Core::BUTTON_STATUS_DOWN;
}

std::vector<std::string> Demo_Serial::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("TODO");
    //ret.push_back("Basic");
    //ret.push_back("Animation");
    return ret;
}

std::string Demo_Serial::GetDescription() {
    return "Serial Port Device";
}

Demo * CreateDemo_Serial(Core::Window * wnd, Graph::Device * dev){
	return new Demo_Serial(wnd,dev);
}
