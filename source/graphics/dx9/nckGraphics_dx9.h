
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCKGRAPHICS_DX9_H
#define NCKGRAPHICS_DX9_H

#include "../nckGraphics.h"
#include "../nckTextureCache.h"
#include "../nckVProfile.h"
#include <list>
#include <vector>

#if defined(NCK_DIRECTX9)

#include "win32/nckWindow_win32.h"
#include <d3d9.h>
#include <d3dx9.h>

_GRAPHICS_BEGIN

inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

D3DFORMAT ConvertFormat_DX9(Format source);

class Device_DX9;

class Texture_DX9:public Texture
{
public:
	virtual ~Texture_DX9(){};

	virtual void Enable(unsigned int sampler_id)=0;
	virtual void Disable(unsigned int sampler_id)=0;
	virtual TextureType GetType()=0;
	virtual Format GetFormat()=0;
	virtual unsigned int GetWidth()=0;
	virtual unsigned int GetHeight()=0;

	void SetFilter(FilterType type,FilterMode mode);
	void SetAdressMode(AdressMode mode);

	void SetFilterAndWrapping(unsigned int sampler);
	D3DTEXTUREFILTERTYPE ConvertFilterMode(FilterMode mode);

	Format m_PFormat;
	Device_DX9 *m_Device;
	D3DTEXTUREFILTERTYPE m_MinFilter,m_MagFilter,m_MipFilter;
	D3DTEXTUREADDRESS m_WrapMode;
};

class Texture2D_DX9:public Texture_DX9, public Texture2D
{
public:
	Texture2D_DX9(Device_DX9 *dev);
	~Texture2D_DX9();

	void Enable(unsigned int sampler_id);
	void Disable(unsigned int sampler_id);
	TextureType GetType();
	Format GetFormat();
	unsigned int GetWidth();
	unsigned int GetHeight();
	void SetFilter(FilterType type,FilterMode mode);
	void SetAdressMode(AdressMode mode);

	void *Lock(unsigned int level);
	void Unlock(unsigned int level);

	LPDIRECT3DSURFACE9 m_Surface;
	LPDIRECT3DTEXTURE9 m_Texture;
};

class TextureCubemap_DX9:public TextureCubeMap, public Texture_DX9
{
public:
	TextureCubemap_DX9(Device_DX9 *dev);
	~TextureCubemap_DX9();

	void Enable(unsigned int sampler_id);
	void Disable(unsigned int sampler_id);
	TextureType GetType();
	Format GetFormat();
	unsigned int GetWidth();
	unsigned int GetHeight();
	void SetFilter(FilterType type,FilterMode mode);
	void SetAdressMode(AdressMode mode);

	void *Lock(unsigned int face,unsigned int level);
	void Unlock(unsigned int face,unsigned int level);

	LPDIRECT3DSURFACE9 m_Surface[6];
	LPDIRECT3DCUBETEXTURE9 m_Texture;
};

class RTManager_DX9 : public RTManager
{
public:
	RTManager_DX9(Device_DX9 *dev);
	~RTManager_DX9();

	bool Enable();
	bool Attach(unsigned int target,Texture2D *tex);
	bool Attach(unsigned int target,unsigned int face,TextureCubeMap *tex);
	bool Disable();

	LPDIRECT3DSURFACE9 m_DSSurface;
	Device_DX9 *m_Device;
};


class Mesh_DX9: public Mesh
{
public:
	Mesh_DX9(Device_DX9 *dev);
	~Mesh_DX9();

	void Render(unsigned int subset);
	void Unlock();
	void* Lock();

	void Enable();
	void Disable();

	unsigned int GetSubSetCount();
	unsigned int GetVerticesCount();
	unsigned int GetFacesCount();
	void GetVertexProfile(VertexProfile *p);

	Device_DX9	*m_Device;
	LPDIRECT3DVERTEXDECLARATION9 m_VertexDeclaration;
	LPD3DXMESH m_Mesh;
};

/**
* DirectX9 shader variable.
*/
class Shader_DX9_Variable
{
public:
	enum Type
	{
		FLOAT1,
		FLOAT1_ARRAY,
		FLOAT4,
		FLOAT4_ARRAY,
		INT1,
		INT4,
		INT1_ARRAY,
		INT4_ARRAY,
		MATRIX,
		MATRIX_ARRAY,		
	};

