
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_H_
#define _NCK_DEMO_H_

#include "nckWindow.h"
#include "nckException.h"
#include "nckThread.h"

//#include "bXScene.h"

#include "nckUtils.h"
#include "nckDataIO.h"

#include "nckVec3.h"
#include "nckMat44.h"
#include "nckMathUtils.h"
#include "nckQuat.h"
#include "nckTriangle.h"
#include "nckBoundBox.h"
#include "nckQuadtree.h"
#include "nckLine.h"
#include "nckWidget.h"
#include "nckGraphics.h"
#include "nckFont.h"
#include "nckCompound.h"

class Demo{
public:
	virtual ~Demo(){};
	virtual void Load() = 0;
	virtual void Render(float dt = 1.0/60.0) = 0;
	virtual void UpdateWndEvents() = 0;

	void RenderLine(const Math::Line & line);
	void RenderDot(Math::Vec3 p,float size);
	void RenderSquare2D(float x,float y,float w, float h, const Math::Color4ub & col);
    
protected:
	Core::Window * wnd;
	Graph::Device * dev;
};

#endif
