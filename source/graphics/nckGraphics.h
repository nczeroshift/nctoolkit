
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_GRAPHICS_H
#define NCK_GRAPHICS_H

#include "nckGraphicsConfig.h"
#include "nckWindow.h"
#include "nckVProfile.h"

#include <string>
#include <list>

_GRAPHICS_BEGIN

/// Texture filter selection.
enum FilterType{
	FILTER_MINIFICATION,	// Minification filter.
	FILTER_MAGNIFICATION,	// Magnification filter.
	FILTER_MIPMAPPING,		// Mipmapping filter.
};

/// Texture filter operation mode.
enum FilterMode{
	FILTER_NONE,	// Disable filter(For mipmapping filter only)
	FILTER_NEAREST, // Use the nearest pixel.
	FILTER_LINEAR,	// Use bilinear interpolation.
};

/// Texture texel acess mode.
enum AdressMode{
	ADRESS_CLAMP,	// Limit uv mapping to 0..1 coordinates.
	ADRESS_WRAP,	// Cyclic uv mapping coordinates.
};

/// Color buffers formats.
enum Format{
	FORMAT_RGB_8B,
	FORMAT_RGBA_8B,
	FORMAT_RGBA_16F,
	FORMAT_RGBA_32F,
	FORMAT_A_8B,  // Single 8 bits alpha channel. 
	FORMAT_R_16F, // Single 16 bits floating point channel. 
	FORMAT_R_32F, // Single 32 bits floating point channel. 
};

/// Texture types.
enum TextureType{
	TEXTURE_2D,
	TEXTURE_3D,
	TEXTURE_CUBEMAP,
};

class Device;
class VertexProfile;

/**
 * Interface class for the graphics device textures.
 */
class Texture
{
public:
	/// Destructor
	virtual ~Texture(){};

	/// Assign texture to a sampler.
	virtual void Enable(unsigned int sampler_id = 0)=0;

	/// Remove texture from a sampler.
	virtual void Disable(unsigned int sampler_id = 0)=0;

	/// Configure texture filtering modes.
	virtual void SetFilter(FilterType type,FilterMode mode)=0;

	/// Set texture adress mode.
	virtual void SetAdressMode(AdressMode mode)=0;
	
    /// Set anisotropy value
    virtual void SetAnisotropyFilter(float value)=0;

	/// Return texture type.
	virtual TextureType GetType()=0;

	/// Return pixel format.
	virtual Format GetFormat()=0;

	/// Return texture width.
	virtual unsigned int GetWidth()=0;

	/// Return texture height.
	virtual unsigned int GetHeight()=0;
};

/**
 * Interface class for the graphics device 2D textures.
 */
class Texture2D: public virtual Texture
{
public:
	/// Acess to texture data. 
	/// @param level Mipmapping map index.
	virtual void *Lock(unsigned int level=0)=0;

	/// Release the acess to texture data.
	virtual void Unlock(unsigned int level=0)=0;
};

/**
 * Interface class for the graphics device cubemap textures.
 */
class TextureCubeMap: public virtual Texture
{
public:
	/// Access to texture data.
	/// @param level Mipmapping map index.
	/// @param face Cubemap face index.
	virtual void *Lock(unsigned int face,unsigned int level=0)=0;

	/// Release the access to texture data.
	/// @param level Mipmapping map index.
	/// @param face Cubemap face index.
	virtual void Unlock(unsigned int face,unsigned int level=0)=0;
};


/**
 * Interface class for the graphics device 3D textures.
 */
class Texture3D: public virtual Texture
{
public:
	/// Get texture depth count.
	virtual unsigned int GetDepth()=0;
};

/**
 * Interface class for the graphics device render targets managers.
 */
class RTManager
{
public:
	/// Destructor
	virtual ~RTManager(){};

	/// Switch to rendering context to the render target. 
	/// @param surf_id Surface ID number for cubemaps only.
	/// @return True if the rendering context switch was successful, false
	///otherwise.
	virtual bool Enable(int face = 0) = 0;

	/// Attach 2d texture as a render target.
	/// @param target Render target ID, must be less than PROPERTY_MAX_SIMULTANEOUS_RT.
	/// @param tex Reference to texture to be used as render target.
	/// @return False if the texture doesn't support usage as RT or invalid
	/// target ID, true otherwise.
	virtual bool Attach(unsigned int target, Texture2D * tex) = 0;

	/// Attach cubemap texture as a render target.
	/// @param face Cubemap face ID (0 to 5).
	virtual bool Attach(unsigned int target, TextureCubeMap *tex) = 0;

