
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckGraphics_dx9.h"
#include "nckException.h"
#include "nckShaderResources_dx9.h"
#include "nckUtils.h"
#include "nckDataIO.h"
#include "../nckShaderParser.h"

_GRAPHICS_BEGIN

#if defined(NCK_DIRECTX9)

D3DFORMAT ConvertFormat_DX9(Format source)
{
	switch(source)
	{
		case FORMAT_RGB_8B:	return D3DFMT_R8G8B8;
		case FORMAT_RGBA_8B:	return D3DFMT_A8R8G8B8;
		case FORMAT_RGBA_16F:	return D3DFMT_A16B16G16R16F;
		case FORMAT_RGBA_32F:	return D3DFMT_A32B32G32R32F;
		case FORMAT_A_8B:	return D3DFMT_A8;
		case FORMAT_R_16F:	return D3DFMT_R16F;
		case FORMAT_R_32F:	return D3DFMT_R32F;
	};

	return D3DFMT_A8B8G8R8;
}

Device_DX9::Device_DX9()
{
	m_ActiveProgram = NULL;
	m_CullFace_Mode = CULL_BACK;
	m_CullFace_Flag = false;
	m_CullFace_Flip = false;

	m_ClipPlaneState = 0;
	for(int i = 0;i<6;i++){
		m_ClipPlanes[i][0] = D3DXPLANE(0,0,1,0);
		m_ClipPlanes[i][1] = D3DXPLANE(0,0,1,0);
		m_ClipPlanes[i][2] = D3DXPLANE(0,0,1,0);
		m_ClipPlaneState = 0;
	}

	m_MatrixMode = D3DTS_PROJECTION;
	m_AntiAliasingAmount = 0;
	m_Height = 480;
	m_Width = 640;
	m_D3Ddevice = NULL;
	m_D3D = NULL;
	m_MatrixStack.clear();
	m_ClearFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;

	m_SceneStatus = false;

	m_RenderSurface = NULL;
	m_DSSurface = NULL;

	m_DiffuseColor = D3DXCOLOR(1,1,1,1);
	m_SpecularColor = D3DXCOLOR(1,1,1,1);
	m_AmbientColor = D3DXCOLOR(0,0,0,0);
	m_SpecularPower = 36.0f;
}	

Device_DX9::~Device_DX9()
{
	m_MatrixStack.clear();
	SafeRelease(m_RenderSurface);
	SafeRelease(m_DSSurface);

	SafeRelease(m_D3Ddevice);
	SafeRelease(m_D3D);
}

void Device_DX9::Viewport(unsigned int x,unsigned int y, unsigned int w,unsigned int h){
	D3DVIEWPORT9 vp;
	vp.X = (DWORD)x;
	vp.Y = (DWORD)y;
	vp.Width = (DWORD)w;
	vp.Height = (DWORD)h;
	vp.MinZ = 0.0;
	vp.MaxZ = 1.0;
	m_D3Ddevice->SetViewport(&vp);
}

void Device_DX9::Clear()
{
	m_D3Ddevice->Clear( 0, NULL,m_ClearFlags,
							D3DCOLOR_COLORVALUE(m_ClearColor[0], 
												m_ClearColor[1], 
												m_ClearColor[2], 
												m_ClearColor[3]), 1.0f, 0 );
	m_MatrixStack.clear();
	m_MatrixMode = D3DTS_PROJECTION;
	m_TextureCache.Clear();
	for(int i = 0;i<8;i++)
		m_D3Ddevice->SetTexture( 0,NULL);

	RestartScene();
}

void Device_DX9::ClearFlags(unsigned int flags)
{
	m_ClearFlags = 0;
	if(flags & BUFFER_DEPTH_BIT)
		m_ClearFlags|=D3DCLEAR_ZBUFFER;
	if(flags & BUFFER_COLOR_BIT)
		m_ClearFlags|=D3DCLEAR_TARGET;
	if(flags & BUFFER_STENCIL_BIT)	
		m_ClearFlags|=D3DCLEAR_STENCIL;
}

void Device_DX9::ClearColor(float r,float g,float b,float a)
{
	m_ClearColor[0] = r;
	m_ClearColor[1] = g;
	m_ClearColor[2] = b;
	m_ClearColor[3] = a;
}

void Device_DX9::PresentAll(){
	FinalizeScene();

	HRESULT hr = m_D3Ddevice->Present( NULL, NULL, NULL, NULL );

	if(hr == D3DERR_DEVICELOST){
		THROW_EXCEPTION("Main context of rendering was lost, due to some screen swaping event, please restart the application.");
		//MessageBox(NULL,"Main context of rendering was lost, due to some screen swaping event, please restart the application.","NCtoolkit error in DirectX 9",MB_OK|MB_ICONERROR);
		//	exit(EXIT_FAILURE);
	}
}

void Device_DX9::Begin(PrimitiveType type)
{
	m_ActiveVertex = 0;
	m_PrimitiveType = type;
}

void Device_DX9::End()
{
	m_ActiveVertex = 0;
}

