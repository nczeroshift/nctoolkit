
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo.h"

#include "demos/nckDemo_Triangles.h"
#include "demos/nckDemo_TextureNPT.h"
#include "demos/nckDemo_TextureWrapping.h"
#include "demos/nckDemo_TextureFiltering.h"
#include "demos/nckDemo_TextureCubemap.h"
#include "demos/nckDemo_Texture3D.h"
#include "demos/nckDemo_Texture2D.h"
#include "demos/nckDemo_Shadows.h"
#include "demos/nckDemo_ShaderProgram.h"
#include "demos/nckDemo_Quadtree.h"
#include "demos/nckDemo_OcclusionQuery.h"
#include "demos/nckDemo_MultiCamera.h"
#include "demos/nckDemo_Model.h"
#include "demos/nckDemo_Material.h"
#include "demos/nckDemo_HttpStream.h"
#include "demos/nckDemo_HttpServer.h"
#include "demos/nckDemo_FBO.h"
#include "demos/nckDemo_CubemapFBO.h"
#include "demos/nckDemo_Compound.h"
#include "demos/nckDemo_Bumpmap.h"
#include "demos/nckDemo_AudioOut.h"
#include "demos/nckDemo_AudioOgg.h"
#include "demos/nckDemo_AudioFFT.h"
#include "demos/nckDemo_Armature.h"
#include "demos/nckDemo_Animation.h"
#include "demos/nckDemo_Webcam.h"
#include "demos/nckDemo_Particles.h"
#include "demos/nckDemo_GUI.h"
#include "demos/nckDemo_Curves.h"
#include "demos/nckDemo_Metaballs.h"
#include "demos/nckDemo_Serial.h"
#include "demos/nckDemo_Frustum.h"
#include "demos/nckDemo_DepthOfField.h"
#include "demos/nckDemo_SSAO.h"
#include "demos/nckDemo_ShadowsProjector.h"
#include "demos/nckDemo_FrustumVolume.h"
#include "demos/nckDemo_Texture_Diff.h"

int DemoCount() {
    return 37;
}

Demo * DemoCreate(int i, Core::Window * wnd, Graph::Device * dev) {
    Demo * (*demos[])(Core::Window*, Graph::Device*) = {
		CreateDemo_Triangles,
        CreateDemo_Texture2D,
        CreateDemo_TextureNPT,
        CreateDemo_TextureCubemap,
        CreateDemo_TextureFiltering,
        CreateDemo_TextureWrapping,
        CreateDemo_GUI,
        CreateDemo_AudioOut,
        CreateDemo_AudioOgg,
        CreateDemo_AudioFFT,
        CreateDemo_Compound,
        CreateDemo_Metaballs,
        CreateDemo_Curves,
        CreateDemo_ShaderProgram,
        CreateDemo_Model,
        CreateDemo_Material,
        CreateDemo_Quadtree,
        CreateDemo_Texture3D,
        CreateDemo_FBO,
        CreateDemo_CubemapFBO,
        CreateDemo_Shadows,
        CreateDemo_Particles,
        CreateDemo_HttpStream,
        CreateDemo_HttpServer,
        CreateDemo_OcclusionQuery,
        CreateDemo_MultiCamera,
        CreateDemo_Bumpmap,
        CreateDemo_Armature,
        CreateDemo_Animation,
        CreateDemo_Webcam,
        CreateDemo_Serial,
        CreateDemo_Frustum,
        CreateDemo_DepthOfField,
        CreateDemo_SSAO,
		CreateDemo_ShadowsProjector,
		CreateDemo_FrustumVolume,
		CreateDemo_Texture_Diff
    };
    
    if (i < DemoCount())
        return demos[i](wnd, dev);

    return NULL;
}



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
