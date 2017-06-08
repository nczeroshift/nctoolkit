
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_SERIAL_H_
#define _NCK_DEMO_SERIAL_H_

#include "../nckDemo.h"

class Demo_Serial : public Demo, public virtual IO::SerialCallback {
public:
    Demo_Serial(Core::Window * wnd, Graph::Device * dev);
	~Demo_Serial();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    void ReceiveData(unsigned char * data, unsigned int size);
    void HandleLine(const std::string & str);

    Gui::ShapeRenderer * shapes;
    Core::Mutex * mutex;
    IO::Serial * serial;
    std::string bufferedString;
    std::map<int, float> potsMap;
};

Demo * CreateDemo_Serial(Core::Window * wnd, Graph::Device * dev);

#endif
