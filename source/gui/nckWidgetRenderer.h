
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_WIDGET_RENDERER_H
#define NCK_WIDGET_RENDERER_H

#include "nckFont.h"
#include "nckShapeRenderer.h"
#include "nckFont.h"
#include <list>

_GUI_BEGIN

enum WidgetState{
	WIDGET_NORMAL,
	WIDGET_HIGHLIGHTED,
	WIDGET_PRESSED,
	WIDGET_DISABED
};

/**
 * Abstract class for UI shapes rendering.
 */
class WidgetRenderer
{
public:
	virtual ~WidgetRenderer(){};

	virtual void Button(float x, float y, float w, float h,const std::string & text,
			WidgetState state, ShapeRenderer::RoundMode mode = ShapeRenderer::ROUND_ALL)=0;

	virtual void RangeOptionButton(float x, float y, float w, float h,const std::string & text,
			WidgetState state, ShapeRenderer::RoundMode mode = ShapeRenderer::ROUND_ALL)=0;

	virtual void RangeValueButton(float x, float y, float w, float h,const std::string & text,
			WidgetState state, ShapeRenderer::RoundMode mode = ShapeRenderer::ROUND_ALL)=0;

	virtual void ToggleButton(float x, float y, float w, float h,const std::string & text,
			bool selected,
			WidgetState state, ShapeRenderer::RoundMode mode = ShapeRenderer::ROUND_ALL)=0;

	virtual void CheckBoxButton(float x, float y, float w, float h,const std::string & text,
			bool checked,
			WidgetState state, ShapeRenderer::RoundMode mode = ShapeRenderer::ROUND_ALL)=0;

	virtual void Tooltip(float x,float y,const std::string & text)=0;

	virtual void Label(float x,float y, const std::string & text,float size,bool bold,
			const Math::Color4ub & color,FontAlignment align = FONT_ALIGNMENT_LEFT)=0;

	virtual void PushTranslation(float x,float y)=0;
	virtual void PopTranslation()=0;

protected:
	Graph::Device * device;
	ShapeRenderer * shapeRenderer;
	FontMap * fontMap;
	Graph::Texture * fontTexture;
};

/**
 * Blender 25 UI rendering class
 */
class BlenderWidgetRenderer: public virtual WidgetRenderer{
public:
	BlenderWidgetRenderer(Graph::Device * dev,ShapeRenderer * shp,FontMap *fMap, Graph::Texture * fTex);
	virtual ~BlenderWidgetRenderer();

	void Button(float x, float y, float w, float h,const std::string & text,
				WidgetState state, ShapeRenderer::RoundMode mode);

	void RangeOptionButton(float x, float y, float w, float h,const std::string & text,
			WidgetState state, ShapeRenderer::RoundMode mode = ShapeRenderer::ROUND_ALL);

	void RangeValueButton(float x, float y, float w, float h,const std::string & text,
			WidgetState state, ShapeRenderer::RoundMode mode = ShapeRenderer::ROUND_ALL);

	void ToggleButton(float x, float y, float w, float h,const std::string & text,
				bool selected,
				WidgetState state,ShapeRenderer::RoundMode mode = ShapeRenderer::ROUND_ALL);

	void CheckBoxButton(float x, float y, float w, float h,const std::string & text,
			bool checked,
			WidgetState state, ShapeRenderer::RoundMode mode = ShapeRenderer::ROUND_ALL);

	void Label(float x,float y, const std::string & text,float size,bool bold,
			const Math::Color4ub & color,FontAlignment align = FONT_ALIGNMENT_LEFT);

	void Tooltip(float x,float y,const std::string & text);

	void PushTranslation(float x,float y);
	void PopTranslation();
};

_GUI_END

#endif // #ifndef NCK_WIDGET_RENDERER_H
