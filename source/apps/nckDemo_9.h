
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 9 - Serial port demo
*/

#ifndef _NCK_DEMO_9_H_
#define _NCK_DEMO_9_H_

#include "nckDemo.h"
#include "nckSerialPort.h"

#include <map>

/**
* Demo 9 class.
*/
class Demo9 : public Demo, public virtual IO::SerialCallback{
public:
	Demo9(Core::Window * wnd, Graph::Device * dev);
	~Demo9();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
	void ReceiveData(unsigned char * data,unsigned int size);

	void HandleLine(const std::string & str);

	Gui::ShapeRenderer * shapes;
	Core::Mutex * mutex;
	IO::Serial * serial;

	std::string bufferedString;
	std::map<int,float> potsMap;
};

Demo * CreateDemo_9(Core::Window * wnd, Graph::Device * dev);

#endif
