
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckGraphics_gl2.h"
#include "nckException.h"
#include "nckMat44.h"
#include "nckTweaks_gl2.h"
#include <algorithm>
#include <stdio.h>

_GRAPHICS_BEGIN

#if defined(NCK_OPENGL2)

Device_GL2::Device_GL2(Core::Window * wnd,
		unsigned int width, unsigned int height,
		unsigned int antialiasing,bool fullscreen)
{
	m_InsideFBO = false;
	m_DestructorFlag = false;
	m_Width = width;
	m_Height = height;
	m_AntiAliasingAmount = antialiasing;
	m_ActiveProgram = NULL;
	m_Window = wnd;
	m_DepthBufferBits = 24;
	m_CullFace_Flag = false;
	m_SpecularPower = 1.0;
	m_MatrixMode = MATRIX_MODEL;
	m_CullFace_Flip = false;
	m_CullFace_Mode = CULL_BACK;
	m_ActiveVertex = 0;
	m_PrimitiveType = PRIMITIVE_QUADS;
    m_IsModelMatrixActive = false;

    m_Alpha = 1.0;

    m_DiffuseColor[0] = 1.0;
    m_DiffuseColor[1] = 1.0;
    m_DiffuseColor[2] = 1.0;
    m_DiffuseColor[3] = 1.0;

    m_SpecularColor[0] = 1.0;
    m_SpecularColor[1] = 1.0;
    m_SpecularColor[2] = 1.0;
    m_SpecularColor[3] = 1.0;

	m_AmbientColor[0] = 0.0;
	m_AmbientColor[1] = 0.0;
	m_AmbientColor[2] = 0.0;
	m_AmbientColor[3] = 0.0;

	m_ClearFlags = GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT;

    m_SamplersPerTarget.insert(std::pair<GLuint, GLuint>(GL_TEXTURE_1D, 0));
    m_SamplersPerTarget.insert(std::pair<GLuint, GLuint>(GL_TEXTURE_2D, 0));
    m_SamplersPerTarget.insert(std::pair<GLuint, GLuint>(GL_TEXTURE_3D, 0));
    m_SamplersPerTarget.insert(std::pair<GLuint, GLuint>(GL_TEXTURE_CUBE_MAP, 0));

#if defined(NCK_WINDOWS)
    
	GLuint pxFormat;
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;

	pfd.cColorBits = 24;
	pfd.cDepthBits = m_DepthBufferBits;
	pfd.cStencilBits = 8;

	Core::Window_Win32 * tmp_Wnd = dynamic_cast<Core::Window_Win32*>(wnd);
	m_WindowHandle = tmp_Wnd->m_Handle;

	m_DrawingContext = GetDC( m_WindowHandle );
	pxFormat = ChoosePixelFormat( m_DrawingContext, &pfd );
	SetPixelFormat( m_DrawingContext, pxFormat, &pfd);
	m_RenderingContext = wglCreateContext( m_DrawingContext );

	wglMakeCurrent(m_DrawingContext, m_RenderingContext);

	nckInitExtensionsGL2();

#ifdef GL_SWAP_INTERVAL
	wglSwapIntervalExt(GL_SWAP_INTERVAL);
#endif

#elif defined(NCK_LINUX)

	Core::Window_Linux * w = (Core::Window_Linux*)wnd;
	
	XLockDisplay(w->m_Display);
	int errorBase;
	int eventBase;

	if( !glXQueryExtension( w->m_Display, &errorBase, &eventBase ) ){
		XUnlockDisplay(w->m_Display);
		THROW_EXCEPTION("X server has no OpenGL GLX extension");
	}

	int doubleBufferVisual[]  =
	{
		GLX_RGBA,
		GLX_DEPTH_SIZE, m_DepthBufferBits,
		GLX_STENCIL_SIZE,8,
		GLX_DOUBLEBUFFER,
		None
	};

	int singleBufferVisual[] =
	{
		GLX_RGBA,
		GLX_DEPTH_SIZE, m_DepthBufferBits,
		None
	};

	XVisualInfo * visualInfo = glXChooseVisual( w->m_Display, DefaultScreen(w->m_Display), doubleBufferVisual );

	if( visualInfo == NULL )
	{
		visualInfo = glXChooseVisual(w->m_Display, DefaultScreen(w->m_Display), singleBufferVisual );

		if( visualInfo == NULL )
		{
			XUnlockDisplay(w->m_Display);
			THROW_EXCEPTION("No RGB visual with depth buffer");
			exit(1);
		}
	}

	m_Context = glXCreateContext(w->m_Display,
								   visualInfo,
								   NULL,      // No sharing of display lists
								   GL_TRUE );

	if(m_Context == NULL )
	{
		XUnlockDisplay(w->m_Display);
		THROW_EXCEPTION("Could not create rendering context");
		exit(1);
	}

	glXMakeCurrent( w->m_Display, w->m_Window, m_Context );

	nckInitExtensionsGL2();

	XUnlockDisplay(w->m_Display);

	glXSwapInterval(1);
    
#elif defined(NCK_MACOSX)
    
    GLCreate(wnd,m_DepthBufferBits);
    
#endif
        
}

