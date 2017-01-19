
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_GRAPHICS_GL2_H
#define NCK_GRAPHICS_GL2_H

#include "../nckGraphics.h"
#include "../nckTextureCache.h"
#include "../nckVProfile.h"
#include "../nckResourceProxy.h"

#include "nckMat44.h"
#include "nckPlane.h"
#include "nckImage.h"

#include <list>
#include <vector>
#include <map>

#if defined(NCK_WINDOWS)
    #include "win32/nckWindow_win32.h"
#elif defined(NCK_LINUX)
    #include "linux/nckWindow_linux.h"
#elif defined(NCK_MACOSX)
    #include "macosx/nckGLBind_macosx.h"
#endif

#include "nckExtensions_gl2.h"

_GRAPHICS_BEGIN

#if defined(NCK_OPENGL2)

// Rendering device class prototype
class Device_GL2;

/**
* Abstract class to support textures in OpenGL2.
*/
class Texture_GL2:public virtual Texture
{
public:
	Texture_GL2(Device_GL2 *dev);
	virtual ~Texture_GL2();

	Format GetFormat();
	unsigned int GetWidth();
	unsigned int GetHeight();
	void SetFilter(FilterType type,FilterMode mode);
	void SetAdressMode(AdressMode mode);

#ifdef NCK_GRAPH_RES_PROXY
	ResourceProxy<Texture_GL2> * GetProxyManager();
#endif

protected:
	/**
	 * Update texture wrapping and filter state.
	 */
	void SetFilterAndWrapping();

	/**
	 * Get OpenGL parameters for a type of texture.
	 */
	static void GetGLPropertiesFromFormat(Format format, int * glComponents,
			GLenum *glInternalFormat, GLenum *glFormat, GLenum * glType);

	/**
	 * Convert filtering mode to a OpenGL type.
	 */
	static GLenum ConvertFilterMode(FilterMode mode);

	/// Texture width.
	int m_Width;

	/// Texture height.
	int m_Height;

	/// OpenGL texture id.
	GLuint m_Texture;

	/// Texture color format.
	Format m_Format;

	/// Reference to rendering device.
	Device_GL2 *m_Device;

	/// Texture filtering state.
	GLuint m_MinFilter,m_MagFilter,m_MipFilter;

	/**
	 * Texture wrapping to all the axis.
	 * TODO: Add individual axis wrapping.
	 */
	GLuint m_WrapMode;

	/// OpenGL target type id.
	GLuint m_Target;

#ifdef NCK_GRAPH_RES_PROXY

	/// Texture Proxy Manager.
	ResourceProxy<Texture_GL2> * m_Proxy;
#endif
};

/**
 * Implementation class for 2D Textures support in OpenGL2.
 */
class Texture2D_GL2:public virtual Texture2D, public Texture_GL2
{
public:
	Texture2D_GL2(Device_GL2 *dev);
	static Texture2D_GL2 * Load(Device_GL2 *dev,const std::string & filename, bool mipmaps);
	virtual ~Texture2D_GL2();

	void Enable(unsigned int sampler_id);
	void Disable(unsigned int sampler_id);
	TextureType GetType();
	GLuint getTextureId();
	void Create(unsigned int width, unsigned int height, Format format, bool renderTarget);
	void *Lock(unsigned int level);
	void Unlock(unsigned int level);

	bool m_LockFlag;
	GLubyte * m_LockData;

    bool m_Mipmap;
    int64_t m_LastModified;
    std::string m_Filename;
    int Reload();
};

/**
 * Implementation class for CubeMap Textures support in OpenGL 2.
 */
class TextureCubemap_GL2:public virtual TextureCubeMap, public Texture_GL2
{
public:
	/**
	* Load cubemap from a collection of images. 
	*/
	static TextureCubemap_GL2 * Load(Device_GL2 * dev, const std::vector<Core::Image*> & images, bool mipmaps);	