void Device_DX9::Vertex(float x,float y,float z)
{
	m_VertexBuffer[m_ActiveVertex] = m_VertexTemp;

	m_VertexBuffer[m_ActiveVertex].m_Loc[0] = x;
	m_VertexBuffer[m_ActiveVertex].m_Loc[1] = y;
	m_VertexBuffer[m_ActiveVertex].m_Loc[2] = z;
	m_ActiveVertex++;
	
	
	if(m_ActiveVertex == 3 && m_PrimitiveType == PRIMITIVE_TRIANGLES){
		if(m_ActiveProgram)	((Program_DX9*)m_ActiveProgram)->Enable_Internal();
		UpdateClipPlanes();
		m_TextureCache.Check();
		m_D3Ddevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE|D3DFVF_NORMAL  | D3DFVF_TEX1);
		m_D3Ddevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,1,m_VertexBuffer,sizeof(BasicVertex));
		m_D3Ddevice->SetFVF(NULL);
		m_ActiveVertex = 0;
	}
	else if(m_ActiveVertex == 4 && m_PrimitiveType == PRIMITIVE_QUADS){
		UpdateClipPlanes();
		if(m_ActiveProgram)	((Program_DX9*)m_ActiveProgram)->Enable_Internal();
		m_TextureCache.Check();
		m_D3Ddevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE|D3DFVF_NORMAL  | D3DFVF_TEX1);
		m_D3Ddevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,m_VertexBuffer,sizeof(BasicVertex));
		m_D3Ddevice->SetFVF(NULL);
		m_ActiveVertex = 0;
	}
	else if(m_ActiveVertex == 2 && m_PrimitiveType == PRIMITIVE_LINES){
		UpdateClipPlanes();
		if(m_ActiveProgram)	((Program_DX9*)m_ActiveProgram)->Enable_Internal();
		m_TextureCache.Check();
		m_D3Ddevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE|D3DFVF_NORMAL  | D3DFVF_TEX1);
		m_D3Ddevice->DrawPrimitiveUP(D3DPT_LINELIST,1,m_VertexBuffer,sizeof(BasicVertex));
		m_D3Ddevice->SetFVF(NULL);
		m_ActiveVertex = 0;
	}
	else if(m_ActiveVertex == 1 && m_PrimitiveType == PRIMITIVE_POINTS){
		UpdateClipPlanes();
		if(m_ActiveProgram)	((Program_DX9*)m_ActiveProgram)->Enable_Internal();
		m_TextureCache.Check();
		m_D3Ddevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE|D3DFVF_NORMAL  | D3DFVF_TEX1);
		m_D3Ddevice->DrawPrimitiveUP(D3DPT_POINTLIST,1,m_VertexBuffer,sizeof(BasicVertex));
		m_D3Ddevice->SetFVF(NULL);
		m_ActiveVertex = 0;
	}
	
	
}
void Device_DX9::TexCoord(float x,float y)
{
	m_VertexTemp.m_Tex[0] = x;
	m_VertexTemp.m_Tex[1] = y;
}

void Device_DX9::Color(unsigned char r,unsigned char g,
	unsigned char b,unsigned char a)
{
	m_VertexTemp.m_Color[0] = b;
	m_VertexTemp.m_Color[1] = g;
	m_VertexTemp.m_Color[2] = r;
	m_VertexTemp.m_Color[3] = a;
}

void Device_DX9::Normal(float x,float y,float z)
{
	m_VertexTemp.m_Nor[0] = x;
	m_VertexTemp.m_Nor[1] = y;
	m_VertexTemp.m_Nor[2] = z;
}



void Device_DX9::Enable(RenderState flag)
{
	switch(flag)
	{
		case STATE_BLEND:
		{
			m_D3Ddevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			m_D3Ddevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
			for(int i = 0;i<8;i++){
				m_D3Ddevice->SetTextureStageState(i,D3DTSS_COLOROP,D3DTOP_MODULATE);
				m_D3Ddevice->SetTextureStageState(i,D3DTSS_COLORARG1,D3DTA_TEXTURE);
				m_D3Ddevice->SetTextureStageState(i,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
				m_D3Ddevice->SetTextureStageState(i,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
				m_D3Ddevice->SetTextureStageState(i,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
				m_D3Ddevice->SetTextureStageState(i,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);
			}
			return;	
		};

		case STATE_DEPTH_TEST:
			m_D3Ddevice->SetRenderState( D3DRS_ZENABLE, TRUE );
			return;

		case STATE_ALPHA_TEST:
			m_D3Ddevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
			m_D3Ddevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			m_D3Ddevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
			m_D3Ddevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
			return;

		case STATE_POINT_SPRITES:
			m_D3Ddevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
			return;

		case STATE_POINT_SPRITES_SCALE:
			m_D3Ddevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
			m_D3Ddevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(1.0f));  
			m_D3Ddevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(10.0f) );  
			m_D3Ddevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(100.0f)); 
			return;
	
		case STATE_CULL_FACE:
			m_CullFace_Flag = true;
			UpdateCullMode();
			return;

		case STATE_FLIP_CULL_FACE:
			m_CullFace_Flip = true;
			UpdateCullMode();
			return;
	}
}

void Device_DX9::Enable(ClipPlaneName flag){
	switch(flag)
	{
	case CLIP_PLANE_0:
		m_ClipPlaneState|=D3DCLIPPLANE0;
		break;
	case CLIP_PLANE_1:
		m_ClipPlaneState|=D3DCLIPPLANE1;
		break;
	case CLIP_PLANE_2:
		m_ClipPlaneState|=D3DCLIPPLANE2;
		break;
	case CLIP_PLANE_3:
		m_ClipPlaneState|=D3DCLIPPLANE3;
		break;
	case CLIP_PLANE_4:
		m_ClipPlaneState|=D3DCLIPPLANE4;
		break;
	case CLIP_PLANE_5:
		m_ClipPlaneState|=D3DCLIPPLANE5;
		break;
	}

	m_D3Ddevice->SetRenderState( D3DRS_CLIPPLANEENABLE, m_ClipPlaneState);
}

void Device_DX9::Disable(RenderState flag)
{
	switch(flag)
	{
		case STATE_BLEND:
			m_D3Ddevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			return;	

		case STATE_DEPTH_TEST:
			m_D3Ddevice->SetRenderState( D3DRS_ZENABLE, FALSE );
			return;

		case STATE_ALPHA_TEST:
			m_D3Ddevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE); 
			return;

		case STATE_POINT_SPRITES:
			m_D3Ddevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
			return;

		case STATE_POINT_SPRITES_SCALE:
			m_D3Ddevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
			m_D3Ddevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.0f) );
			m_D3Ddevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.0f) ); 
			m_D3Ddevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(0.0f) );
			return;

		case STATE_CULL_FACE:
			m_CullFace_Flag = false;
			UpdateCullMode();
			return;

		case STATE_FLIP_CULL_FACE:
			m_CullFace_Flip = false;
			UpdateCullMode();
			return;
	}
}