Device_GL2::~Device_GL2()
{
	m_DestructorFlag = true;

#ifdef NCK_GRAPH_RES_PROXY
	while(m_TextureProxys.size()){
		ResourceProxy<Texture_GL2> * first = m_TextureProxys.begin()->second;
		delete first;
		m_TextureProxys.erase(m_TextureProxys.begin());
	}

	while(m_ProgramProxys.size()){
		ResourceProxy<Program_GL2> * first = m_ProgramProxys.begin()->second;
		delete first;
		m_ProgramProxys.erase(m_ProgramProxys.begin());
	}
#endif

#if defined(NCK_WINDOWS)
	if( m_RenderingContext != NULL )
	{
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( m_RenderingContext );
		m_RenderingContext = NULL;
	}

	if( m_DrawingContext != NULL )
	{
		ReleaseDC( m_WindowHandle, m_DrawingContext );
		m_DrawingContext = NULL;
	}
#elif defined(NCK_LINUX)
	Core::Window_Linux * w = (Core::Window_Linux*)m_Window;
	XLockDisplay(w->m_Display);
	glXDestroyContext(w->m_Display,m_Context);
	XUnlockDisplay(w->m_Display);
#elif defined(NCK_MACOSX)
    GLDestroy(m_Window);
#endif
}

void Device_GL2::Viewport(unsigned int x,unsigned int y, unsigned int w,
	unsigned int h){
#if defined(NCK_MACOSX)
    GLLock(m_Window);
#endif
	if(m_InsideFBO)
		glViewport(0,0,w,h);
	else
		glViewport(x,m_Window->GetHeight()-h-y,w,h);
}


void Device_GL2::Clear(){
    m_ModelMatrixStack.clear();
    m_IsModelViewClear = false;
#if defined(NCK_WINDOWS)
	glClear(m_ClearFlags);
#elif defined(NCK_LINUX)
	Core::Window_Linux * w = (Core::Window_Linux*)m_Window;
	XLockDisplay(w->m_Display);
	glClear(m_ClearFlags);
	XUnlockDisplay(w->m_Display);
#elif defined(NCK_MACOSX)
    GLLock(m_Window);
    glClear(m_ClearFlags);
#endif

    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_3D);
    glDisable(GL_TEXTURE_CUBE_MAP);
}

void Device_GL2::ClearFlags(unsigned int flags){
	m_ClearFlags = 0;
	if(flags & BUFFER_DEPTH_BIT)
		m_ClearFlags |= GL_DEPTH_BUFFER_BIT;
	if(flags & BUFFER_COLOR_BIT)
		m_ClearFlags |= GL_COLOR_BUFFER_BIT;
	if(flags & BUFFER_STENCIL_BIT)
		m_ClearFlags |= GL_STENCIL_BUFFER_BIT;
}