	Shader_DX9_Variable(){
		m_Handle = NULL;
		m_Count = 1;
		m_Type = FLOAT1;
		m_Data = NULL;
		m_Name = "";
		m_Global = NULL;
	}

	~Shader_DX9_Variable(){
		if(m_Data) free(m_Data);
		m_Data=NULL;
	}

	/// Update variable data.
	void Bind(LPD3DXCONSTANTTABLE constants, LPDIRECT3DDEVICE9 dev);

	/// Reference to a global variable.
	Shader_DX9_Variable *m_Global;

	/// Variable name.
	std::string m_Name;

	/// Variable data contents.
	void *m_Data;

	/// Variable type.
	Type m_Type;

	/// Array dimension.
	int m_Count;

	/// DirectX shader handle.
	D3DXHANDLE m_Handle;
};



class Shader_DX9_Shader
{
public:
	virtual ~Shader_DX9_Shader(){}
	
	virtual void Enable()=0;
	virtual void Disable()=0;

	/// Set variable contents.
	bool SetVariable(const std::string & name,Shader_DX9_Variable::Type type,
						unsigned int count,unsigned int esize, void *data);

	/// Shader variables values.
	std::list<Shader_DX9_Variable *> m_Variables;

	/// Shader constants table.
	LPD3DXCONSTANTTABLE	m_Constants;

	/// Program name.
	std::string m_pName;
};

class Shader_DX9_Vextex : public Shader_DX9_Shader
{
public:
	Shader_DX9_Vextex(Device_DX9 *dev);
	~Shader_DX9_Vextex();

	/// Vertex shader.
	LPDIRECT3DVERTEXSHADER9 m_Shader;

	/// Common shader variables.
	D3DXHANDLE m_ViewMatrix;
	D3DXHANDLE m_ModelMatrix;				// ModelMatrix
	D3DXHANDLE m_ModelViewMatrix;			// ModelViewMatrix
	D3DXHANDLE m_ProjectionModelViewMatrix;	// ProjectionModelViewMatrix
	D3DXHANDLE m_ProjectionViewMatrix;
	D3DXHANDLE m_NormalMatrix;				// NormalMatrix
	//D3DXHANDLE m_TextureSampler[8];		// TextureSampler
	D3DXHANDLE m_DiffuseColor;				// DiffuseColor
	D3DXHANDLE m_SpecularColor;				// SpecularColor
	D3DXHANDLE m_AmbientColor;				// AmbientColor
	D3DXHANDLE m_SpecularPower;				// SpecularPower
	D3DXHANDLE m_BoneMatrix;				// BoneMatrix
	D3DXHANDLE m_BoneCount;					// BoneCount

	bool Load(const std::string & code_src, ShaderModelType sm = SHADER_MODEL_2);

	void Enable();
	void Disable();

	/// Reference to graphic device.
	Device_DX9 *m_gDevice;
};

class Shader_DX9_Fragment : public Shader_DX9_Shader
{
public:
	Shader_DX9_Fragment(Device_DX9 *dev);
	~Shader_DX9_Fragment();

	/// Vertex shader.
	LPDIRECT3DPIXELSHADER9 m_Shader;

	/// Common shader variables.
	D3DXHANDLE m_TextureSampler[8];			// TextureSampler
	D3DXHANDLE m_DiffuseColor;				// DiffuseColor
	D3DXHANDLE m_SpecularColor;				// SpecularColor
	D3DXHANDLE m_AmbientColor;				// AmbientColor
	D3DXHANDLE m_SpecularPower;				// SpecularColor

	bool Load(const std::string & code_src, ShaderModelType sm = SHADER_MODEL_2);

	void Enable();
	void Disable();

	/// Reference to graphic device.
	Device_DX9 *m_gDevice;
};

class Program_DX9:public Program
{
public:
	Program_DX9(Device_DX9 *dev);
	~Program_DX9();

	void Enable();
	void Enable_Internal();
	void Disable();