void Device_DX9::Disable(ClipPlaneName flag){
	switch(flag)
	{
	case CLIP_PLANE_0:
		m_ClipPlaneState-= m_ClipPlaneState & D3DCLIPPLANE0;
		break;
	case CLIP_PLANE_1:
		m_ClipPlaneState-= m_ClipPlaneState & D3DCLIPPLANE1;
		break;
	case CLIP_PLANE_2:
		m_ClipPlaneState-= m_ClipPlaneState & D3DCLIPPLANE2;
		break;
	case CLIP_PLANE_3:
		m_ClipPlaneState-= m_ClipPlaneState & D3DCLIPPLANE3;		
		break;
	case CLIP_PLANE_4:
		m_ClipPlaneState-= m_ClipPlaneState & D3DCLIPPLANE4;		
		break;
	case CLIP_PLANE_5:
		m_ClipPlaneState-= m_ClipPlaneState & D3DCLIPPLANE5;	
		break;
	}
	m_D3Ddevice->SetRenderState( D3DRS_CLIPPLANEENABLE, m_ClipPlaneState);
}


D3DBLEND Device_DX9::ConvertFromBlendMode(BlendMode blend)
{
	switch(blend)
	{
		case BLEND_ZERO:
			return D3DBLEND_ZERO;
		case BLEND_ONE:
			return D3DBLEND_ONE;
		case BLEND_SRC_COLOR:
			return D3DBLEND_SRCCOLOR;
		case BLEND_INV_SRC_COLOR:
			return D3DBLEND_INVSRCCOLOR;

		case BLEND_DST_COLOR:
			return D3DBLEND_DESTCOLOR;
		case BLEND_INV_DST_COLOR:
			return D3DBLEND_INVDESTCOLOR;

		case BLEND_SRC_ALPHA:
			return D3DBLEND_SRCALPHA;
		case BLEND_INV_SRC_ALPHA:
			return D3DBLEND_INVSRCALPHA;
		case BLEND_DST_ALPHA:
			return D3DBLEND_DESTALPHA;
		case BLEND_INV_DST_ALPHA:
			return D3DBLEND_INVDESTALPHA;
	}
	return D3DBLEND_ZERO;
}

void Device_DX9::BlendFunc(BlendMode sfactor,BlendMode dfactor)
{
	m_D3Ddevice->SetRenderState	(D3DRS_SRCBLEND,ConvertFromBlendMode(sfactor));
	m_D3Ddevice->SetRenderState	(D3DRS_DESTBLEND,ConvertFromBlendMode(dfactor));
}

void Device_DX9::AlphaFunc(TestFlag func,float value){
	m_D3Ddevice->SetRenderState(D3DRS_ALPHAFUNC,func);
	m_D3Ddevice->SetRenderState(D3DRS_ALPHAREF, (DWORD) (0x00000000 + value *255));
}

void Device_DX9::StencilFunc(TestFlag func,unsigned int ref, 
	unsigned int mask){

}

void Device_DX9::StencilOp(OperationMode sfail,OperationMode dpmode, 
	OperationMode dppass){

}

void Device_DX9::UpdateCullMode()
{
	if(!m_CullFace_Flag){
		m_D3Ddevice->SetRenderState( D3DRS_CULLMODE ,D3DCULL_NONE);
		return;
	}

	if(m_CullFace_Flip)
	{
		if(m_CullFace_Mode == CULL_FRONT)
			m_D3Ddevice->SetRenderState( D3DRS_CULLMODE ,D3DCULL_CCW);
		else if(m_CullFace_Mode == CULL_BACK)
			m_D3Ddevice->SetRenderState( D3DRS_CULLMODE ,D3DCULL_CW );
	}
	else
	{
		if(m_CullFace_Mode == CULL_BACK)
			m_D3Ddevice->SetRenderState( D3DRS_CULLMODE ,D3DCULL_CW);
		else if(m_CullFace_Mode == CULL_FRONT)
			m_D3Ddevice->SetRenderState( D3DRS_CULLMODE ,D3DCULL_CCW );
	}

}

void Device_DX9::CullMode(CullFaceMode mode)
{
	m_CullFace_Mode = mode;
	UpdateCullMode();
}

void Device_DX9::FillMode(PolygonMode mode){
	if(mode == FILL_WIREFRAME)
		m_D3Ddevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME );
	else if(mode == FILL_SOLID)
		m_D3Ddevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID );
}

void Device_DX9::DepthBias(float scale,float offset){
	m_D3Ddevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS ,*((DWORD*)&scale));
	m_D3Ddevice->SetRenderState(D3DRS_DEPTHBIAS,*((DWORD*)&offset));
}

void Device_DX9::MatrixMode(MatrixType mode){
	if(mode == MATRIX_PROJECTION)
		m_MatrixMode = D3DTS_PROJECTION;
	else if(mode == MATRIX_VIEW)
		m_MatrixMode = D3DTS_VIEW;
	else if(mode == MATRIX_MODEL)
		m_MatrixMode = D3DTS_WORLD;
}

void Device_DX9::LoadMatrix(const float *v)
{
	D3DMATRIX mat;
	
	mat._11 = v[0];
	mat._12 = v[1];
	mat._13 = v[2];
	mat._14 = v[3];
	mat._21 = v[4];
	mat._22 = v[5];
	mat._23 = v[6];
	mat._24 = v[7];
	mat._31 = v[8];
	mat._32 = v[9];
	mat._33 = v[10];
	mat._34 = v[11];
	mat._41 = v[12];
	mat._42 = v[13];
	mat._43 = v[14];
	mat._44 = v[15];
	m_D3Ddevice->SetTransform(m_MatrixMode,&mat);

	UpdatePVMatrix();
}

void Device_DX9::MultMatrix(const float *mat){
	D3DXMATRIX t_mat,t2_mat,r_mat;
	m_D3Ddevice->GetTransform(m_MatrixMode,&t_mat);
	memcpy(&t2_mat,mat,sizeof(float)*16);
	D3DXMatrixMultiply(&r_mat,&t2_mat,&t_mat);
	m_D3Ddevice->SetTransform(m_MatrixMode,&r_mat);
	UpdatePVMatrix();
}


void Device_DX9::Identity()
{
	D3DXMATRIX t_mat;
	D3DXMatrixIdentity(&t_mat);
	m_D3Ddevice->SetTransform(m_MatrixMode,&t_mat);
	UpdatePVMatrix();
}

void Device_DX9::PushMatrix(){
	D3DXMATRIX t_mat;
	m_D3Ddevice->GetTransform(m_MatrixMode,&t_mat);
	m_MatrixStack.push_front(t_mat);
	UpdatePVMatrix();
}