void Device_GL2::ClearColor(float r,float g,float b,float a){
	glClearColor(r,g,b,a);
}

void Device_GL2::PresentAll(){
    if (m_ActiveProgram)
        m_ActiveProgram->DisableInternal();
    m_ActiveProgram = NULL;

    m_TextureCache.Clear();
    
    m_SamplersPerTarget.insert(std::pair<GLuint, GLuint>(GL_TEXTURE_1D, 0));
    m_SamplersPerTarget.insert(std::pair<GLuint, GLuint>(GL_TEXTURE_2D, 0));
    m_SamplersPerTarget.insert(std::pair<GLuint, GLuint>(GL_TEXTURE_3D, 0));
    m_SamplersPerTarget.insert(std::pair<GLuint, GLuint>(GL_TEXTURE_CUBE_MAP, 0));

    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_3D);
    glDisable(GL_TEXTURE_CUBE_MAP);

#if defined(NCK_WINDOWS)
	SwapBuffers(m_DrawingContext);
#elif defined(NCK_LINUX)
	Core::Window_Linux * w = (Core::Window_Linux*)m_Window;
	XLockDisplay(w->m_Display);
	glXSwapBuffers(w->m_Display,w->m_Window);
	XUnlockDisplay(w->m_Display);
#elif defined(NCK_MACOSX)
    GLUnlock(m_Window);
#endif
}

void Device_GL2::Begin(PrimitiveType type){
	GLenum target;

	if(m_ActiveProgram){
		if(m_ActiveProgram->m_ToRemove){
			m_ActiveProgram->DisableInternal();
		}
		else
			m_ActiveProgram->EnableInternal();
	}

	m_TextureCache.Check();
	switch(type)
	{
	case PRIMITIVE_LINES:
		target = GL_LINES;
		break;
	case PRIMITIVE_TRIANGLES:
		target = GL_TRIANGLES;
		break;
	case PRIMITIVE_QUADS:
		target = GL_QUADS;
		break;
	case PRIMITIVE_POINTS:
		target = GL_POINTS;
		break;
	}
	glBegin(target);
}

void Device_GL2::End(){
	glEnd();
}

void Device_GL2::Vertex(float x,float y,float z){
	glVertex3f(x,y,z);
}

void Device_GL2::TexCoord(float x,float y){
	glTexCoord2f(x,y);
}

void Device_GL2::Color(unsigned char r,unsigned char g,unsigned char b,
	unsigned char a){
	glColor4ub(r,g,b,a);
}

void Device_GL2::Normal(float x,float y,float z){
	glNormal3f(x,y,z);
}

void Device_GL2::Enable(RenderState cap){
	switch(cap)
	{
	case STATE_BLEND:
		glEnable(GL_BLEND);
		break;
	case STATE_DEPTH_TEST:
		glEnable(GL_DEPTH_TEST);
		break;
	case STATE_ALPHA_TEST:
		glEnable(GL_ALPHA_TEST);
		break;
	case STATE_CULL_FACE:
		glEnable(GL_CULL_FACE);
		break;
	case STATE_FOG:
		glEnable(GL_FOG);
		break;
	case STATE_ZBUFFER_WRITE:
		glDepthMask(true);
		break;
	case STATE_COLOR_WRITE:
		glColorMask(true,true,true,true);
		break;
	case STATE_FLIP_CULL_FACE:
		m_CullFace_Flip = true;
		break;
	case STATE_STENCIL_TEST:
		glEnable(GL_STENCIL_TEST);
		break;
	case STATE_POINT_SPRITES:
		glEnable(GL_POINT_SPRITE);
		break;
	case STATE_POINT_SPRITES_SCALE:
		break;
	}
}

void Device_GL2::Enable(ClipPlaneName plane){

}

