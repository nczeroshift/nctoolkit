
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo Selector FrontEnd
*/

#ifndef _NCK_DEMO_SELECTOR_H_
#define _NCK_DEMO_SELECTOR_H_

#include "nckDemo.h"

class DemoDetails;

/**
* Demo selection callback/delegate.
*/
class DemoSelector_Callback{
public:
	virtual ~DemoSelector_Callback(){};
	virtual void SelectDemo(int demoId) = 0;
};

/**
* Frontend implementation, rendering and user interation.
*/
class DemoSelector : public Demo{
public:
	DemoSelector(Core::Window * wnd, Graph::Device * dev);
	~DemoSelector();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
	void SetCallback(DemoSelector_Callback * callback);
private:
	DemoSelector_Callback * m_Callback;
	Graph::Texture2D * m_FontTexture;
	Graph::Texture2D * m_BlurryBg;
	Graph::Texture2D * m_IconNck;
	Gui::FontMap * m_FontMap;

	std::vector<int> m_ScrollVelocitySamples;
	Core::Point m_LastCursorPosition;
	float m_ScrollPosition;
	float m_ScrollVelocity;
	int m_ScrollOffset;
	std::vector<DemoDetails*> m_DemoDetails;
	int m_TotalDeslocation;
	int m_ActiveDemoId;

	void RenderSquare(float x,float y,float w, float h, const Math::Color4ub & col1);
};

Demo * CreateDemo_Selector(Core::Window * wnd, Graph::Device * dev);

#endif