void Device_DX9::PopMatrix()
{
	D3DXMATRIX t_mat;
	t_mat = *m_MatrixStack.begin();
	m_D3Ddevice->SetTransform(m_MatrixMode,&t_mat);
	m_MatrixStack.pop_front();
	UpdatePVMatrix();
}

void Device_DX9::Translate(float x,float y,float z){
	D3DXMATRIX  tmp_mat;
	D3DXMatrixTranslation(&tmp_mat,x,y,z);
	MultMatrix((float*)&tmp_mat);
	UpdatePVMatrix();
}

void Device_DX9::Scale(float x,float y,float z){
	D3DMATRIX scale = {
    x,	0.0f,	0.0f,	0.0f,
    0.0f,	y,	0.0f,	0.0f,
    0.0f,	0.0f,	z,	0.0f,
    0.0f,	0.0f,	0.0f,	1.0f
	};
	MultMatrix((float*)&scale);
	UpdatePVMatrix();
}

void Device_DX9::Rotate(float angle,float x,float y,float z){
	/*float c = (float)cos(angle*3.14159f/180.0f);
	float s = (float)sin(angle*3.14159f/180.0f);
	float dist = sqrt(x*x + y*y +z*z);
	x/=dist;
	y/=dist;
	z/=dist;
	D3DMATRIX rotate = {
    x*x*(1-c)+c,	x*y*(1-c)-z*s,	x*z*(1-c)+y*s,	0.0f,
    y*x*(1-c)+z*s,	y*y*(1-c)+c,	y*z*(1-c)-x*s,	0.0f,
    x*z*(1-c)-y*s,	y*z*(1-c)+x*s,	z*z*(1-c)+c,		0.0f,
    0.0f,	0.0f,	0.0f,	1.0f
	};*/
	D3DXVECTOR3 v(x,y,z);
	D3DXMATRIX rotate;
	D3DXMatrixRotationAxis(&rotate,&v,angle*3.14159f/180.0f);
	MultMatrix((float*)&rotate);
	UpdatePVMatrix();
}

void Device_DX9::Quaternion(float x,float y,float z,float w){
	D3DXMATRIX tmp_mat;
	D3DXQUATERNION q = D3DXQUATERNION(x,y,z,w);
	D3DXMatrixRotationQuaternion (&tmp_mat,&q);
	MultMatrix((float*)&tmp_mat);
	UpdatePVMatrix();
}

void Device_DX9::Perspective(float aspect, float fov, float znear,float zfar){
	D3DXMATRIX source;
	m_D3Ddevice->GetTransform(m_MatrixMode,&source);

	D3DXMATRIX temp;
	D3DXMatrixPerspectiveFovRH( &temp, D3DXToRadian( fov ),
										aspect, znear, zfar );
	
	m_Perspective_ZFar = zfar;

	m_D3Ddevice->SetTransform(m_MatrixMode,&(temp*source));
	UpdatePVMatrix();
}

void Device_DX9::Ortho2D(float width,float height,float znear, float zfar){
	D3DVIEWPORT9 vp;
	m_D3Ddevice->GetViewport(&vp);

	float offx = (width/(float)vp.Width)*0.5f;
	float offy = (height/(float)vp.Height)*0.5f;

	D3DXMATRIX source;
	m_D3Ddevice->GetTransform(m_MatrixMode,&source);

	D3DXMATRIX temp;
	D3DXMatrixOrthoRH( &temp, width,-height,znear,zfar);

	m_D3Ddevice->SetTransform(m_MatrixMode,&(temp*source));
	Translate(-width/2-offx,-height/2-offy,0);
	UpdatePVMatrix();
}

void Device_DX9::Ortho(float left, float right, float bottom,float top,float znear,float zfar){
	/*D3DVIEWPORT9 vp;
	m_D3Ddevice->GetViewport(&vp);

	float offx = (width/(float)vp.Width)*0.5f;
	float offy = (height/(float)vp.Height)*0.5f;

	D3DXMATRIX source;
	m_D3Ddevice->GetTransform(m_MatrixMode,&source);

	D3DXMATRIX temp;
	D3DXMatrixOrthoRH( &temp, width,-height,znear,zfar);

	m_D3Ddevice->SetTransform(m_MatrixMode,&(temp*source));
	Translate(-width/2-offx,-height/2-offy,0);
	UpdatePVMatrix();*/
}


void Device_DX9::Material(MaterialProperty prop,const float *v)
{
	switch(prop)
	{
	case MATERIAL_AMBIENT_COLOR:
		m_AmbientColor = D3DXCOLOR(v);return;
	case MATERIAL_DIFFUSE_COLOR:
		m_DiffuseColor = D3DXCOLOR(v);return;
	case MATERIAL_SPECULAR_COLOR:
		m_SpecularColor = D3DXCOLOR(v);return;
	case MATERIAL_SHININESS:
		m_SpecularPower = v[0];
	}
}



void Device_DX9::PointSprite(PointSpriteProperty prop, const float &v)
{
	switch (prop)
	{
	case POINT_MAX_SIZE:
		m_D3Ddevice->SetRenderState( D3DRS_POINTSIZE_MAX,*(DWORD*) &v);
		break;

	case POINT_MIN_SIZE:
		m_D3Ddevice->SetRenderState( D3DRS_POINTSIZE_MIN,*(DWORD*) &v);
		break;

	case POINT_SIZE:
		m_D3Ddevice->SetRenderState( D3DRS_POINTSIZE,*(DWORD*) &v);
		return;
	}
}