void Device_GL2::Disable(RenderState cap){
	switch(cap)
		{
		case STATE_BLEND:
			glDisable(GL_BLEND);
			break;
		case STATE_DEPTH_TEST:
			glDisable(GL_DEPTH_TEST);
			break;
		case STATE_ALPHA_TEST:
			glDisable(GL_ALPHA_TEST);
			break;
		case STATE_CULL_FACE:
			glDisable(GL_CULL_FACE);
			break;
		case STATE_FOG:
			glDisable(GL_FOG);
			break;
		case STATE_ZBUFFER_WRITE:
			glDepthMask(false);
			break;
		case STATE_COLOR_WRITE:
			glColorMask(0,0,0,0);
			break;
		case STATE_FLIP_CULL_FACE:
			m_CullFace_Flip = false;
			break;
		case STATE_STENCIL_TEST:
			glDisable(GL_STENCIL_TEST);
			break;
		case STATE_POINT_SPRITES:
			glDisable(GL_POINT_SPRITE);
			break;
		case STATE_POINT_SPRITES_SCALE:

			break;
		}
}

void Device_GL2::Disable(ClipPlaneName plane){

}

GLenum Device_GL2::ConvertFromBlendMode(BlendMode blend){
	switch(blend){
	case BLEND_ZERO:
		return GL_ZERO;
	case BLEND_ONE:
		return GL_ONE;
	case BLEND_SRC_COLOR:
		return GL_SRC_COLOR;
	case BLEND_DST_COLOR:
		return GL_DST_COLOR;
	case BLEND_INV_SRC_COLOR:
		return GL_ONE_MINUS_SRC_COLOR;
	case BLEND_INV_DST_COLOR:
		return GL_ONE_MINUS_DST_COLOR;
	case BLEND_SRC_ALPHA:
		return GL_SRC_ALPHA;
	case BLEND_INV_SRC_ALPHA:
		return GL_ONE_MINUS_SRC_ALPHA;
	case BLEND_DST_ALPHA:
		return GL_DST_ALPHA;
	case BLEND_INV_DST_ALPHA:
		return GL_ONE_MINUS_DST_ALPHA;
	default:
		return GL_ZERO;
	}
}

void Device_GL2::BlendFunc(BlendMode sfactor,BlendMode dfactor){
	glBlendFunc(ConvertFromBlendMode(sfactor),ConvertFromBlendMode(dfactor));
}

void Device_GL2::AlphaFunc(TestFlag func,float value){
	GLenum afunc;
	switch(func)
	{
	case TEST_ALWAYS:
		afunc = GL_ALWAYS;
		break;
	case TEST_EQUAL:
		afunc = GL_EQUAL;
		break;
	case TEST_GREATER:
		afunc = GL_GREATER;
		break;
	case TEST_GREATEREQUAL:
		afunc = GL_GEQUAL;
		break;
	case TEST_LESS:
		afunc = GL_LESS;
		break;
	case TEST_LESSEQUAL:
		afunc = GL_LEQUAL;
		break;
	case TEST_NEVER:
		afunc = GL_NEVER;
		break;
	case TEST_NOTEQUAL:
		afunc = GL_NOTEQUAL;
		break;
	}

	glAlphaFunc(afunc,value);
}

void Device_GL2::StencilFunc(TestFlag func,unsigned int ref,unsigned int
	mask){
	GLenum afunc;
	switch(func)
	{
	case TEST_ALWAYS:
		afunc = GL_ALWAYS;
		break;
	case TEST_EQUAL:
		afunc = GL_EQUAL;
		break;
	case TEST_GREATER:
		afunc = GL_GREATER;
		break;
	case TEST_GREATEREQUAL:
		afunc = GL_GEQUAL;
		break;
	case TEST_LESS:
		afunc = GL_LESS;
		break;
	case TEST_LESSEQUAL:
		afunc = GL_LEQUAL;
		break;
	case TEST_NEVER:
		afunc = GL_NEVER;
		break;
	case TEST_NOTEQUAL:
		afunc = GL_NOTEQUAL;
		break;
	}

	glStencilFunc(afunc,ref,mask);
}

