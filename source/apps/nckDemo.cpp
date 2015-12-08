
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo.h"

void Demo::RenderLine(const Math::Line & line){
	dev->Begin(Graph::PRIMITIVE_LINES);
	Math::Vec3 v1 = line.GetStart();
	Math::Vec3 v2 = line.GetEnd();
	dev->Vertex(v1.GetX(),v1.GetY(),v1.GetZ());
	dev->Vertex(v2.GetX(),v2.GetY(),v2.GetZ());
	dev->End();
}

void Demo::RenderDot(Math::Vec3 p,float size){
	dev->Begin(Graph::PRIMITIVE_QUADS);
	dev->Vertex(p.GetX()-size,p.GetY()+size,p.GetZ());
	dev->Vertex(p.GetX()-size,p.GetY()-size,p.GetZ());
	dev->Vertex(p.GetX()+size,p.GetY()-size,p.GetZ());
	dev->Vertex(p.GetX()+size,p.GetY()+size,p.GetZ());
	dev->End();
}

void Demo::RenderSquare2D(float x,float y,float w, float h, const Math::Color4ub & col){
	dev->Color(col.GetR(),col.GetG(),col.GetB(),col.GetA());
	dev->Begin(Graph::PRIMITIVE_QUADS);
	dev->TexCoord(0,0);
	dev->Vertex(x,y);
	dev->TexCoord(0,1);
	dev->Vertex(x,y+h);
	dev->TexCoord(1,1);
	dev->Vertex(x+w,y+h);
	dev->TexCoord(1,0);
	dev->Vertex(x+w,y);
	dev->End();
}