void Device_DX9::ClipPlane(ClipPlaneName id, const float *v)
{
	unsigned int p_id = 0;

	switch(id)
	{
		case CLIP_PLANE_0:
			p_id = 0;break;
		case CLIP_PLANE_1:
			p_id = 1;break;
		case CLIP_PLANE_2:
			p_id = 2;break;
		case CLIP_PLANE_3:
			p_id = 3;break;
		case CLIP_PLANE_4:
			p_id = 4;break;
		case CLIP_PLANE_5:
			p_id = 5;break;
	}

	if(v){
		D3DXMATRIX mat_model;
		D3DXMATRIX mat_view;
		D3DXMATRIX mat_proj;

		m_D3Ddevice->GetTransform(D3DTS_WORLD,&mat_model);
		m_D3Ddevice->GetTransform(D3DTS_VIEW,&mat_view);
		m_D3Ddevice->GetTransform(D3DTS_PROJECTION,&mat_proj);

		D3DXPLANE in(v);

		D3DXPLANE res_model;
		D3DXPLANE res_viewprojection;

		D3DXPlaneTransform(&res_model,&in,&mat_model);

		m_ClipPlanes[id][0] = in;
		m_ClipPlanes[id][1] = res_model;

		D3DXMATRIX mat_mv = mat_model*mat_view*mat_proj;

		D3DXMatrixInverse(&mat_mv, NULL, &mat_mv);
		D3DXMatrixTranspose(&mat_mv, &mat_mv);

		D3DXPLANE viewSpacePlane;
		D3DXPlaneTransform(&res_viewprojection, &in, &mat_mv);

		m_ClipPlanes[id][2] = res_viewprojection;
	}
	
}

void Device_DX9::UpdateClipPlanes()
{
	for(int i = 0;i<6;i++){
		if(m_ClipPlaneState & 1<<i)
		{
			if(m_ActiveProgram)
				m_D3Ddevice->SetClipPlane(i,(float*)&m_ClipPlanes[i][2]);
			else
				m_D3Ddevice->SetClipPlane(i,(float*)&m_ClipPlanes[i][1]);

		}
	}
}

void Device_DX9::GetProperty(MatrixType mat,float *v)
{
	switch ( mat )
	{
		case MATRIX_PROJECTION:
		{
			D3DMATRIX *mat = (D3DMATRIX*)v;
			m_D3Ddevice->GetTransform(D3DTS_PROJECTION,mat);
			return;
		}
		
		case MATRIX_VIEW:
		{
			D3DMATRIX *mat = (D3DMATRIX*)v;
			m_D3Ddevice->GetTransform(D3DTS_VIEW ,mat);
			return;
		}
		
		case MATRIX_MODEL:
		{
			D3DMATRIX *mat = (D3DMATRIX*)v;
			m_D3Ddevice->GetTransform(D3DTS_WORLD,mat);
			return;
		}
			
		default:
			return;
	}
}


void Device_DX9::GetProperty(RenderProperties prop,float *v)
{
	switch ( prop )
	{
		case PROPERTY_VIEWPORT:
		{
			D3DVIEWPORT9 vp;
			m_D3Ddevice->GetViewport(&vp);
			v[0] = (float)vp.X;
			v[1] = (float)vp.Y;
			v[2] = (float)vp.Width;
			v[3] = (float)vp.Height;
			return;
		}

		case PROPERTY_MAX_POINT_SIZE:
		{
			D3DCAPS9 caps;
			m_D3Ddevice->GetDeviceCaps(&caps);
			*v = (float)caps.MaxPointSize;
			return;
		}

		case PROPERTY_MAX_TEXTURE_WIDTH:
		{
			D3DCAPS9 caps;
			m_D3Ddevice->GetDeviceCaps(&caps);
			*v = (float)caps.MaxTextureWidth;
			return;
		}

		case PROPERTY_MAX_TEXTURE_HEIGHT:
		{
			D3DCAPS9 caps;
			m_D3Ddevice->GetDeviceCaps(&caps);
			*v = (float)caps.MaxTextureHeight;
			return;
		}

		case PROPERTY_MAX_TEXTURE_SAMPLERS:
		{
			D3DCAPS9 caps;
			m_D3Ddevice->GetDeviceCaps(&caps);
			*v = (float)caps.MaxSimultaneousTextures;
			return;
		}

		case PROPERTY_VERTEX_SHADER_VERSION:
		{
			D3DCAPS9 caps;
			m_D3Ddevice->GetDeviceCaps(&caps);
			*v = (float)caps.VertexShaderVersion;
			return;
		}

		case PROPERTY_FRAGMENT_SHADER_VERSION:
		{
			D3DCAPS9 caps;
			m_D3Ddevice->GetDeviceCaps(&caps);
			*v = (float)caps.PixelShaderVersion;
			return;
		}

		case PROPERTY_MAX_SIMULTANEOUS_RT:
		{
			D3DCAPS9 caps;
			m_D3Ddevice->GetDeviceCaps(&caps);
			*v = (float)caps.NumSimultaneousRTs;
			return;
		}


		default:
			return;
	}
}

void Device_DX9::GetProperty(RenderProperties prop,int *v)
{
	switch ( prop )
	{
		case PROPERTY_VIEWPORT:
		{
			D3DVIEWPORT9 vp;
			m_D3Ddevice->GetViewport(&vp);
			v[0] = (int)vp.X;
			v[1] = (int)vp.Y;
			v[2] = (int)vp.Width;
			v[3] = (int)vp.Height;
			return;
		}

		case PROPERTY_MAX_POINT_SIZE:
		{
			D3DCAPS9 caps;
			m_D3Ddevice->GetDeviceCaps(&caps);
			*v = (int)caps.MaxPointSize;
			return;
		}

		case PROPERTY_MAX_TEXTURE_WIDTH:
		{
			D3DCAPS9 caps;
			m_D3Ddevice->GetDeviceCaps(&caps);
			*v = caps.MaxTextureWidth;
			return;
		}

		case PROPERTY_MAX_TEXTURE_HEIGHT:
		{
			D3DCAPS9 caps;
			m_D3Ddevice->GetDeviceCaps(&caps);
			*v = caps.MaxTextureHeight;
			return;
		}

		case PROPERTY_MAX_TEXTURE_SAMPLERS:
		{
			D3DCAPS9 caps;
			m_D3Ddevice->GetDeviceCaps(&caps);
			*v = caps.MaxSimultaneousTextures;
			return;
		}

		case PROPERTY_VERTEX_SHADER_VERSION:
		{
			D3DCAPS9 caps;
			m_D3Ddevice->GetDeviceCaps(&caps);
			*v = caps.VertexShaderVersion;
			return;
		}

		case PROPERTY_FRAGMENT_SHADER_VERSION:
		{
			D3DCAPS9 caps;
			m_D3Ddevice->GetDeviceCaps(&caps);
			*v = caps.PixelShaderVersion;
			return;
		}

		case PROPERTY_MAX_SIMULTANEOUS_RT:
		{
			D3DCAPS9 caps;
			m_D3Ddevice->GetDeviceCaps(&caps);
			*v = caps.NumSimultaneousRTs;
			return;
		}

		default:
			return;
	}
}