void Device_GL2::StencilOp(OperationMode sfail,OperationMode dpmode
	,OperationMode dppass){
	GLenum fail,mode,pass;

	switch(sfail)
	{
	case OPERATION_REPLACE:
		fail = GL_REPLACE;
		break;
	case OPERATION_DECREASE:
		fail = GL_DECR;
		break;
	case OPERATION_INCREASE:
		fail = GL_INCR;
		break;
	case OPERATION_KEEP:
		fail = GL_KEEP;
		break;
	}

	switch(dpmode)
	{
	case OPERATION_REPLACE:
		mode = GL_REPLACE;
		break;
	case OPERATION_DECREASE:
		mode = GL_DECR;
		break;
	case OPERATION_INCREASE:
		mode = GL_INCR;
		break;
	case OPERATION_KEEP:
		mode = GL_KEEP;
		break;
	}

	switch(dppass)
	{
	case OPERATION_REPLACE:
		pass = GL_REPLACE;
		break;
	case OPERATION_DECREASE:
		pass = GL_DECR;
		break;
	case OPERATION_INCREASE:
		pass = GL_INCR;
		break;
	case OPERATION_KEEP:
		pass = GL_KEEP;
		break;
	}

	glStencilOp(fail,mode,pass);
}


void Device_GL2::CullMode(CullFaceMode mode){
    if (m_CullFace_Flip) {
        if (mode == CULL_FRONT)
            glCullFace(GL_BACK);
        else if (mode == CULL_BACK)
            glCullFace(GL_FRONT);
    }
    else {
        if (mode == CULL_FRONT)
            glCullFace(GL_FRONT);
        else if (mode == CULL_BACK)
            glCullFace(GL_BACK);
    }
}

void Device_GL2::FillMode(PolygonMode mode){
	if(mode == FILL_SOLID)
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	else if(mode == FILL_WIREFRAME)
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
}

void Device_GL2::DepthBias(float scale,float offset){
	glPolygonOffset(scale,offset);
}

void Device_GL2::MatrixMode(MatrixType mode){
	GLenum mMode = GL_MODELVIEW;
    m_CurrentMatrix = mode;
    m_IsModelMatrixActive = false;
	switch(mode)
	{
	case MATRIX_MODEL:
		mMode = GL_MODELVIEW;
        m_IsModelMatrixActive = true;
		break;
	case MATRIX_PROJECTION:
		mMode = GL_PROJECTION;
		break;
	case MATRIX_VIEW:
        mMode = GL_MODELVIEW;
		break;
	}
	glMatrixMode(mMode);
}

void Device_GL2::LoadMatrix(const float *v){
	glLoadMatrixf(v);
    if (m_IsModelMatrixActive) {
        m_ModelMatrix = Math::Mat44(v);
    }
}

void Device_GL2::MultMatrix(const float *mat){
	glMultMatrixf(mat);
    if (m_IsModelMatrixActive) {
        m_ModelMatrix *= Math::Mat44(mat);
    }
}

void Device_GL2::Identity(){
    if (m_IsModelMatrixActive) {
        if(!m_IsModelViewClear)
            glLoadIdentity();

        m_ModelMatrixStack.clear();
        m_ModelMatrix = Math::Identity();
    }
    else {
        if (m_CurrentMatrix == MATRIX_VIEW)
            m_IsModelViewClear = true;

        glLoadIdentity();
    }
}

void Device_GL2::PushMatrix(){
    if (m_IsModelMatrixActive) {
        m_ModelMatrixStack.push_front(m_ModelMatrix);
    }
	glPushMatrix();
}

void Device_GL2::PopMatrix(){
    if (m_IsModelMatrixActive && !m_ModelMatrixStack.empty()) {
        m_ModelMatrix = *m_ModelMatrixStack.begin();
        m_ModelMatrixStack.pop_front();
    }
	glPopMatrix();
}

void Device_GL2::Translate(float x,float y,float z){
	glTranslatef(x,y,z);
}

void Device_GL2::Scale(float x,float y,float z){
	glScalef(x,y,z);
}