	/**
	* Load cubemap from a tar package.
	*/
	static TextureCubemap_GL2 * Load(Device_GL2 * dev,const std::string & packageFilename, bool mipmaps);

	TextureCubemap_GL2(Device_GL2 *dev);
	virtual ~TextureCubemap_GL2();

	void Enable(unsigned int sampler_id);
	void Disable(unsigned int sampler_id);
	TextureType GetType();

	void *Lock(unsigned int face,unsigned int level);
	void Unlock(unsigned int face,unsigned int level);
};

/**
 * Implementation class for 3D Textures support in OpenGL2.
 */
class Texture3D_GL2:public virtual Texture3D, public Texture_GL2
{
public:
	/**
	* Load 3d texture from a tar package.
	*/
	static Texture3D_GL2 * Load(Device_GL2 * dev,const std::string & packageFilename, bool mipmaps);

	/**
	* Load 3d texture from multiple filenames.
	*/
	static Texture3D_GL2 * Load(Device_GL2 * dev,const std::vector<std::string> & filenames, bool mipmaps);

	/**
	* Load 3d texture from a collection of images.
	*/
	static Texture3D_GL2 * Load(Device_GL2 * dev,const std::vector<Core::Image*> & images,bool mipmaps);

	Texture3D_GL2(Device_GL2 *dev);
	virtual ~Texture3D_GL2();

	void Enable(unsigned int sampler_id);
	void Disable(unsigned int sampler_id);

	TextureType GetType();
	GLuint getTextureId();

	/**
	* Get the number of Z planes in the texture.
	*/
	unsigned int GetDepth();

protected:
	/// Number of Z planes
	unsigned int m_Depth;
};

/**
 * Implementation class for Render Targets or Frame Buffer Objects OpenGL2.
 */
class RTManager_GL2 : public RTManager
{
public:
	class AttachedTexture{
	public:
		AttachedTexture(Texture_GL2* tex,
				uint32_t target,uint32_t face){
			this->texture = tex;
			this->target = target;
			this->face = face;
		}
		Texture_GL2* texture;
		uint32_t target;
		uint32_t face;
	};

	RTManager_GL2(Device_GL2 *dev);
	virtual ~RTManager_GL2();

	bool Enable();
	void Load(GLuint width, GLuint height);
	bool Attach(unsigned int target,Texture2D *tex);
	bool Attach(unsigned int target,unsigned int face,TextureCubeMap *tex);
	bool Disable();
    bool InvertedY();
protected:
	std::list<AttachedTexture> m_Textures;
	GLuint m_RenderBuffer;
	GLuint m_FrameBuffer;
	Device_GL2 *m_Device;
};

/**
 * Implementation class for Vertex Buffer Object and Index Buffer Object based
 * Triangular meshes in OpenGL2.
 */
class Mesh_GL2: public virtual Mesh
{
public:
	struct FacesGroup{
		uint32_t count;
		uint32_t buffer;
	};

	Mesh_GL2(Device_GL2 *dev);
	virtual ~Mesh_GL2();

	void Load(uint32_t vertices,VertexProfile profile, 
			void * vertexdata, uint32_t face_groups,
			uint32_t *face_count,uint32_t **faces);

	void Render(unsigned int subset);
	void Unlock();
	void* Lock();

	void Enable();
	void Disable();

	unsigned int GetSubSetCount();
	unsigned int GetVerticesCount();
	unsigned int GetFacesCount();

	void GetVertexProfile(VertexProfile *p);

protected:
	Device_GL2 * m_Device;
	VertexProfile m_VertexProfile;
	GLuint m_VertexBuffer;
	std::vector<FacesGroup> m_IndexBuffer;
	GLuint m_VerticeCount;
};

/**
 * Class to store shader variable state in OpenGL2.
 */