unsigned int Device_DX9::GetAvailableMemory(){
	return m_D3Ddevice->GetAvailableTextureMem()/(1024*1024);
}


Texture* Device_DX9::LoadTexture(const std::string & filename, bool genMipmap)
{
	std::string complete_filename = Core::ResolveFilename(filename);

	if(!Core::FileReader::Exists(filename))
		THROW_EXCEPTION("Unable to find the texture file \"" + filename + "\"");

	size_t tag_split = filename.find("_"); 

	if(tag_split > 0){
		std::string tag_value = filename.substr(0,tag_split);
		
		if(tag_value == "tex2dnpt")
		{
			/// Load nonpower of two texture
			LPDIRECT3DTEXTURE9 loaded_tex = NULL;

			HRESULT res = D3DXCreateTextureFromFileEx(m_D3Ddevice,
				complete_filename.c_str(),
				D3DX_DEFAULT_NONPOW2, 
				D3DX_DEFAULT_NONPOW2,
				D3DX_DEFAULT,
				D3DPOOL_DEFAULT,
				D3DFMT_FROM_FILE,
				D3DPOOL_DEFAULT,
				D3DX_DEFAULT,D3DX_DEFAULT,
				0,
				NULL,
				NULL,
				&loaded_tex);

			if(res == S_OK)
			{
				Texture2D_DX9 *tex = new Texture2D_DX9(this);
				tex->m_Texture = loaded_tex;
				return (Texture2D*)tex;
			}
			else
				THROW_EXCEPTION("Unable to load 2D non power of two texture \"" + filename + "\" from file");
		}
		else if(tag_value == "texcb")
		{
			// cubemap
		}
	}

	/// Try to load simple 2D texture.
	LPDIRECT3DTEXTURE9 loaded_tex = NULL;

	HRESULT res = D3DXCreateTextureFromFileEx(m_D3Ddevice,
		complete_filename.c_str(),
		D3DX_DEFAULT, 
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DPOOL_DEFAULT,
		D3DFMT_FROM_FILE,
		D3DPOOL_DEFAULT,
		D3DX_DEFAULT,D3DX_DEFAULT,
		0,
		NULL,
		NULL,
		&loaded_tex);

	if(res == S_OK)
	{
		Texture2D_DX9 *tex = new Texture2D_DX9(this);
		tex->m_Texture = loaded_tex;
		return (Texture2D*)tex;
	}
	else
		THROW_EXCEPTION("Unable to load 2D non power of two texture \"" + filename + "\" from file");

	return NULL;
}

Texture *Device_DX9::CreateTexture(TextureType type,unsigned int width,
	unsigned int height,unsigned int depth,Format format, bool render_target)
{
	if(type == TEXTURE_3D){

	}
	else if(type == TEXTURE_2D)
	{	
		Texture2D_DX9 *tex = new Texture2D_DX9(this);

		if(SUCCEEDED(m_D3Ddevice->CreateTexture(width, height, 1, 
			render_target?D3DUSAGE_RENDERTARGET : D3DUSAGE_DYNAMIC
			,ConvertFormat_DX9(format), D3DPOOL_DEFAULT, &tex->m_Texture, NULL)))
		{
				tex->m_PFormat = format;
				tex->m_MipFilter = D3DTEXF_NONE;

				if(render_target)
					tex->m_Texture->GetSurfaceLevel(0, &tex->m_Surface);

			return (Texture2D*)tex;
		}

		delete tex;

		THROW_EXCEPTION("Unable to create 2D texture from request parameters");
	}
	else if(type == TEXTURE_CUBEMAP)
	{	
		TextureCubemap_DX9 *tex = new TextureCubemap_DX9(this);

		if(SUCCEEDED(D3DXCreateCubeTexture(m_D3Ddevice,width,1,
				render_target ? D3DUSAGE_RENDERTARGET : D3DUSAGE_DYNAMIC,
				ConvertFormat_DX9(format),D3DPOOL_DEFAULT,&tex->m_Texture)))
			{
				tex->m_PFormat = format;
				tex->m_MipFilter = D3DTEXF_NONE;
				if(render_target){
					tex->m_Texture->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_X,0, &tex->m_Surface[0]);
					tex->m_Texture->GetCubeMapSurface(D3DCUBEMAP_FACE_NEGATIVE_X,0, &tex->m_Surface[1]);
					tex->m_Texture->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_Y,0, &tex->m_Surface[2]);
					tex->m_Texture->GetCubeMapSurface(D3DCUBEMAP_FACE_NEGATIVE_Y,0, &tex->m_Surface[3]);
					tex->m_Texture->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_Z,0, &tex->m_Surface[4]);
					tex->m_Texture->GetCubeMapSurface(D3DCUBEMAP_FACE_NEGATIVE_Z,0, &tex->m_Surface[5]);
				}
				return (TextureCubeMap*)tex;
			}

		delete tex;
		THROW_EXCEPTION("Unable to create cubemap texture from request parameters");
	}

	return NULL;
}



RTManager * Device_DX9::CreateRTManager(unsigned int width,unsigned int height)
{
	RTManager_DX9 * rt = new RTManager_DX9(this);

	if(SUCCEEDED(
		m_D3Ddevice->CreateDepthStencilSurface(width, height,
		m_PresentParameters.AutoDepthStencilFormat,
		D3DMULTISAMPLE_NONE, 0, TRUE, &rt->m_DSSurface, NULL)))
		//m_PresentParameters.MultiSampleType,
		//m_PresentParameters.MultiSampleQuality, TRUE, &rt->m_DSSurface, NULL)))
		// D3DMULTISAMPLE_NONE, 0, TRUE, &rt->m_DSSurface, NULL)))
	{
		
		return rt;
	}


	delete rt;

	THROW_EXCEPTION("Unable to create render target manager from request parameters");
}