void Device_GL2::Rotate(float angle,float x,float y,float z){
	glRotatef(angle,x,y,z);
}

void Device_GL2::Quaternion(float x,float y,float z,float w){

	// 3D Math Primer For Graphics & Game Development
	// Pages 185-187

	float mat[16];
	mat[0] = 1.0f - 2.0f * ( y * y + z * z );
	mat[1] = 2.0f * (x * y + z * w);
	mat[2] = 2.0f * (x * z - y * w);
	mat[3] = 0.0f;

	mat[4] = 2.0f * ( x * y - z * w );
	mat[5] = 1.0f - 2.0f * ( x * x + z * z );
	mat[6] = 2.0f * (z * y + x * w );
	mat[7] = 0.0f;

	mat[8] = 2.0f * ( x * z + y * w );
	mat[9] = 2.0f * ( y * z - x * w );
	mat[10] = 1.0f - 2.0f * ( x * x + y * y );
	mat[11] = 0.0f;

	mat[12] = 0;
	mat[13] = 0;
	mat[14] = 0;
	mat[15] = 1.0f;

	glMultMatrixf(mat);
}

void Device_GL2::Perspective(float aspect, float fov, float znear,float zfar){
	gluPerspective(fov,aspect,znear,zfar);
	if(m_InsideFBO)
		glScalef(1,-1,1);
}

void Device_GL2::Ortho(float left, float right, float bottom, float top, float znear,float zfar){
	glOrtho(left,right,bottom,top,znear,zfar); 	
	if(m_InsideFBO){
		glScalef(1,-1,1);
		glTranslatef(0,-(bottom-top),0); 
	}
}


void Device_GL2::Ortho2D(float width,float height,float znear, float zfar){
	glOrtho(0,width,height,0,znear,zfar);
	if(m_InsideFBO){
		// This flips the content in the y axis to make rendering coherent with directx
		glScalef(1,-1,1); // wnd.top = -height and wnd.bottom = 0
		glTranslatef(0,-height,0); // wnd.top = 0 and wnd.bottom = height
	}
}


void Device_GL2::Material(MaterialProperty prop,const float *v){
	switch(prop)
	{
	case MATERIAL_DIFFUSE_COLOR:
		m_DiffuseColor[0] = v[0];
		m_DiffuseColor[1] = v[1];
		m_DiffuseColor[2] = v[2];
		m_DiffuseColor[3] = v[3];
		break;
	case MATERIAL_SPECULAR_COLOR:
		m_SpecularColor[0] = v[0];
		m_SpecularColor[1] = v[1];
		m_SpecularColor[2] = v[2];
        m_SpecularColor[3] = v[3];
		break;
	case MATERIAL_AMBIENT_COLOR:
		m_AmbientColor[0] = v[0];
		m_AmbientColor[1] = v[1];
		m_AmbientColor[2] = v[2];
		break;
	case MATERIAL_SHININESS:
		m_SpecularPower = v[0];
		break;
	case MATERIAL_AMBIENT:
		m_AmbientColor[3] = v[0];
        break;
    case MATERIAL_ALPHA:
        m_Alpha = v[0];
		break;
	}
}

void Device_GL2::PointSprite(PointSpriteProperty prop, const float &v){
	switch(prop){
	case POINT_MAX_SIZE:
		break;
	case POINT_MIN_SIZE:
		break;
	case POINT_SIZE:
		break;
	}
}

void Device_GL2::ClipPlane(ClipPlaneName id, const float *v){

}

void Device_GL2::GetProperty(MatrixType prop,float *v){

}

void Device_GL2::GetProperty(RenderProperties prop,float *v){

}

void Device_GL2::GetProperty(RenderProperties prop,int *v){

}

unsigned int Device_GL2::GetAvailableMemory(){
	return 0;
}