	/// Remove all textures
	virtual void DetachAll() = 0;

	/// Switch back to default rendering context.
	virtual bool Disable() = 0;

	/// Check if rendered output is inverted (opengl/directx)
    virtual bool InvertedY() = 0;
};

/**
 * Interface class for graphics device triangle based models.
 */
class Mesh
{
public:
	/// Destructor.
	virtual ~Mesh(){};
	
	virtual void Enable() = 0;
	virtual void Disable() = 0;

	/// Render model subset.
	/// @param subset Index of faces set to be rendered.
	virtual void Render(unsigned int subset = 0)=0;

	/// Get the number of faces subsets used in the mesh.
	virtual unsigned int GetSubSetCount()=0;

	/// Get the number of vertices.
	virtual unsigned int GetVerticesCount()=0;

	/// Get the number of triangle faces.
	virtual unsigned int GetFacesCount()=0;

	/// Get acess to vertex profile data.
	virtual void GetVertexProfile(VertexProfile *p)=0;

	/// Release the acess to mesh vertex buffer data. 
	virtual void Unlock()=0;

	/// Acess to mesh vertex buffer data. 
	virtual void* Lock()=0;
};

/**
 * Interface class for variable geometry type mesh.
 */
class DynamicMesh{
public:
	/// Destructor.
	virtual ~DynamicMesh(){};

	virtual void Enable() = 0;
	virtual void Disable() = 0;

	virtual void Render()=0;

	virtual void LockVBO(void ** data)=0;
	virtual void UnlockVBO()=0;

	virtual void LockIBO(void ** data)=0;
	virtual void UnlockIBO()=0;

	virtual unsigned int GetMaxVertices()=0;
	virtual unsigned int GetMaxTriangles()=0;

	virtual unsigned int GetVerticesCount()=0;
	virtual unsigned int GetTrianglesCount()=0;

	virtual void SetVerticesCount(unsigned int count)=0;
	virtual void SetTrianglesCount(unsigned int count)=0;
};

/// Shader models types.
enum ShaderModelType{
	SHADER_MODEL_1,
	SHADER_MODEL_2,
	SHADER_MODEL_2A,
	SHADER_MODEL_2B,
	SHADER_MODEL_3,
	SHADER_MODEL_4,
};

/**
 * Interface class for graphics device shader programs.
 */
class Program
{
public:
	/// Destructor.
	virtual ~Program(){};

	/// Enable Shaders.
	virtual void Enable()=0;

	/// Disable Shaders.
	virtual void Disable()=0;

	/// Get variable index for fast setter 
	/// @return Integer number with variable id, Positive or zero. Returns -1 if 
	/// variable not found.
	virtual int GetVariableId(const std::string & name) = 0;

	/// Initialize shader integer variable.
	/// @param name Variable name.
	/// @param value Variable value.
	/// @return Returns false if variable name is invalid, true otherwise.
	virtual bool SetVariable1i(const std::string & name,int value)=0;

	/// Initialize a shader variable of four integers.
	/// @param name Variable name.
	/// @param x X component value.
	/// @param y Y component value(by default is zero).
	/// @param y Z component value(by default is zero).
	/// @param y W component value(by default is zero).
	/// @return Returns false if variable name is invalid, true otherwise.	
	virtual bool SetVariable4i(const std::string & name,int x,
		int y=0.0,
		int z=0.0,
		int w=0.0)=0;

	/// Initialize shader float variable.
	/// @param name Variable name.
	/// @param value Variable value.
	/// @return Returns false if variable name is invalid, true otherwise.	
	virtual bool SetVariable1f(const std::string & name,float value)=0;

	/// Initialize shader array of floats
	/// @param name Variable name.
	/// @param count Array size.
	/// @param value Array contents.
	/// @return Returns false if variable name is invalid, true otherwise.
	virtual bool SetArray1f(const std::string & name,int count,const float *value)=0;

	/// Initialize shader variable of four floats.
	/// @param name Variable name.
	/// @param x X component value.
	/// @param y Y component value(by default is zero).
	/// @param y Z component value(by default is zero).
	/// @param y W component value(by default is zero).
	/// @return Returns false if variable name is invalid, true otherwise.	
	virtual bool SetVariable4f(const std::string & name,float x,
		float y=0.0,
		float z=0.0,
		float w=0.0)=0;

	/// Initialize shader array of four floats.
	/// @param name Variable name.
	/// @param count Array size.
	/// @param value Array contents.
	/// @return Returns false if variable name is invalid, true otherwise.	
	virtual bool SetArray4f(const std::string & name,int count,const float *value)=0;