class Shader_GL2_Variable
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

	Shader_GL2_Variable(){
		m_NeedsUpdate = true;
		m_Handle = 0;
		m_Count = 1;
		m_Type = FLOAT1;
		m_Data = NULL;
		m_Name = "";
		m_Global = NULL;
	}

	~Shader_GL2_Variable(){
		if(m_Data) free(m_Data);
		m_Data=NULL;
	}

	/// Update variable data.
	void Bind();

	/// Reference to a global variable.
	Shader_GL2_Variable *m_Global;

	/// Variable name.
	std::string m_Name;

	/// Variable data contents.
	void *m_Data;

	/// Variable type.
	Type m_Type;

	/// Array dimension.
	int m_Count;

	/// GLSL shader handle.
	GLuint m_Handle;

	/// Requires update
	bool m_NeedsUpdate;
};

/**
 * Implementation class for Program / Shaders objects in OpenGL2.
 */
class Program_GL2:public virtual Program
{
public:
	Program_GL2(Device_GL2 *dev);
	virtual ~Program_GL2();

	void Load(const std::string & source);
	void LoadFromFilename(const std::string & filename);

	void EnableInternal();
	void Enable();
	void Disable();
	void DisableInternal();

	int GetVariableId(const std::string & name);
	bool SetVariable1i(const std::string & name,int value);
	bool SetVariable1f(const std::string & name,float value);
	bool SetVariable4i(const std::string & name,int x,int y=0.0,int z=0.0,int w=0.0);
	bool SetVariable4f(const std::string & name,float x,float y=0.0,float z=0.0,float w=0.0);
	bool SetArray1f(const std::string & name,int count,const float *value);
	bool SetArray4f(const std::string & name,int count,const float *value);
	bool SetMatrix(const std::string & name,const float *mat);
	bool SetMatrixArray(const std::string & name,int count,const float *v);

#ifdef NCK_GRAPH_RES_PROXY
	ResourceProxy<Program_GL2> * GetProxyManager();
#endif

	bool m_ToRemove;
	
    int Reload();
    void BindUniforms();

protected:
    std::string m_Filename;
    int64_t m_LastModified;
	
    GLint m_ViewMatrix;
	GLint m_ModelMatrix;				// ModelMatrix
	GLint m_ModelViewMatrix;			// ModelViewMatrix
	GLint m_ProjectionModelViewMatrix;	// ProjectionModelViewMatrix
	GLint m_ProjectionViewMatrix;
	GLint m_NormalMatrix;				// NormalMatrix
	GLint m_TextureSampler[8];			// TextureSampler
	GLint m_DiffuseColor;				// DiffuseColor
	GLint m_SpecularColor;				// SpecularColor
	GLint m_AmbientColor;				// AmbientColor
	GLint m_SpecularPower;				// SpecularPower
	GLint m_BoneMatrix;                 // BoneMatrix
	GLint m_BoneCount;                  // BoneCount
    GLint m_Alpha;                      // Transparency

	bool m_Active;
 
	GLuint m_VertexShader;
	GLuint m_FragmentShader;

	std::map<std::string,Shader_GL2_Variable*> m_Variables;
	GLuint m_Program;

#ifdef NCK_GRAPH_RES_PROXY
	ResourceProxy<Program_GL2> * m_Proxy;
#endif

	Device_GL2 * m_Device;
};

/**
 * Implementation for Occlusion Query object in OpenGL2.
 */
class OcclusionQuery_GL2: public OcclusionQuery
{
public:
	OcclusionQuery_GL2(Device_GL2 *dev);
	virtual ~OcclusionQuery_GL2();

	void Begin();
	void End();
	bool GetDrawnPixels(unsigned int *pixels);

	GLuint m_OccQuery;
	Device_GL2* m_Device;
};

/**
 * Implementation for mutable geometry object in OpenGL2.
 */