Texture * Device_GL2::LoadTexture(const std::string & filename, bool genMipmap)
{
	std::string fn = filename; 
	std::transform(fn.begin(), fn.end(), fn.begin(), ::tolower);

	if(fn.find("texcm") != std::string::npos)
	{
		return (TextureCubeMap*)TextureCubemap_GL2::Load(this,filename,genMipmap);
	}
	else if(fn.find("tex3d") != std::string::npos)
	{
		return (Texture3D*)Texture3D_GL2::Load(this,filename,genMipmap);
	}
	else 
	{
		return (Texture2D*)Texture2D_GL2::Load(this,filename,genMipmap);
	}
}


Texture * Device_GL2::CreateTexture(TextureType type,unsigned int width,
	unsigned int height,unsigned int depth,Format format, bool render_target)
{
	Texture2D_GL2 * tex = new Texture2D_GL2(this);
	tex->Create(width,height,format,false);
	return (Texture2D*)tex;
}

RTManager * Device_GL2::CreateRTManager(unsigned int width,
	unsigned int height){
	RTManager_GL2 * manager = new RTManager_GL2(this);
	manager->Load(width,height);
	return manager;
}

Mesh * Device_GL2::CreateMesh(unsigned int vertices,VertexProfile vp,
	void *vb,unsigned int groups, unsigned int *gfc, unsigned int **fb){
	Mesh_GL2 * mesh = new Mesh_GL2(this);
	mesh->Load((uint32_t)vertices,vp,(void*)vb,(uint32_t)groups,(uint32_t*)gfc,(uint32_t**)fb);
	return mesh;
}

DynamicMesh * Device_GL2::CreateDynamicMesh(unsigned int maxVertices, unsigned int maxTriangles){
	DynamicMesh_GL2 * dm = new DynamicMesh_GL2(this);
	dm->Create(maxVertices,maxTriangles);
	return dm;
}

Program * Device_GL2::LoadProgram(const std::string & filename, ShaderModelType model, Program *global){
	Program_GL2 * prog = new Program_GL2(this);

	try{
		prog->LoadFromFilename(filename);
	}
	catch(Core::Exception & e){
		delete prog;
		THROW_EXCEPTION_STACK("Load program exception",e);
	}

	return prog;
}

Program * Device_GL2::LoadProgramFromText(const std::string & text, ShaderModelType model, Program *global){
	Program_GL2 * prog = new Program_GL2(this);

	try{
		prog->Load(text);
	}
	catch(Core::Exception & e){
		delete prog;
		THROW_EXCEPTION_STACK("Load program exception",e);
	}

	return prog;
}

OcclusionQuery * Device_GL2::CreateOcclusionQuery(){
	return NULL;
}

#ifdef NCK_GRAPH_RES_PROXY

ResourceProxy<Texture_GL2> * Device_GL2::GetTextureProxy(const std::string & fileName){
	std::map<std::string,ResourceProxy<Texture_GL2>*>::iterator i = m_TextureProxys.find(fileName);
	if(i != m_TextureProxys.end()){
		ResourceProxy<Texture_GL2> * manager = i->second;
		return manager;
	}
	return NULL;
}

ResourceProxy<Texture_GL2> * Device_GL2::AddTextureProxy(const std::string & fileName,Texture_GL2 * tex){
	std::map<std::string,ResourceProxy<Texture_GL2>*>::iterator i = m_TextureProxys.find(fileName);
	ResourceProxy<Texture_GL2> * manager = NULL;

	if(i == m_TextureProxys.end()){
		manager = new ResourceProxy<Texture_GL2>(fileName);
		m_TextureProxys.insert(std::pair<std::string,ResourceProxy<Texture_GL2>*>(fileName,manager));
	}
	else
		manager = i->second;

	manager->Add(tex);

	return manager;
}

bool Device_GL2::RemoveTextureProxy(Texture_GL2 * tex){
	if(m_DestructorFlag)
		return false;

	ResourceProxy<Texture_GL2> * mng = tex->GetProxyManager();

	// Unmanaged resource -> delete
	if(!mng) 
		return true;

	mng->Remove(tex);

	if(mng->GetSize() == 0){
		m_TextureProxys.erase(mng->GetResourceName());
		delete mng;
		return true;
	}

	return false;
}