	/// Initialize shader matrix variable.
	/// @param name Variable name.
	/// @param value Variable value.
	/// @return Returns false if variable name is invalid, true otherwise.	
	virtual bool SetMatrix(const std::string & name,const float *mat)=0;

	/// Initialize shader array of 4x4 matrix.
	/// @param name Variable name.
	/// @param value Array size.
	/// @param v Array contents.
	/// @return Returns false if variable name is invalid, true otherwise.	
	virtual bool SetMatrixArray(const std::string & name,int count,const float *v)=0;

	/// Set time variable value.
	static void SetGlobalTime(double time);
	static double GetGlobalTime();
};

/// Geometry primitives types.
enum PrimitiveType{
	PRIMITIVE_LINES,
	PRIMITIVE_TRIANGLES,
	PRIMITIVE_QUADS,
	PRIMITIVE_POINTS,
};

/// Transformation matrix types.
enum MatrixType{
	MATRIX_PROJECTION,
	MATRIX_VIEW,
	MATRIX_MODEL,
};

/// Rendering device state properties.
enum RenderState{
	STATE_BLEND,
	STATE_DEPTH_TEST,
	STATE_ALPHA_TEST,
	STATE_CULL_FACE,
	STATE_FOG,
	STATE_ZBUFFER_WRITE,
	STATE_COLOR_WRITE,
	STATE_FLIP_CULL_FACE,
	STATE_STENCIL_TEST,
	STATE_POINT_SPRITES,
	STATE_POINT_SPRITES_SCALE,
};

/// Render device properties.
enum RenderProperties{
	PROPERTY_VIEWPORT,
	PROPERTY_MAX_POINT_SIZE,
	PROPERTY_MAX_TEXTURE_WIDTH,
	PROPERTY_MAX_TEXTURE_HEIGHT,
	PROPERTY_MAX_TEXTURE_SAMPLERS,
	PROPERTY_VERTEX_SHADER_VERSION,
	PROPERTY_FRAGMENT_SHADER_VERSION,
	PROPERTY_MAX_SIMULTANEOUS_RT,
};

/// Clip planes names.
enum ClipPlaneName{
	CLIP_PLANE_0,
	CLIP_PLANE_1,
	CLIP_PLANE_2,
	CLIP_PLANE_3,
	CLIP_PLANE_4,
	CLIP_PLANE_5,
};

/// Render device cull modes.
enum CullFaceMode{
	CULL_FRONT,
	CULL_BACK,
};

/// Render device primitives fill modes.
enum PolygonMode{
	FILL_SOLID,
	FILL_WIREFRAME,
};

/// Color blend modes.
enum BlendMode{
	BLEND_ZERO,
	BLEND_ONE,
	BLEND_SRC_COLOR,
	BLEND_DST_COLOR,
	BLEND_INV_SRC_COLOR,
	BLEND_INV_DST_COLOR,
	BLEND_SRC_ALPHA,
	BLEND_INV_SRC_ALPHA,
	BLEND_DST_ALPHA,
	BLEND_INV_DST_ALPHA,
};

/// Device test flags.
enum TestFlag{
	TEST_NEVER,
	TEST_LESS,
	TEST_EQUAL,
	TEST_LESSEQUAL,
	TEST_GREATER,
	TEST_NOTEQUAL,
	TEST_GREATEREQUAL,
	TEST_ALWAYS,
};

/// Device buffers operation modes.
enum OperationMode{
	OPERATION_KEEP,
	OPERATION_REPLACE,
	OPERATION_INCREASE,
	OPERATION_DECREASE,
};

/// Device buffers identification.
enum BufferFlags{
	BUFFER_DEPTH_BIT=1,
	BUFFER_COLOR_BIT=2,
	BUFFER_STENCIL_BIT=4,
};

/// Material properties.
enum MaterialProperty{
	MATERIAL_DIFFUSE_COLOR,
	MATERIAL_SPECULAR_COLOR,
	MATERIAL_AMBIENT_COLOR,
	MATERIAL_SHININESS,
	MATERIAL_AMBIENT,
    MATERIAL_ALPHA,
};

/// Point sprite properties.
enum PointSpriteProperty{
	POINT_MAX_SIZE=6,
	POINT_MIN_SIZE=7,
	POINT_SIZE=8,
};

/**
 * Interface class for the graphic device occlusion query methods.
 */
