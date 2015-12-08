
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckShapeRenderer.h"
#include "nckUtils.h"
#include <string.h>

// Bit Twiddling Hacks
unsigned int NextPowerOfTwo(unsigned int v){
	v--;
	v |= v>>1;
	v |= v>>2;
	v |= v>>4;
	v |= v>>8;
	v |= v>>16;
	v++;
	return v;
}

_GUI_BEGIN

void ShapeRenderer::SetRoundMode(RoundMode mode){
	roundMode = mode;
}

ShapeRenderer::RoundMode ShapeRenderer::GetRoundMode(){
	return roundMode;
}

void ShapeRenderer::SetRoundRadius(float radius){
	roundRadius = radius;
}

float ShapeRenderer::GetRoundRadius(){
	return roundRadius;
}

Graph::Texture2D* ShapeRenderer::UpdateCheckCache(int size)
{
	Graph::Texture2D* tTexture = NULL;

	int tsize = NextPowerOfTwo(((int)(size/5.0))*10);
	if(tsize<8) tsize = 8;

	if(cbTextureCache.find(tsize)==cbTextureCache.end())
	{
		
		Graph::Texture2D * tex = dynamic_cast<Graph::Texture2D *>(
				device->CreateTexture(Graph::TEXTURE_2D,tsize,tsize,0,Graph::FORMAT_RGBA_8B,false)
				);
		Core::Image * img = Core::Image::GenerateCheck(tsize,Core::PIXEL_FORMAT_RGBA_8B);
		void * texData = tex->Lock();
		memcpy(texData,img->GetData(),4*tsize*tsize);
		tex->Unlock();
		SafeDelete(img);

		tTexture = tex;

		cbTextureCache.insert(std::pair<int,Graph::Texture2D*>(tsize,tex));
	}
	else
		tTexture = cbTextureCache.find(tsize)->second;

	return tTexture;
}

void ShapeRenderer::CheckSymbol(float x,float y,float size,const Math::Color4ub & col){
	Graph::Texture2D * tex = UpdateCheckCache(size);
	tex->Enable();
	device->Begin(Graph::PRIMITIVE_QUADS);

	device->Color(col.GetR(),col.GetG(),col.GetB(),col.GetA());

	device->TexCoord(0,0);
	device->Vertex(x,y);
	device->TexCoord(0,1);
	device->Vertex(x,y+size);
	device->TexCoord(1,1);
	device->Vertex(x+size,y+size);
	device->TexCoord(1,0);
	device->Vertex(x+size,y);
	device->End();

	tex->Disable();
}


void ShapeRenderer::CircleShape(float x,float y,float size,const Math::Color4ub & col){
	Graph::Texture2D * tex = UpdateCircleCache(size);
	tex->Enable();
	device->Begin(Graph::PRIMITIVE_QUADS);

	device->Color(col.GetR(),col.GetG(),col.GetB(),col.GetA());

	device->TexCoord(0,0);
	device->Vertex(x,y);
	device->TexCoord(0,1);
	device->Vertex(x,y+size);
	device->TexCoord(1,1);
	device->Vertex(x+size,y+size);
	device->TexCoord(1,0);
	device->Vertex(x+size,y);
	device->End();

	tex->Disable();
}

Graph::Texture2D* ShapeRenderer::UpdateCircleCache(int size)
{
	Graph::Texture2D* tTexture = NULL;

	int tsize = NextPowerOfTwo(((int)(size/5.0))*10);
	if(tsize<8) tsize = 8;
	if(rsTextureCache.find(tsize)==rsTextureCache.end())
	{
		Graph::Texture2D * tex = dynamic_cast<Graph::Texture2D *>(
				device->CreateTexture(Graph::TEXTURE_2D,tsize,tsize,0,Graph::FORMAT_RGBA_8B,false)
				);
		//int ws = tex->GetWidth();
		//int hs = tex->GetHeight();
		Core::Image * img = Core::Image::GenerateCircle(tsize,false,Core::PIXEL_FORMAT_RGBA_8B);
		void * texData = tex->Lock();
		memcpy(texData,img->GetData(),4*tsize*tsize);
		tex->Unlock();
		SafeDelete(img);

		tTexture = tex;

		rsTextureCache.insert(std::pair<int,Graph::Texture2D*>(tsize,tex));
	}
	else
		tTexture = rsTextureCache.find(tsize)->second;

	return tTexture;
}

ShapeRenderer::ShapeRenderer(Graph::Device * dev){
	device = dev;
	roundMode = ROUND_NONE;
	
	rsProgram[ROUND_ALL] = device->LoadProgram("shader://gui/roundall.cpp");
	rsProgram[ROUND_TOP] = device->LoadProgram("shader://gui/roundtop.cpp");
	rsProgram[ROUND_LEFT] = device->LoadProgram("shader://gui/roundleft.cpp");
	rsProgram[ROUND_RIGHT] = device->LoadProgram("shader://gui/roundright.cpp");
	rsProgram[ROUND_BOTTOM] = device->LoadProgram("shader://gui/roundbottom.cpp");
}

ShapeRenderer::~ShapeRenderer(){
	for(int i = 0;i<5;i++)
		SafeDelete(rsProgram[i]);

	for(std::map<int,Graph::Texture2D*>::iterator i = rsTextureCache.begin();i!=rsTextureCache.end();i++){
		delete (*i).second;
	}

	for(std::map<int,Graph::Texture2D*>::iterator i = cbTextureCache.begin();i!=cbTextureCache.end();i++){
		delete (*i).second;
	}
}