DynamicMesh * Device_DX9::CreateDynamicMesh(unsigned int maxVertices, 
	 unsigned int maxTriangles)
{
	DynamicMesh_DX9 * dm = new DynamicMesh_DX9(this);
	try{
		dm->Create(maxVertices,maxTriangles);
	}
	catch(const Core::Exception & ex){
		delete dm;
		THROW_EXCEPTION_STACK("Unable to create dynamic mesh",ex);
	}
	return dm;
}

Mesh *Device_DX9::CreateMesh(unsigned int vertices,VertexProfile vp,
		void *vb,unsigned int groups, unsigned int *gfc, unsigned int **fb)
{
	D3DVERTEXELEMENT9 *vp_dx = VertexProfileToDX(vp);
	
	unsigned int total_faces = 0;
	for(unsigned int i = 0;i<groups;i++)
	{
		total_faces+=gfc[i];
	}

	unsigned int *faces_ids = new unsigned int[total_faces*3];
	DWORD		*faces_subsets = new DWORD[total_faces];

	unsigned int fv_count = 0, sv_count = 0;
	for(unsigned int i = 0;i<groups;i++)
	{
		for(unsigned int j = 0;j<gfc[i]*3;j++)
			faces_ids[fv_count++] = fb[i][j];

		for(unsigned int j = 0;j<gfc[i];j++)
			faces_subsets[sv_count++] = i;
	}

	LPD3DXMESH dx_mesh;
	HRESULT res = D3DXCreateMesh(total_faces,vertices,D3DXMESH_MANAGED | D3DXMESH_32BIT,vp_dx ,m_D3Ddevice,&dx_mesh);

	if(res!=S_OK){
		delete faces_ids;
		delete faces_subsets;
		delete vp_dx;
		THROW_EXCEPTION("Unable to create mesh from request parameters");
	}

	/// carregar vertices
	void* buffer = NULL;
	dx_mesh->LockVertexBuffer( 0, (void**)&buffer );
	memcpy(buffer,vb,vertices*vp.GetVertexSize());
	dx_mesh->UnlockVertexBuffer();

	/// carregar faces
	buffer = NULL;
	dx_mesh->LockIndexBuffer( 0, (void**)&buffer );
	memcpy(buffer,faces_ids,sizeof(int)*3*total_faces);
	dx_mesh->UnlockIndexBuffer();

	/// subsets das faces
	buffer = NULL;
	dx_mesh->LockAttributeBuffer( 0, (DWORD **)&buffer );
	memcpy(buffer,faces_subsets,sizeof(int)*total_faces);
	dx_mesh->UnlockAttributeBuffer();

	LPDIRECT3DVERTEXDECLARATION9 vdec;
	res = m_D3Ddevice->CreateVertexDeclaration( vp_dx, &vdec);

	if(res!=S_OK){
		SafeRelease(dx_mesh);
		delete faces_ids;
		delete faces_subsets;
		delete vp_dx;
		THROW_EXCEPTION("Unable to vertex declaration from request parameters");
	}

	Mesh_DX9 * m = new Mesh_DX9(this);
	m->m_VertexDeclaration = vdec;
	m->m_Mesh = dx_mesh;

	// Cleanup backup data.
	delete faces_ids;
	delete faces_subsets;
	delete vp_dx;
	
	return m;
}



Program *Device_DX9::LoadProgram(const std::string & filename,
						ShaderModelType model, Program *global)
{
	Core::FileReader * reader = Core::FileReader::Open(filename);

	if(!reader)
		THROW_EXCEPTION("Unable to open program \"" + filename + "\" source code");

	long fsize = Core::FileReader::Size(filename);
	char *buffer = new char[fsize+1];
	reader->Read(buffer,fsize);
	buffer[fsize] = '\0';
	SafeDelete(reader);

	std::string src = std::string(buffer);
	delete buffer;

	return LoadProgramFromText(src,model);
}

Program *Device_DX9::LoadProgramFromText(const std::string & text,
	ShaderModelType model, Program *global)
{
	Shader_DX9_Vextex * vsh = NULL;
	Shader_DX9_Fragment * psh = NULL;

	std::map<std::string,std::string> shaderMap = ShaderParser::Map(text);

	try{
		
		if(shaderMap.find("vertex_shader_dx9")!=shaderMap.end())
		{
			vsh = new Shader_DX9_Vextex(this);
			vsh->Load(Program_Resources+shaderMap.find("vertex_shader_dx9")->second,model);

			if(shaderMap.find("pixel_shader_dx9")!=shaderMap.end())
			{
				psh = new Shader_DX9_Fragment(this);
				psh->Load(Program_Resources+shaderMap.find("pixel_shader_dx9")->second,model);
			}
		}

	}
	catch(Core::Exception ex){
		SafeDelete(vsh);
		SafeDelete(psh);
		THROW_EXCEPTION_STACK("Unable to load shader program",ex);
	}
	
	if(!vsh && !psh)
		return NULL;

	Program_DX9 *prg = new Program_DX9(this);

	if(vsh)
		prg->m_Shaders.push_back(vsh);

	if(psh)
		prg->m_Shaders.push_back(psh);

	return prg;
}

void Device_DX9::UpdatePVMatrix(){

	// Avoid unnecessary operations in object mode
	if(m_MatrixMode != D3DTS_PROJECTION && m_MatrixMode != D3DTS_VIEW)
		return; 

	D3DXMATRIX mat_proj;
	m_D3Ddevice->GetTransform(D3DTS_PROJECTION,&mat_proj);

	D3DXMATRIX mat_view;
	m_D3Ddevice->GetTransform(D3DTS_VIEW,&mat_view);

	m_ProjectionViewMatrix = mat_view * mat_proj;
}


void Device_DX9::RestartScene()
{
	if(m_SceneStatus) // finish previous
		m_D3Ddevice->EndScene();

	m_D3Ddevice->BeginScene();

	m_SceneStatus = true;
}

void Device_DX9::FinalizeScene()
{
	if(m_SceneStatus)
		m_D3Ddevice->EndScene();

	m_SceneStatus = false;
}

//-------------------------------------------------------------------

