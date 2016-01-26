
/**
 * NCtoolKit © 2007-2015 Lu’s F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckWidgetRenderer.h"

_GUI_BEGIN

BlenderWidgetRenderer::BlenderWidgetRenderer(Graph::Device * dev,ShapeRenderer * shp,FontMap *fMap, Graph::Texture * fTex){
	device = dev;
	shapeRenderer = shp;
	fontTexture = fTex;
	fontMap = fMap;
}

BlenderWidgetRenderer::~BlenderWidgetRenderer(){

}

void BlenderWidgetRenderer::Button(float x, float y, float w, float h,const std::string & text,
			WidgetState state, ShapeRenderer::RoundMode mode){
	shapeRenderer->SetRoundMode(mode);
	shapeRenderer->SetRoundRadius(5);

	shapeRenderer->Square(x+2,y+2,w,h,Math::Color4ub(135,135,135));
	shapeRenderer->Square(x-1,y-1,w+2,h+2,Math::Color4ub(55,55,55));
	if(state==WIDGET_NORMAL){
		shapeRenderer->SquareVGrad(x,y,w,h,Math::Color4ub(166,166,166),Math::Color4ub(138,138,138));
		device->Color(0,0,0, 255);
		fontTexture->Enable();
		fontMap->Draw(x+w/2, y+h/2, 16, text, false, FONT_ALIGNMENT_MIDDLE);
		fontTexture->Disable();
	}
	else if(state==WIDGET_PRESSED){
		shapeRenderer->SquareVGrad(x,y,w,h,Math::Color4ub(85,85,85),Math::Color4ub(113,113,113));
		fontTexture->Enable();
		device->Color(220,220,220,255);
		fontMap->Draw(x+w/2, y+h/2, 16, text, false, FONT_ALIGNMENT_MIDDLE);
		fontTexture->Disable();
	}
	else if(state==WIDGET_HIGHLIGHTED){
		shapeRenderer->SquareVGrad(x,y,w,h,Math::Color4ub(181,181,181),Math::Color4ub(153,153,153));
		fontTexture->Enable();
		device->Color(0,0,0,255);
		fontMap->Draw(x+w/2, y+h/2, 16, text, false, FONT_ALIGNMENT_MIDDLE);
		fontTexture->Disable();
	}
}

void BlenderWidgetRenderer::RangeValueButton(float x, float y, float w, float h,const std::string & text,
			WidgetState state,ShapeRenderer::RoundMode mode){

	shapeRenderer->SetRoundMode(mode);
	shapeRenderer->SetRoundRadius(5);

	shapeRenderer->Square(x+2,y+2,w,h,Math::Color4ub(135,135,135));
	shapeRenderer->Square(x-1,y-1,w+2,h+2,Math::Color4ub(55,55,55));
	if(state == WIDGET_NORMAL){
		shapeRenderer->SquareVGrad(x,y,w,h,Math::Color4ub(160,160,160),Math::Color4ub(178,178,178));
		shapeRenderer->Triangle(x+4,y+h*0.5-4,5,8,Math::Color4ub(117,117,117),false);
		shapeRenderer->Triangle(x+w-9,y+h*0.5-4,5,8,Math::Color4ub(117,117,117),true);
		fontTexture->Enable();
		device->Color(0,0,0, 255 );
		fontMap->Draw(x+w/2,y+h/2,16,text,false,FONT_ALIGNMENT_MIDDLE);
		fontTexture->Disable();
	}
	else if(state == WIDGET_PRESSED){
		shapeRenderer->SquareVGrad(x,y,w,h,Math::Color4ub(133,133,133),Math::Color4ub(151,151,151));
		fontTexture->Enable();
		device->Color( 255, 255,255, 200 );
		fontMap->Draw(x+w/2, y+h/2, 16, text, false, FONT_ALIGNMENT_MIDDLE);
		fontTexture->Disable();
		shapeRenderer->Triangle(x+4,y+h*0.5-4,5,8,Math::Color4ub(117,117,117),false);
		shapeRenderer->Triangle(x+w-9,y+h*0.5-4,5,8,Math::Color4ub(117,117,117),true);
	}
	else if(state == WIDGET_HIGHLIGHTED){
		shapeRenderer->SquareVGrad(x,y,w,h,Math::Color4ub(175,175,175),Math::Color4ub(193,193,193));
		fontTexture->Enable();
		device->Color(0,0,0,255);
		fontMap->Draw( x + w/2, y + h/2, 16, text, false, FONT_ALIGNMENT_MIDDLE);
		fontTexture->Disable();
		shapeRenderer->Triangle(x+4,y+h*0.5-4,5,8,Math::Color4ub(117,117,117),false);
		shapeRenderer->Triangle(x+w-9,y+h*0.5-4,5,8,Math::Color4ub(117,117,117),true);
	}
}


void BlenderWidgetRenderer::RangeOptionButton(float x, float y, float w, float h,const std::string & text,
			WidgetState state,ShapeRenderer::RoundMode mode)
{
	shapeRenderer->SetRoundMode(mode);
	shapeRenderer->SetRoundRadius(5);

	shapeRenderer->Square(x+2,y+2,w,h,Math::Color4ub(135,135,135));
	shapeRenderer->Square(x-1,y-1,w+2,h+2,Math::Color4ub(39,39,39));

	if(state == WIDGET_NORMAL){
		shapeRenderer->SquareVGrad(x,y,w,h,Math::Color4ub(83,83,83),Math::Color4ub(55,55,55));
		fontTexture->Enable();
		device->Color(255,255,255,255);
		fontMap->Draw(x+w/2,y+h/2,16,text,false,FONT_ALIGNMENT_MIDDLE);
		fontTexture->Disable();
		shapeRenderer->Triangle(x+4,y+h*0.5-4,5,8,Math::Color4ub(200,200,200),false);
		shapeRenderer->Triangle(x+w-9,y+h*0.5-4,5,8,Math::Color4ub(200,200,200),true);
	}
	else if(state == WIDGET_PRESSED){
		shapeRenderer->SquareVGrad(x,y,w,h,Math::Color4ub(71,113,179),Math::Color4ub(99,141,207));
		fontTexture->Enable();
		device->Color(0,0,0,255);
		fontMap->Draw(x+w/2, y+h/2, 16, text, false, FONT_ALIGNMENT_MIDDLE);
		fontTexture->Disable();
		shapeRenderer->Triangle(x+4,y+h*0.5-4,5,8,Math::Color4ub(200,200,200),false);
		shapeRenderer->Triangle(x+w-9,y+h*0.5-4,5,8,Math::Color4ub(200,200,200),true);
	}
	else if(state == WIDGET_HIGHLIGHTED){
		shapeRenderer->SquareVGrad(x,y,w,h,Math::Color4ub(98,98,98),Math::Color4ub(70,70,70));
		fontTexture->Enable();
		device->Color(255,255,255,255);
		fontMap->Draw( x + w/2, y + h/2, 16, text, false, FONT_ALIGNMENT_MIDDLE);
		fontTexture->Disable();
		shapeRenderer->Triangle(x+4,y+h*0.5-4,5,8,Math::Color4ub(200,200,200),false);
		shapeRenderer->Triangle(x+w-9,y+h*0.5-4,5,8,Math::Color4ub(200,200,200),true);
	}
}

void BlenderWidgetRenderer::ToggleButton(float x, float y, float w, float h,const std::string & text,bool selected,
			WidgetState state,ShapeRenderer::RoundMode mode)
{
	shapeRenderer->SetRoundMode(mode);
	shapeRenderer->SetRoundRadius(5);

	shapeRenderer->Square(x+2,y+2,w,h,Math::Color4ub(135,135,135));
	shapeRenderer->Square(x-1,y-1,w+2,h+2,Math::Color4ub(39,39,39));

	if(state == WIDGET_PRESSED || selected){
		shapeRenderer->SquareVGrad(x,y,w,h,Math::Color4ub(71,113,179),Math::Color4ub(99,141,207));
		fontTexture->Enable();
		device->Color(0,0,0,255);
		fontMap->Draw(x+w/2, y+h/2, 16, text, false, FONT_ALIGNMENT_MIDDLE);
		fontTexture->Disable();
	}
	else if(state == WIDGET_NORMAL){
		shapeRenderer->SquareVGrad(x,y,w,h,Math::Color4ub(83,83,83),Math::Color4ub(55,55,55));
		fontTexture->Enable();
		device->Color(255,255,255,255);
		fontMap->Draw(x+w/2,y+h/2,16,text,false,FONT_ALIGNMENT_MIDDLE);
		fontTexture->Disable();
	}
	else if(state == WIDGET_HIGHLIGHTED){
		shapeRenderer->SquareVGrad(x,y,w,h,Math::Color4ub(98,98,98),Math::Color4ub(70,70,70));
		fontTexture->Enable();
		device->Color(255,255,255,255);
		fontMap->Draw( x + w/2, y + h/2, 16, text, false, FONT_ALIGNMENT_MIDDLE);
		fontTexture->Disable();
	}
}

void BlenderWidgetRenderer::Tooltip(float x,float y,const std::string & text){
	shapeRenderer->SetRoundMode(ShapeRenderer::ROUND_ALL);
	shapeRenderer->SetRoundRadius(5);

	float width = fontMap->GetLength(16,text,false);

	float w = width + 20;
	float h = 20;

	shapeRenderer->Square(x-4,y-4,w+8,h+8,Math::Color4ub(0,0,0,10));
	shapeRenderer->Square(x-3,y-3,w+6,h+6,Math::Color4ub(0,0,0,10));
	shapeRenderer->Square(x-2,y-2,w+4,h+4,Math::Color4ub(0,0,0,10));

	shapeRenderer->Square(x-1,y-1,w+2,h+2,Math::Color4ub(50,50,50,200));
	shapeRenderer->Square(x,y,w,h,Math::Color4ub(0,0,0,155));

	fontTexture->Enable();
	device->Color(255,255,255,255);
	fontMap->Draw(x+10,y+h*0.5+2, 16, text);
	fontTexture->Disable();
}

void BlenderWidgetRenderer::CheckBoxButton(float x, float y, float w, float h,
		const std::string & text,bool checked,
		WidgetState state, ShapeRenderer::RoundMode mode)
{
	shapeRenderer->SetRoundMode(mode);
	shapeRenderer->SetRoundRadius(3);

	shapeRenderer->Square(x+2,y+2,w,h,Math::Color4ub(135,135,135));
	shapeRenderer->Square(x-1,y-1,w+2,h+2,Math::Color4ub(39,39,39));

	if(state == WIDGET_NORMAL){
		shapeRenderer->SquareVGrad(x,y,w,h,Math::Color4ub(83,83,83),Math::Color4ub(55,55,55));
	}
	else if(state == WIDGET_PRESSED){
		shapeRenderer->SquareVGrad(x,y,w,h,Math::Color4ub(85,85,85),Math::Color4ub(113,113,113));
	}
	else if(state == WIDGET_HIGHLIGHTED){
		shapeRenderer->SquareVGrad(x,y,w,h,Math::Color4ub(98,98,98),Math::Color4ub(70,70,70));
	}

	if(checked){
		shapeRenderer->CheckSymbol(x+2,y-1,w+2,Math::Color4ub(255,255,255));
	}

	fontTexture->Enable();
	device->Color(0,0,0,255);
	fontMap->Draw(x+w+6,y+h/2,16,text,false,FONT_ALIGNMENT_LEFT);
	fontTexture->Disable();

}

void BlenderWidgetRenderer::Label(float x,float y, const std::string & text,float size,bool bold,
		const Math::Color4ub & color,FontAlignment align){
	fontTexture->Enable();
	device->Color(color.GetR(),color.GetG(),color.GetB(),color.GetA());
	fontMap->Draw(x*shapeRenderer->GetDensity(),y*shapeRenderer->GetDensity(),size*shapeRenderer->GetDensity(),text,bold,align);
	fontTexture->Disable();
}

void BlenderWidgetRenderer::PushTranslation(float x,float y){
	device->PushMatrix();
	device->Translate(x*shapeRenderer->GetDensity(),y*shapeRenderer->GetDensity(),0);
}

void BlenderWidgetRenderer::PopTranslation(){
	device->PopMatrix();
}


_GUI_END