	bool SetVariable1i(const std::string & name,int value);
	bool SetVariable1f(const std::string & name,float value);
	bool SetVariable4i(const std::string & name,int x,int y=0.0,int z=0.0,int w=0.0);
	bool SetVariable4f(const std::string & name,float x,float y=0.0,float z=0.0,float w=0.0);
	bool SetArray1f(const std::string & name,int count,const float *value);
	bool SetArray4f(const std::string & name,int count,const float *value);
	bool SetMatrix(const std::string & name,const float *mat);
	bool SetMatrixArray(const std::string & name,int count,const float *v);

	std::list<Shader_DX9_Shader*> m_Shaders;

	Device_DX9 *m_Device;
};


class OcclusionQuery_DX9: public OcclusionQuery
{
public:
	OcclusionQuery_DX9(Device_DX9 *dev);
	~OcclusionQuery_DX9();

	void Begin();
	void End();
	bool GetDrawnPixels(unsigned int *pixels);

	LPDIRECT3DQUERY9 m_OccQuery;
	Device_DX9* m_Device;
};

class DynamicMesh_DX9 : public virtual DynamicMesh{
public:
	/// Destructor.
	DynamicMesh_DX9(Device_DX9 *dev);
	virtual ~DynamicMesh_DX9();
	void Create(int maxVertices, int maxTriangles);
	void Enable();
	void Disable();
	void Render();
	void LockVBO(void ** data);
	void UnlockVBO();
	void LockIBO(void ** data);
	void UnlockIBO();
	unsigned int GetMaxVertices();
	unsigned int GetMaxTriangles();
	unsigned int GetVerticesCount();
	unsigned int GetTriangleCount();
	void SetVerticeCount(unsigned int count);
	void SetTrianglesCount(unsigned int count);
private:
	Device_DX9 * m_Device;
	LPDIRECT3DVERTEXBUFFER9  m_VertexBuffer;
	unsigned int m_MaxVertices;
	unsigned int m_VerticesCount;

	LPDIRECT3DINDEXBUFFER9  m_IndexBuffer;
	unsigned int m_MaxTriangles;
	unsigned int m_TrianglesCount;
};

class Device_DX9:public Device
{
public:
	Device_DX9();
	~Device_DX9();
	void Viewport(unsigned int x,unsigned int y, unsigned int w, unsigned int h);
	void Clear();
	void ClearFlags(unsigned int flags);
	void ClearColor(float r,float g,float b,float a);
	void PresentAll();
	void Begin(PrimitiveType type);
	void End();
	void Vertex(float x,float y,float z);
	void TexCoord(float x,float y);
	void Color(unsigned char r,unsigned char g,unsigned char b, unsigned char a);
	void Normal(float x,float y,float z);
	void Enable(RenderState cap);
	void Enable(ClipPlaneName plane);
	void Disable(RenderState cap);
	void Disable(ClipPlaneName plane);
	void BlendFunc(BlendMode sfactor,BlendMode dfactor);
	void AlphaFunc(TestFlag func,float value);
	void StencilFunc(TestFlag func,unsigned int ref,unsigned int mask);
	void StencilOp(OperationMode sfail,OperationMode dpmode ,OperationMode dppass);
	void CullMode(CullFaceMode mode);
	void FillMode(PolygonMode mode);
	void DepthBias(float scale,float offset);
	void MatrixMode(MatrixType mode);
	void LoadMatrix(const float *v);
	void MultMatrix(const float *mat);
	void Identity();
	void PushMatrix();
	void PopMatrix();
	void Translate(float x,float y,float z);
	void Scale(float x,float y,float z);
	void Rotate(float angle,float x,float y,float z);
	void Quaternion(float x,float y,float z,float w);
	void Perspective(float aspect, float fov, float znear,float zfar);
	void Ortho(float left,float right,float bottom,float top,float znear,float zfar);
	void Ortho2D(float width,float height,float znear, float zfar);
	void Material(MaterialProperty prop,const float *v);
	void PointSprite(PointSpriteProperty, const float &v);
	void ClipPlane(ClipPlaneName id, const float *v);
	void GetProperty(MatrixType prop,float *v);
	void GetProperty(RenderProperties prop,float *v);
	void GetProperty(RenderProperties prop,int *v);
	unsigned int GetAvailableMemory();
	Texture* LoadTexture(const std::string & filename, bool genMipmap = true);
	Texture *CreateTexture(TextureType type,unsigned int width,
		unsigned int height,unsigned int depth,Format format, bool render_target = true);
	RTManager * CreateRTManager(unsigned int width,
		unsigned int height);
	DynamicMesh * CreateDynamicMesh(unsigned int maxVertices, unsigned int maxTriangles);
	Mesh*	CreateMesh(unsigned int vertices,VertexProfile vp,
		void *vb,unsigned int groups, unsigned int *gfc, unsigned int **fb);
	Program *LoadProgram(const std::string & filename,
		ShaderModelType model = SHADER_MODEL_2, Program *global = NULL);
	Program *LoadProgramFromText(const std::string & text,
		ShaderModelType model = SHADER_MODEL_2, Program *global = NULL);
	OcclusionQuery *CreateOcclusionQuery();