OcclusionQuery *Device_DX9::CreateOcclusionQuery()
{
	LPDIRECT3DQUERY9 lpq = NULL;
	HRESULT res = m_D3Ddevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &lpq);
	
	if(res != S_OK)
		THROW_EXCEPTION("Unable to create occlusion query handler");

	OcclusionQuery_DX9 *q = new OcclusionQuery_DX9(this);
	q->m_OccQuery = lpq;
	return q;
}





D3DVERTEXELEMENT9 *Device_DX9::VertexProfileToDX(const VertexProfile & vprofile)
{
	D3DVERTEXELEMENT9 *vdx = new D3DVERTEXELEMENT9[vprofile.Size() +1];

	for(unsigned int i = 0;i < vprofile.Size();i++)
	{
		vdx[i].Stream	= 0;
		vdx[i].Offset	= vprofile[i].offset;

		if(vprofile[i].type == VERTEX_COLOR)
			vdx[i].Type = D3DDECLTYPE_D3DCOLOR;
		else
			vdx[i].Type	= vprofile[i].components-1;

		vdx[i].Method = D3DDECLMETHOD_DEFAULT;

		if(vprofile[i].type  == VERTEX_POSITION)
			vdx[i].Usage  = D3DDECLUSAGE_POSITION;

		else if(vprofile[i].type == VERTEX_NORMAL)
			vdx[i].Usage  = D3DDECLUSAGE_NORMAL;

		else if(vprofile[i].type == VERTEX_UV)
			vdx[i].Usage  = D3DDECLUSAGE_TEXCOORD;

		else if(vprofile[i].type == VERTEX_COLOR)
			vdx[i].Usage  = D3DDECLUSAGE_COLOR;

		vdx[i].UsageIndex = vprofile[i].index;
	}

	vdx[vprofile.Size()].Stream =0xFF;
	vdx[vprofile.Size()].Offset = 0;
	vdx[vprofile.Size()].Type = D3DDECLTYPE_UNUSED;
	vdx[vprofile.Size()].Method = 0;
	vdx[vprofile.Size()].Usage = 0;
	vdx[vprofile.Size()].UsageIndex = 0;

	return vdx;
}

Device_DX9 *CreateDevice_DX9(Core::Window_Win32 *wnd, 
		unsigned int width, unsigned int height, unsigned int antialiasing,bool fullscreen)
{
	Device_DX9 *dev = new Device_DX9();
	
	dev->m_Width = width;
	dev->m_Height = height;
	dev->m_AntiAliasingAmount = antialiasing;
	
	UINT AdapterToUse=D3DADAPTER_DEFAULT;
	D3DDEVTYPE DeviceType=D3DDEVTYPE_HAL;
	dev->m_D3D = Direct3DCreate9( D3D_SDK_VERSION );

	D3DDISPLAYMODE d3ddm;
	dev->m_D3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );

	//for (UINT Adapter=0;Adapter<dev->m_D3D->GetAdapterCount();Adapter++)
	//{
	//	D3DADAPTER_IDENTIFIER9 Identifier;
	//	HRESULT Res;
	//	Res = dev->m_D3D->GetAdapterIdentifier(Adapter,0,&Identifier);

	//	if (strstr(Identifier.Description,"PerfHUD") != 0)
	//		{
	//		AdapterToUse=Adapter;
	//		DeviceType=D3DDEVTYPE_REF;
	//		break;
	//	}
	//}

	ZeroMemory( &dev->m_PresentParameters, sizeof(D3DPRESENT_PARAMETERS) );
	
	dev->m_PresentParameters.Windowed = !fullscreen;
	dev->m_PresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD ;
	dev->m_PresentParameters.BackBufferFormat = d3ddm.Format;
	dev->m_PresentParameters.EnableAutoDepthStencil = TRUE;
	dev->m_PresentParameters.AutoDepthStencilFormat = D3DFMT_D24X8;
	dev->m_PresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;//D3DPRESENT_INTERVAL_IMMEDIATE;
	dev->m_PresentParameters.BackBufferWidth = width;
	dev->m_PresentParameters.BackBufferHeight = height;

	if(antialiasing == 2)
		dev->m_PresentParameters.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
	else if(antialiasing == 4)
		dev->m_PresentParameters.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
	else if(antialiasing == 8)
		dev->m_PresentParameters.MultiSampleType = D3DMULTISAMPLE_8_SAMPLES;
	else if(antialiasing == 16)
		dev->m_PresentParameters.MultiSampleType = D3DMULTISAMPLE_16_SAMPLES;

	if(FAILED(dev->m_D3D->CheckDeviceMultiSampleType(
					AdapterToUse,DeviceType,
					D3DFMT_D24X8,!fullscreen,dev->m_PresentParameters.MultiSampleType,NULL))){
			dev->m_PresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
	}


	dev->m_D3D->CreateDevice( AdapterToUse, DeviceType, wnd->m_Handle,
						  D3DCREATE_HARDWARE_VERTEXPROCESSING,
						  &dev->m_PresentParameters, &dev->m_D3Ddevice );

	if(dev->m_D3Ddevice == NULL)
	{
		dev->m_PresentParameters.AutoDepthStencilFormat = D3DFMT_D16;
		dev->m_D3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd->m_Handle,
						  D3DCREATE_SOFTWARE_VERTEXPROCESSING,
						  &dev->m_PresentParameters, &dev->m_D3Ddevice );
	}

	if(dev->m_D3Ddevice == NULL){
		SafeDelete(dev);
		THROW_EXCEPTION("Unable to create graphics device from request parameters");
	}

	dev->m_D3Ddevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	dev->m_D3Ddevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	dev->m_D3Ddevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
	dev->m_D3Ddevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,D3DMCS_MATERIAL);

	dev->m_ClearColor[0] = 0.5;
	dev->m_ClearColor[1] = 0.5;
	dev->m_ClearColor[2] = 0.5;
	dev->m_ClearColor[3] = 1.0;

	dev->m_D3Ddevice->GetRenderTarget(0,&dev->m_RenderSurface);
	dev->m_D3Ddevice->GetDepthStencilSurface(&dev->m_DSSurface);

	return dev;
}

#endif // #if defined(NCK_DIRECTX9)

_GRAPHICS_END