class OcclusionQuery
{
public:
	/// Destructor.
	virtual ~OcclusionQuery(){};

	/// Begin rendering query.
	virtual void Begin()=0;

	/// End rendering query.
	virtual void End()=0;

	/// Get the number of drawn pixels. 
	/// @param px_count Number of pixels drawn.
	/// @return True if the query has finished, false otherwise.
	virtual bool GetDrawnPixels(unsigned int *px_count)=0;
};

/**
 * Interface class for the graphics device rendering system.
 */
class Device
{
public:
	/// Destructor.
	virtual ~Device(){};
	
	/// Viewport definition method.
	virtual void Viewport(unsigned int x,unsigned int y, unsigned int w,unsigned int h)=0;

	/// Clear current render target.
	virtual void Clear()=0;

	/// Set render target clear flags.
	virtual void ClearFlags(unsigned int flags)=0;

	/// Set render target clear color.
	virtual void ClearColor(float r,float g,float b,float a=1.0f)=0;

	/// Finalize the rendering and present the produced content.
	virtual void PresentAll()=0;

	/// Begin geometry primitives rendering.
	virtual void Begin(PrimitiveType type)=0;

	/// End geometry generation.
	virtual void End()=0;

	/// Set vertex positon.
	virtual void Vertex(float x,float y,float z=0.0f)=0;

	/// Set vertex texture coordinates.
	virtual void TexCoord(float x,float y)=0;

	/// Set vertex texture coordinates.
	virtual void Color(unsigned char r,unsigned char g,
						unsigned char b,unsigned char a=0xFF)=0;

	/// Set vertex normal.
	virtual void Normal(float x,float y,float z)=0;

	/// Enable render device capabilities.
	virtual void Enable(RenderState cap)=0;

	/// Enable render device clip plane. 
	virtual void Enable(ClipPlaneName plane)=0;

	/// Disable render device capabilities.
	virtual void Disable(RenderState cap)=0;

	/// Disable render device clip plane. 
	virtual void Disable(ClipPlaneName plane)=0;

	/// Set Color blending mode.
	virtual void BlendFunc(BlendMode sfactor,BlendMode dfactor)=0;

	/// Set Alpha function mode.
	virtual void AlphaFunc(TestFlag func,float value)=0;

	// Set stencil function mode.
	virtual void StencilFunc(TestFlag func,unsigned int ref, 
						unsigned int mask)=0;

	// Set stencil operation mode.
	virtual void StencilOp(OperationMode sfail,OperationMode dpmode, 
						OperationMode dppass)=0;
	
	/// Set face culling mode.
	virtual void CullMode(CullFaceMode mode)=0;

	/// Set geometry filling mode.
	virtual void FillMode(PolygonMode mode)=0;

	/// Set depth bias.
	virtual void DepthBias(float scale,float offset)=0;

	/// Set current transformation matrix.
	virtual void MatrixMode(MatrixType mode)=0;

	/// Set transformation matrix.
	virtual void LoadMatrix(const float *v)=0;

	/// Multiply matrix with current transformation matrix.
	virtual void MultMatrix(const float *mat)=0;

	/// Set current transformation matrix to identity.
	virtual void Identity()=0;

	/// Store current transformation matrix state in the stack.
	virtual void PushMatrix()=0;

	/// Restore transformation matrix from the stack.
	virtual void PopMatrix()=0;

	/// Apply a translation transformation, to the transformation stack.
	virtual void Translate(float x,float y,float z)=0;

	/// Apply a scale transformation, to the transformation stack.
	virtual void Scale(float x,float y,float z)=0;

	/// Apply a rotation(radians) in one of the axis, to the transformation stack.
	virtual void Rotate(float angle,float x,float y,float z)=0;

	/// Apply a rotation with a quaterninon, to the transformation stack.
	virtual void Quaternion(float x,float y,float z,float w)=0;

	/// Apply perspective projection to current matrix.
	/// @param aspect Width/Height viewport relation.
	/// @param fov Camera field of view in degrees.
	/// @param znear Start rendering distance.
	/// @param zfar End rendering distance.
	virtual void Perspective(float aspect, float fov=36, float znear=0.2f,float zfar=1000.0f)=0;

	/// Apply ortho projection to current matrix.
	virtual void Ortho(float left,float right,float bottom, float top,float znear=0.0f,float zfar=1.0f)=0;

	/// Apply ortho 2D projection to current matrix.
	/// @param width Visualization volume width.
	/// @param height Visualization volume height.
	virtual void Ortho2D(float width,float height,float znear=-1.0f,float zfar=1.0f)=0;