void ShapeRenderer::Square(float x,float y,float w, float h, const Math::Color4ub & col1){
	if(roundMode==ROUND_NONE){
		device->Color(col1.GetR(),col1.GetG(),col1.GetB(),col1.GetA());
		device->Begin(Graph::PRIMITIVE_QUADS);
		device->TexCoord(0,0);
		device->Vertex(x,y);
		device->TexCoord(0,1);
		device->Vertex(x,y+h);
		device->TexCoord(1,1);
		device->Vertex(x+w,y+h);
		device->TexCoord(1,0);
		device->Vertex(x+w,y);
		device->End();
	}
	else{
		Graph::Texture2D * tex = UpdateCircleCache(roundRadius);
		tex->Enable();
		rsProgram[roundMode]->Enable();
		rsProgram[roundMode]->SetVariable4f("properties",w,h,roundRadius,0);
		rsProgram[roundMode]->SetVariable1i("texture_0",0);
		device->Color(col1.GetR(),col1.GetG(),col1.GetB(),col1.GetA());
		device->Begin(Graph::PRIMITIVE_QUADS);
		device->TexCoord(0,0);
		device->Vertex(x,y);
		device->TexCoord(0,1);
		device->Vertex(x,y+h);
		device->TexCoord(1,1);
		device->Vertex(x+w,y+h);
		device->TexCoord(1,0);
		device->Vertex(x+w,y);
		device->End();
		rsProgram[roundMode]->Disable();
		tex->Disable();
	}
}

void ShapeRenderer::SquareVGrad(float x,float y,float w, float h,const Math::Color4ub & col1,const Math::Color4ub & col2){
	if(roundMode == ROUND_NONE){
	device->Begin(Graph::PRIMITIVE_QUADS);
	device->Color(col1.GetR(),col1.GetG(),col1.GetB(),col1.GetA());
	device->TexCoord(0,0);
	device->Vertex(x,y);
	device->Color(col2.GetR(),col2.GetG(),col2.GetB(),col2.GetA());
	device->TexCoord(0,1);
	device->Vertex(x,y+h);
	device->TexCoord(1,1);
	device->Vertex(x+w,y+h);
	device->Color(col1.GetR(),col1.GetG(),col1.GetB(),col1.GetA());
	device->TexCoord(1,0);
	device->Vertex(x+w,y);
	device->End();
	}
	else{
		Graph::Texture2D * tex = UpdateCircleCache(roundRadius);
		tex->Enable();
		rsProgram[roundMode]->Enable();
		rsProgram[roundMode]->SetVariable4f("properties",w,h,roundRadius,0);
		rsProgram[roundMode]->SetVariable1i("texture_0",0);
		device->Begin(Graph::PRIMITIVE_QUADS);
		device->Color(col1.GetR(),col1.GetG(),col1.GetB(),col1.GetA());
		device->TexCoord(0,0);
		device->Vertex(x,y);
		device->Color(col2.GetR(),col2.GetG(),col2.GetB(),col2.GetA());
		device->TexCoord(0,1);
		device->Vertex(x,y+h);
		device->TexCoord(1,1);
		device->Vertex(x+w,y+h);
		device->Color(col1.GetR(),col1.GetG(),col1.GetB(),col1.GetA());
		device->TexCoord(1,0);
		device->Vertex(x+w,y);
		device->End();
		rsProgram[roundMode]->Disable();
		tex->Disable();
	}
}

void ShapeRenderer::SquareHGrad(float x,float y,float w, float h,const Math::Color4ub & col1,const Math::Color4ub & col2){
	if(roundMode == ROUND_NONE){
		device->Begin(Graph::PRIMITIVE_QUADS);
		device->Color(col1.GetR(),col1.GetG(),col1.GetB(),col1.GetA());
		device->TexCoord(0,0);
		device->Vertex(x,y);
		device->TexCoord(0,1);
		device->Vertex(x,y+h);
		device->Color(col2.GetR(),col2.GetG(),col1.GetB(),col2.GetA());
		device->TexCoord(1,1);
		device->Vertex(x+w,y+h);
		device->TexCoord(1,0);
		device->Vertex(x+w,y);
		device->End();
	}
	else{
		Graph::Texture2D * tex = UpdateCircleCache(roundRadius);
		tex->Enable();
		rsProgram[roundMode]->Enable();
		rsProgram[roundMode]->SetVariable4f("properties",w,h,roundRadius,0);
		rsProgram[roundMode]->SetVariable1i("texture_0",0);
		device->Begin(Graph::PRIMITIVE_QUADS);
		device->Color(col1.GetR(),col1.GetG(),col1.GetB(),col1.GetA());
		device->TexCoord(0,0);
		device->Vertex(x,y);
		device->TexCoord(0,1);
		device->Vertex(x,y+h);
		device->Color(col2.GetR(),col2.GetG(),col1.GetB(),col2.GetA());
		device->TexCoord(1,1);
		device->Vertex(x+w,y+h);
		device->TexCoord(1,0);
		device->Vertex(x+w,y);
		device->End();
		rsProgram[roundMode]->Disable();
		tex->Disable();
	}
}


void ShapeRenderer::Triangle(float x,float y,float w,float h, const Math::Color4ub &col, bool side)
{
	device->Color(col.GetR(),col.GetG(),col.GetB(),col.GetA() );
	device->Begin(Graph::PRIMITIVE_TRIANGLES);
	if(side){
		device->Vertex(x,y);
		device->Vertex(x,y+h);
		device->Vertex(x+w,(2*y+h)/2);
	}
	else
	{
		device->Vertex(x+w,y);
		device->Vertex(x+w,y+h);
		device->Vertex(x,(2*y+h)/2);
	}
	device->End();
}

_GUI_END