ResourceProxy<Program_GL2> * Device_GL2::GetProgramProxy(const std::string & fileName){
	std::map<std::string,ResourceProxy<Program_GL2>*>::iterator i = m_ProgramProxys.find(fileName);
	if(i != m_ProgramProxys.end()){
		ResourceProxy<Program_GL2> * manager = i->second;
		return manager;
	}
	return NULL;
}

ResourceProxy<Program_GL2> * Device_GL2::AddProgramProxy(const std::string & fileName,Program_GL2 * prog){
	std::map<std::string,ResourceProxy<Program_GL2>*>::iterator i = m_ProgramProxys.find(fileName);
	ResourceProxy<Program_GL2> * manager = NULL;

	if(i == m_ProgramProxys.end()){
		manager = new ResourceProxy<Program_GL2>(fileName);
		m_ProgramProxys.insert(std::pair<std::string,ResourceProxy<Program_GL2>*>(fileName,manager));
	}
	else
		manager = i->second;

	manager->Add(prog);

	return manager;
}

bool Device_GL2::RemoveProgramProxy(Program_GL2 * prog)
{
	if(m_DestructorFlag)
		return false;

	ResourceProxy<Program_GL2> * mng = prog->GetProxyManager();
	
	//  Unmanaged resource -> delete
	if(!mng) 
		return true;

	mng->Remove(prog);

	if(mng->GetSize() == 0){
		m_ProgramProxys.erase(mng->GetResourceName());
		delete mng;
		return true;
	}

	return false;
}

#endif


bool Device_GL2::Capture(int x, int y, int width, int height, Format colorFormat, unsigned char * buffer){
	GLenum format;
	GLenum type;
	switch(colorFormat){
		case FORMAT_RGB_8B:
			format = GL_RGB;
			type = GL_UNSIGNED_BYTE;
			break;
		case FORMAT_RGBA_8B:
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
		case FORMAT_RGBA_16F:
			format = GL_RGBA;
			type = GL_HALF_FLOAT;
			break;
		case FORMAT_RGBA_32F:
			format = GL_RGBA;
			type = GL_FLOAT;
			break;
		case FORMAT_A_8B:
			format = GL_ALPHA;
			type = GL_UNSIGNED_BYTE;
			break;
		case FORMAT_R_16F:
			format = GL_RED;
			type = GL_HALF_FLOAT;
			break;
		case FORMAT_R_32F:
			format = GL_RED;
			type = GL_FLOAT;
			break;
	};
	glReadPixels(x,y,width,height,format,type,buffer);
	return true;
}

int Device_GL2::ReloadPrograms(int * reloaded, std::list<std::string> * errors) {
    int total = 0;
    MapFor(std::string, ResourceProxy<Program_GL2>*, m_ProgramProxys, i) {
        try {
            if(i->second->GetReference()->Reload())
                (*reloaded)++;
        }
        catch (const Core::Exception & e) {
            errors->push_back(e.ToString());
        }
        total++;
      
    }
    return total;
}

int Device_GL2::ReloadTextures(int * reloaded, std::list<std::string> * errors) {
    int total = 0;
    MapFor(std::string, ResourceProxy<Texture_GL2>*, m_TextureProxys, i) {
        if (i->second->GetReference()->GetType() == Graph::TextureType::TEXTURE_2D) {
            try {
                if (dynamic_cast<Texture2D_GL2*>(i->second->GetReference())->Reload()) 
                    (*reloaded)++;
            }
            catch (const Core::Exception & e) {
                errors->push_back(e.ToString());
            }
            total++;
        }
    }
    return total;
}

void Device_GL2::UpdateMatrix(){

}


Device_GL2 *CreateDevice_GL2(Core::Window *wnd,
		unsigned int width, unsigned int height, unsigned int antialiasing,bool fullscreen){
	return new Device_GL2(wnd,width,height,antialiasing,fullscreen);
}

#endif // #if defined(NCK_OPENGL2)

_GRAPHICS_END
