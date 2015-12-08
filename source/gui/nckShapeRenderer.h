
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_SHAPES_RENDER_H
#define NCK_SHAPES_RENDER_H

#include "nckGuiConfig.h"
#include "nckGraphics.h"
#include "nckImage.h"
#include "nckColor4.h"

#include <map>

_GUI_BEGIN

/**
* UI shape rendering class.
*/
class ShapeRenderer{
public:
	typedef enum{
		ROUND_ALL=0,
		ROUND_TOP=1,
		ROUND_LEFT=2,
		ROUND_RIGHT=3,
		ROUND_BOTTOM=4,
		ROUND_NONE=5,
	}RoundMode;

	ShapeRenderer(Graph::Device * dev);
	~ShapeRenderer();

	void SetRoundMode(RoundMode mode);
	RoundMode GetRoundMode();

	void SetRoundRadius(float radius);
	float GetRoundRadius();

	// Square rendering methods.
	void Square(float x,float y,float w, float h, const Math::Color4ub & col1);
	void SquareVGrad(float x,float y,float w, float h,const Math::Color4ub & col1,const Math::Color4ub & col2);
	void SquareHGrad(float x,float y,float w, float h,const Math::Color4ub & col1,const Math::Color4ub & col2);

	// Symbols and other forms rendering methods.
	void Triangle(float x,float y,float w,float h, const Math::Color4ub & col, bool side);
	void CheckSymbol(float x,float y,float size,const Math::Color4ub & col);
	void CircleShape(float x,float y,float size,const Math::Color4ub & col);

private:
	/// Get the circle texture from cache(or generate a new)
	Graph::Texture2D* UpdateCircleCache(int size);
	/// Get the check box symbol from cache(or generate a new)
	Graph::Texture2D* UpdateCheckCache(int size);
	/// Round corners radius.
	float roundRadius;
	/// Round corners mode.
	RoundMode roundMode;
	/// Round corners cached textures.
	std::map<int,Graph::Texture2D *> rsTextureCache;
	/// Check box symbols cached textures.
	std::map<int,Graph::Texture2D *> cbTextureCache;
	/// Reference to the graphics device.
	Graph::Device * device;
	/// Rounded corner rendering shaders.
	Graph::Program * rsProgram[5];
};

_GUI_END

#endif // #ifndef NCK_SHAPES_RENDER_H