	// Convert blend mode to DX flags.
	D3DBLEND ConvertFromBlendMode(BlendMode blend);

	/// Vertex profile conversion to D3DVERTEXELEMENT9
	D3DVERTEXELEMENT9 *VertexProfileToDX(const VertexProfile & vprofile);

	/// Texture cache manager.
	TextureCache m_TextureCache;

	/// Active shader program.
	Program *m_ActiveProgram;

	/// Main render surface.
	LPDIRECT3DSURFACE9 m_RenderSurface;

	/// Main depth stencil surface.
	LPDIRECT3DSURFACE9 m_DSSurface;

	/// ZFar plate distance.
	float m_Perspective_ZFar;

	/// Scene creation/finalization status.
	/// false not started
	/// true started
	bool m_SceneStatus;

	/// Start new scene and finalize the previous scene.
	void RestartScene();
	
	/// Finalize unfinished scenes.
	void FinalizeScene();

	/// Device creation parameters.
	D3DPRESENT_PARAMETERS m_PresentParameters;

	/// Direct 3D reference.
	LPDIRECT3D9	m_D3D;

	/// Direct 3D device reference.
	LPDIRECT3DDEVICE9 m_D3Ddevice;

	/// Graphic device window width.
	unsigned int m_Width;

	/// Graphic device height.
	unsigned int m_Height;

	/// Anti-Aliasing Amount 
	unsigned int m_AntiAliasingAmount;

	/// Device clear color.
	float m_ClearColor[4];

	/// Clear buffers flags.
	unsigned int m_ClearFlags;

	/// Active transformation matrix.
	D3DTRANSFORMSTATETYPE m_MatrixMode;	

	/// Transformation matrix stack.
	std::list<D3DXMATRIX> m_MatrixStack;

	/// Update projection and view matrix.
	void UpdatePVMatrix();

	/// Projection view matrix.
	D3DXMATRIX m_ProjectionViewMatrix;		

	/// Face culling mode.
	CullFaceMode m_CullFace_Mode;

	/// Face culling state flag.
	bool m_CullFace_Flag;

	/// Face culling "flip" flag. 
	bool m_CullFace_Flip;

	// Update cullmode.
	void UpdateCullMode();

	/// Clip planes vectors.
	D3DXPLANE m_ClipPlanes[6][3];

	unsigned int m_ClipPlaneState;
	/// Clip plane state.
	//bool m_ClipPlaneState[6];

	/// Update clip planes.
	void UpdateClipPlanes();

	/// Selected primitive type for rendering.
	PrimitiveType m_PrimitiveType;

	/// Active vertex ID.
	unsigned m_ActiveVertex;

	/// Vertex buffer.
	BasicVertex m_VertexBuffer[4];

	/// Temporary vertex properties.
	BasicVertex m_VertexTemp;

	/// Current material diffuse color.
	D3DXCOLOR m_DiffuseColor;

	/// Current material specular color.
	D3DXCOLOR m_SpecularColor;

	/// Current material ambient color.
	D3DXCOLOR m_AmbientColor;

	/// Current material specular shininess factor.
	float	  m_SpecularPower;
};

Device_DX9 * CreateDevice_DX9(Core::Window_Win32 *wnd, 
		unsigned int width, unsigned int height, unsigned int antialiasing,bool fullscreen);

_GRAPHICS_END

#endif // #if defined(NCK_DIRECTX9)

#endif // #ifndef NCKGRAPHICS_DX9_H