class DynamicMesh_GL2 : public virtual DynamicMesh{
public:
	DynamicMesh_GL2(Device_GL2 *dev);
	virtual ~DynamicMesh_GL2();
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
	unsigned int GetTrianglesCount();
	void SetVerticesCount(unsigned int count);
	void SetTrianglesCount(unsigned int count);
private:
	Device_GL2 * m_Device;
	float  * m_VertexBuffer;
	unsigned int m_MaxVertices;
	unsigned int m_VerticesCount;

	int32_t *  m_IndexBuffer;
	unsigned int m_MaxTriangles;
	unsigned int m_TrianglesCount;
};

/**
 * Implementation class for OpenGL2 graphics rendering.
 */
class Device_GL2:public Device
{
public:
	Device_GL2(Core::Window *wnd,
			unsigned int width, unsigned int height, unsigned int antialiasing,bool fullscreen);
	virtual ~Device_GL2();

	void Viewport(unsigned int x,unsigned int y, unsigned int w, unsigned int h);
	void Clear();
	void ClearFlags(unsigned int flags);
	void ClearColor(float r,float g,float b,float a);
	void PresentAll();
	void Begin(PrimitiveType type);
	void End();
	void Vertex(float x,float y,float z);
	void TexCoord(float x,float y);
	void Color(unsigned char r,unsigned char g,unsigned char b,
		unsigned char a);
	void Normal(float x,float y,float z);
	void Enable(RenderState cap);
	void Enable(ClipPlaneName plane);
	void Disable(RenderState cap);
	void Disable(ClipPlaneName plane);
	void BlendFunc(BlendMode sfactor,BlendMode dfactor);
	void AlphaFunc(TestFlag func,float value);
	void StencilFunc(TestFlag func,unsigned int ref,unsigned int mask);
	void StencilOp(OperationMode sfail, OperationMode dpmode, OperationMode dppass);
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
	void Ortho(float left,float right, float bottom, float top,float znear,float zfar);
	void Ortho2D(float width,float height,float znear, float zfar);
	void Material(MaterialProperty prop,const float *v);
	void PointSprite(PointSpriteProperty, const float & v);
	void ClipPlane(ClipPlaneName id, const float *v);
	void GetProperty(MatrixType prop,float *v);
	void GetProperty(RenderProperties prop,float *v);
	void GetProperty(RenderProperties prop,int *v);
	unsigned int GetAvailableMemory();
	Texture* LoadTexture(const std::string & filename, bool genMipmap);
	Texture * LoadTexture3D(const std::vector<std::string> & filenames, bool genMipmap);
	Texture *CreateTexture(TextureType type,unsigned int width,	unsigned int height,
                           unsigned int depth,Format format, bool render_target = true);
	RTManager * CreateRTManager(unsigned int width,	unsigned int height);
	Mesh * CreateMesh(unsigned int vertices,VertexProfile vp, void *vb,unsigned int groups,
                      unsigned int *gfc, unsigned int **fb);
	DynamicMesh * CreateDynamicMesh(unsigned int maxVertices, unsigned int maxTriangles);
	Program *LoadProgram(const std::string & filename,
		ShaderModelType model = SHADER_MODEL_2, Program *global = NULL);
	Program *LoadProgramFromText(const std::string & text,
		ShaderModelType model = SHADER_MODEL_2, Program *global = NULL);
	OcclusionQuery *CreateOcclusionQuery();
	bool Capture(int x, int y, int width, int height, Format colorFormat, unsigned char * buffer);
    int ReloadPrograms(int * reloaded, std::list<std::string> * errors);
    int ReloadTextures(int * reloaded, std::list<std::string> * errors);

	/// FBO rendering flag.
	bool m_InsideFBO;

	// Convert blend mode to OpenGL flags.
	GLenum ConvertFromBlendMode(BlendMode blend);

	/// Texture cache manager.
	TextureCache m_TextureCache;

	/// Active shader program.
	Program_GL2 *m_ActiveProgram;

