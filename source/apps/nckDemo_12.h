
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 11 - BXON Demo
*/

#ifndef _NCK_DEMO_12_H_
#define _NCK_DEMO_12_H_

#include "nckDemo.h"
#include <map>
#include "bxonDataIO.h"

/**
* Demo 12 class.
*/
class Demo12 : public Demo{
public:
	Demo12(Core::Window * wnd, Graph::Device * dev);
	~Demo12();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    
private:
    Gui::ShapeRenderer * shape;
    Graph::Texture  * t1, *t2;
    float time;
};

Demo * CreateDemo_12(Core::Window * wnd, Graph::Device * dev);

#endif