	/// Set material properties.
	virtual void Material(MaterialProperty prop,const float *v)=0; 

	/// Set point sprite properties.
	virtual void PointSprite(PointSpriteProperty, const float &v) = 0;

	/// Set clip plane.
	/// @param id Clip plane identification.
	/// @param v Reference to an array(four floats) with plane data.
	virtual void ClipPlane(ClipPlaneName id, const float *v) = 0;

	/// Get matrix float attributes.
	virtual void GetProperty(MatrixType prop,float *v)=0;

	/// Get renderer device float properties.
	virtual void GetProperty(RenderProperties prop,float *v)=0;

	/// Get renderer device integer properties.
	virtual void GetProperty(RenderProperties prop,int *v)=0;

	/// Return available memory in the gpu in megabytes(MiB).
	virtual unsigned int GetAvailableMemory()=0;

	/// Texture loading from filename method.
	/// @param filename 2D "tex2d_something.png", 3D "tex3d_something_a.png;tex2d_something_b.ong;...", CubeMap "texCM_something.png"
	/// @param genMipmap Generate mipmap.
	/// @return Created texture reference.
	virtual Texture* LoadTexture(const std::string & filename, bool genMipmap = true)=0;

	/// Create a blank texture.
	/// @param render_target Texture can be used as a render target.
	/// @param format Texture pixel format
	/// @param depth 3D textures depth planes count.	
	/// @return Reference to texture.
	virtual Texture *CreateTexture(TextureType type,unsigned int width,
		unsigned int height, unsigned int depth = 0,Format format = FORMAT_RGBA_8B, bool render_target = true)=0;

	/// Create render target manager.
	virtual RTManager * CreateRTManager(unsigned int width,
		unsigned int height)=0;
    
	/// Dynamic Mesh creation.
	/// @param maxVertices
	/// @param vp
	/// @param maxTriangles
	virtual DynamicMesh * CreateDynamicMesh(unsigned int maxVertices,unsigned int maxTriangles)=0;

	/// Mesh creation from vertices and face data.
	/// @param vp Reference to the class with vertex data organization.
	/// @param vb Reference to the vertex buffer
	/// @param gfc Reference to the face subset id buffer per face.
	/// @param fc Reference to the face buffer, with vertices ID per face.
	/// @throws Core::Exception in case of mesh creation failure.
	virtual Mesh* CreateMesh(unsigned int vertices,VertexProfile vp,
		void *vb,unsigned int groups, unsigned int *gfc, unsigned int **fb)=0;

	/// Shader loading from filename.
	/// @param global Reference to global shader variables manager.
	/// @throws Core::Exception if problems were encountered in the source code.	
	virtual Program *LoadProgram(const std::string & filename,
						ShaderModelType model = SHADER_MODEL_2, Program *global = NULL)=0;

	/// Shader loading from text.
	/// @param text Shader program text, if the first line is "global", the returned shader 
	/// is a global shader variable manager.
	/// @param global Reference to global shader variables manager.
	/// @throws Core::Exception if problems were encountered in the source code.
	virtual Program *LoadProgramFromText(const std::string & text,
						ShaderModelType model = SHADER_MODEL_2, Program *global = NULL)=0;
	
	/// Create a occlusion query object. 
	virtual OcclusionQuery *CreateOcclusionQuery()=0;

	/// Capture device color buffer to an image.
	virtual bool Capture(int x, int y, int width, int height, Format colorFormat, unsigned char * buffer) = 0;

	/// Check if textures loaded from file changed.
    virtual int ReloadTextures(int * reloaded, std::list<std::string> * errors) = 0;

	/// Check if shader programs loaded from file changed.
    virtual int ReloadPrograms(int * reloaded, std::list<std::string> * errors) = 0;
};

/// Available graphic devices types.
enum DeviceType
{
	DEVICE_AUTO,
	DEVICE_DIRECTX,
	DEVICE_OPENGL,
};

/*
* Function to create the graphics renderer device.
* @param wnd Reference to OS window.
* @param type Graphic device type.
* @param width Rendering screen width.
* @param height Rendering screen height.
* @param antialiasing Number of antialiasing samples.
* @param fullscreen Enable fullscreen mode.
*/
Device* CreateDevice(Core::Window *wnd,
		DeviceType type,
		unsigned int width, unsigned int height,
		unsigned int antialiasing = 0,
		bool fullscreen = false);

_GRAPHICS_END

#endif // #ifndef NCK_GRAPHICS_H