	/// Target render window.
	Core::Window *m_Window;

#if defined(NCK_WINDOWS)
	HDC		m_DrawingContext;
	HGLRC	m_RenderingContext;
	HWND	m_WindowHandle;
#elif defined(NCK_LINUX)
	/// Active X11 OpenGL context.
	GLXContext m_Context;
#endif

	/// This flag is true if class destructor is called, 
	// to stop recursive calls to proxy manager.
	bool m_DestructorFlag;

	/// Graphic device window width.
	unsigned int m_Width;

	/// Graphic device height.
	unsigned int m_Height;

	/// Anti-Aliasing Amount
	unsigned int m_AntiAliasingAmount;

	/// Depth buffer bits count
	unsigned int m_DepthBufferBits;

	/// Device clear color.
	float m_ClearColor[4];

	/// Clear buffers flags.
	unsigned int m_ClearFlags;

	/// Active transformation matrix.
	GLenum m_MatrixMode;

	/// Transformation matrix stack.
	std::list<Math::Mat44> m_Stack;

	Math::Mat44 m_ProjectionMatrix;
	Math::Mat44 m_ViewMatrix;
	Math::Mat44 m_ModelMatrix;

	Math::Mat44 m_ModelViewMatrix;
	Math::Mat44 m_InvModelViewMatrix;

	Math::Mat44 m_ProjectionViewMatrix;
	Math::Mat44 m_ProjectionViewModelMatrix;

#ifdef NCK_GRAPH_RES_PROXY
	/// Texture Proxy Map
	std::map<std::string,ResourceProxy<Texture_GL2>*> m_TextureProxys;

	ResourceProxy<Texture_GL2> * GetTextureProxy(const std::string & fileName);
	ResourceProxy<Texture_GL2> * AddTextureProxy(const std::string & fileName, Texture_GL2 * tex);

	/**
	* Returns True if last texture object removed, GPU object can be safely released. 
 	*/
	bool RemoveTextureProxy(Texture_GL2 * tex);

	/// Program Proxy Map
	std::map<std::string,ResourceProxy<Program_GL2>*> m_ProgramProxys;

	ResourceProxy<Program_GL2> * GetProgramProxy(const std::string & fileName);
	ResourceProxy<Program_GL2> * AddProgramProxy(const std::string & fileName, Program_GL2 * prog);

	/**
	* Returns True if last program object removed, GPU object can be safely released. 
 	*/
	bool RemoveProgramProxy(Program_GL2 * prog);
#endif

    std::map<GLuint, GLuint> m_SamplersPerTarget;
	
    /// Update various matrix for shaders.
	void UpdateMatrix();

	/// Face culling mode.
	CullFaceMode m_CullFace_Mode;

	/// Face culling state flag.
	bool m_CullFace_Flag;

	/// Face culling "flip" flag.
	bool m_CullFace_Flip;

	// Update cullmode.
	void UpdateCullMode();

	/// Clip planes vectors.
	Math::Plane m_ClipPlanes[6];

	/// Clip plane state.
	bool m_ClipPlaneState[6];

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
	float m_DiffuseColor[4];

	/// Current material specular color.
	float m_SpecularColor[4];

	/// Current material ambient color.
	float m_AmbientColor[4];

	/// Current material specular shininess factor.
	float m_SpecularPower;

    /// Current material transparency.
    float m_Alpha;

    // Is model matrix active.
    bool m_IsModelMatrixActive;
    bool m_IsModelViewClear;

    MatrixType m_CurrentMatrix;

    std::list<Math::Mat44> m_ModelMatrixStack;
};

Device_GL2 * CreateDevice_GL2(Core::Window *wnd,
		unsigned int width, unsigned int height, unsigned int antialiasing,bool fullscreen);

#endif // #if defined(NCK_OPENGL2)

_GRAPHICS_END

#endif // #ifndef NCK_GRAPHICS_GL2_H
