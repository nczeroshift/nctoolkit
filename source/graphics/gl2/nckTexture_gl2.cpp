
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckGraphics_gl2.h"
#include "nckUtils.h"
#include "nckException.h"
#include "nckDataIO.h"

#include <map>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef GL_RGBA16F
#define GL_RGBA16F 0x881A
#endif

#ifndef GL_RGBA32F
#define GL_RGBA32F 0x8814
#endif

/**
 * Convert a 2D picture (in blender cubemap arrangement format) to cubemap 2D planes
 */
unsigned char** Convert2DToCubemapPlanes(unsigned char *image_data, int width, int height, int depth)
{
	unsigned int i;
	unsigned int cbdimension = height>>1;

	unsigned char  **data = new unsigned char *[6];

	// Check if its a blender cubemap image
	if(cbdimension*3 != width)
	{
		delete [] data;
		return NULL;
	}

	for(i = 0;i<6;i++)
	{
		data[i] = new unsigned char[cbdimension*cbdimension*depth];
	}

	for(i = 0;i<cbdimension;i++)
	{
		// parte superior da imagem

		// -X
		memcpy(data[1]+(cbdimension-1-i)*cbdimension*depth,image_data+i*width*depth,cbdimension*depth);
		// -Y
		memcpy(data[3]+(cbdimension-1-i)*cbdimension*depth,image_data+cbdimension*depth+i*width*depth,cbdimension*depth);
		// +X
		memcpy(data[0]+(cbdimension-1-i)*cbdimension*depth,image_data+cbdimension*depth*2+i*width*depth,cbdimension*depth);

		//  parte inferior da imagem

		// -Z
		memcpy(data[5]+(cbdimension-1-i)*cbdimension*depth,image_data+(cbdimension+i)*width*depth,cbdimension*depth);
		// +Z
		memcpy(data[4]+(cbdimension-1-i)*cbdimension*depth,image_data+cbdimension*depth+(cbdimension+i)*width*depth,cbdimension*depth);
		// +Y
		memcpy(data[2]+(cbdimension-1-i)*cbdimension*depth,image_data+cbdimension*depth*2+(cbdimension+i)*width*depth,cbdimension*depth);

	}
	return data;
}

_GRAPHICS_BEGIN

#if defined(NCK_OPENGL2)

Texture_GL2::Texture_GL2(Device_GL2 *dev){
	m_Texture = GL_ZERO;
	m_MinFilter = GL_LINEAR;
	m_MagFilter = GL_LINEAR;
	m_Target = GL_TEXTURE_2D;
	m_MipFilter = GL_NEAREST;
	m_Format = FORMAT_RGBA_8B;
	m_Width = 0;
	m_Height = 0;
	m_WrapMode = GL_REPEAT;
	m_Device = dev;
    
#ifdef NCK_GRAPH_RES_PROXY
	m_Proxy = NULL;
#endif
    
}

Texture_GL2::~Texture_GL2(){

}

Format Texture_GL2::GetFormat(){
	return m_Format;
}

unsigned int Texture_GL2::GetWidth(){
	return m_Width;
}

unsigned int Texture_GL2::GetHeight(){
	return m_Height;
}

void Texture_GL2::SetFilter(FilterType type,FilterMode mode)
{
	if(type == FILTER_MINIFICATION)
		m_MinFilter = ConvertFilterMode(mode);
	else if(type == FILTER_MAGNIFICATION)
		m_MagFilter = ConvertFilterMode(mode);
	else if(type == FILTER_MIPMAPPING)
		m_MipFilter = ConvertFilterMode(mode);
}

void Texture_GL2::SetAdressMode(AdressMode mode)
{
	if(mode==ADRESS_CLAMP)
		m_WrapMode = GL_CLAMP;
	else
		m_WrapMode = GL_REPEAT;
}

void Texture_GL2::SetFilterAndWrapping()
{
	GLenum minFilter = m_MinFilter;
	GLenum magFilter = m_MagFilter;

	if(m_MipFilter==GL_LINEAR){
		if(m_MinFilter==GL_LINEAR)
			minFilter = GL_LINEAR_MIPMAP_LINEAR;
		else if(m_MinFilter==GL_NEAREST)
			minFilter = GL_NEAREST_MIPMAP_LINEAR;

		if(m_MagFilter==GL_LINEAR)
			magFilter = GL_LINEAR_MIPMAP_LINEAR;
		else if(m_MagFilter==GL_NEAREST)
			magFilter = GL_NEAREST_MIPMAP_LINEAR;
	}
	else if(m_MipFilter==GL_NEAREST){
		if(m_MinFilter==GL_LINEAR)
			minFilter = GL_LINEAR_MIPMAP_NEAREST;
		else if(m_MinFilter== GL_NEAREST)
			minFilter = GL_NEAREST_MIPMAP_NEAREST;

		if(m_MagFilter==GL_LINEAR)
			magFilter = GL_LINEAR_MIPMAP_NEAREST;
		else if(m_MagFilter== GL_NEAREST)
			magFilter = GL_NEAREST_MIPMAP_NEAREST;
	}

	glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER,minFilter );
	glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER,magFilter );

	glTexParameteri(m_Target, GL_TEXTURE_WRAP_S,m_WrapMode);
	glTexParameteri(m_Target, GL_TEXTURE_WRAP_T,m_WrapMode);
	glTexParameteri(m_Target, GL_TEXTURE_WRAP_R,GL_CLAMP);
}

GLenum Texture_GL2::ConvertFilterMode(FilterMode mode){
	if(mode == FILTER_NONE)
		return GL_NONE;
	else if(mode == FILTER_NEAREST)
		return GL_NEAREST;
	else
		return GL_LINEAR;
}

void Texture_GL2::GetGLPropertiesFromFormat(Format srcFormat, int * glComponents,
			GLenum *glInternalFormat, GLenum *glFormat, GLenum * glType)
{
	int components = 3;
	GLenum format = GL_RGB;
	GLenum datatype = GL_UNSIGNED_BYTE;
	GLenum intFormat = GL_RGB8;

	switch(srcFormat)
	{
	case FORMAT_A_8B:
		intFormat = GL_ALPHA8;
		format = GL_ALPHA;
		components = 1;
		datatype = GL_UNSIGNED_BYTE;
		break;
	case FORMAT_RGBA_16F:
		intFormat = GL_RGBA16F;
		format = GL_RGBA;
		components = 4;
		datatype = GL_HALF_FLOAT;
		break;
	case FORMAT_RGBA_32F:
		intFormat = GL_RGBA32F;
		format = GL_RGBA;
		components = 4;
		datatype = GL_FLOAT;
		break;
	case FORMAT_RGBA_8B:
		intFormat = GL_RGBA8;
		format = GL_RGBA;
		components = 4;
		datatype = GL_UNSIGNED_BYTE;
		break;
	case FORMAT_RGB_8B:
		intFormat = GL_RGB8;
		format = GL_RGB;
		components = 3;
		datatype = GL_UNSIGNED_BYTE;
		break;
	case FORMAT_R_16F:
		intFormat = GL_R16F;
		format = GL_RED;
		components = 1;
		datatype = GL_HALF_FLOAT;
		break;
	case FORMAT_R_32F:
		intFormat = GL_R32F;
        format = GL_RED;
		components = 1;
		datatype = GL_FLOAT;
		break;
	}
	*glInternalFormat = intFormat;
	*glComponents = components;
	*glFormat = format;
	*glType = datatype;
}

#ifdef NCK_GRAPH_RES_PROXY
ResourceProxy<Texture_GL2> * Texture_GL2::GetProxyManager(){
	return m_Proxy;
}
#endif 

Texture2D_GL2::Texture2D_GL2(Device_GL2 *dev) : Texture_GL2(dev){
	m_LockData = NULL;
	m_LockFlag = false;
	m_Target = GL_TEXTURE_2D;
    m_LastModified = 0;
}

Texture2D_GL2::~Texture2D_GL2(){
	m_Device->m_TextureCache.Clear();

	SafeDelete(m_LockData);

#ifdef NCK_GRAPH_RES_PROXY
	// Check if GPU resource can be released.
	if(!m_Device->RemoveTextureProxy(this))
		return;
#endif
	
	if(m_Texture)
		glDeleteTextures(1,&m_Texture);

	m_Texture = GL_ZERO;
}

TextureType Texture2D_GL2::GetType(){
	return TEXTURE_2D;
}

void Texture2D_GL2::Enable(unsigned int sampler_id)
{
	if(sampler_id >= 8)	return;

	if(m_Device->m_TextureCache.GetTexture(sampler_id) != (Texture2D*)this)
	{
		// Disable previous texture.
		if(m_Device->m_TextureCache.GetTexture(sampler_id)){
			m_Device->m_TextureCache.GetTexture(sampler_id)->Disable(sampler_id);
		}

        m_Device->m_SamplersPerTarget.find(m_Target)->second++;
       
		// Enable this texture.
		glEnable(m_Target);
		glActiveTexture(GL_TEXTURE0+sampler_id);
		glBindTexture(m_Target,m_Texture);
	}

	SetFilterAndWrapping();

	m_Device->m_TextureCache.SetTexture((Graph::Texture2D*)(this),true,sampler_id);
}

void Texture2D_GL2::Disable(unsigned int sampler_id)
{
	if(sampler_id >= 8)	return;

	// Unbind texture if it was scheduled to be disabled.
	if(!m_Device->m_TextureCache.GetTextureState(sampler_id))
	{
        GLuint count = m_Device->m_SamplersPerTarget.find(m_Target)->second;
        if (count > 0) count--;
        m_Device->m_SamplersPerTarget.find(m_Target)->second = count;

		glActiveTexture(GL_TEXTURE0+sampler_id);
		glBindTexture(m_Target,0);

        if (count == 0)
            glDisable(m_Target);

		// Remove from cache
		m_Device->m_TextureCache.SetTexture(NULL,false,sampler_id);
     
		return;
	}

	// Scheduled texture to be disabled.
	m_Device->m_TextureCache.SetTexture((Graph::Texture2D*)(this),false,sampler_id);
}

void *Texture2D_GL2::Lock(unsigned int level)
{
	glEnable(m_Target);
	glBindTexture(m_Target,m_Texture);

	int components = 3;
	GLenum format = GL_RGB;
	GLenum datatype = GL_UNSIGNED_BYTE;
	GLenum intFormat = GL_RGB8;

	GetGLPropertiesFromFormat(m_Format,&components,&intFormat,&format,&datatype);

	int dataSize = 1;
	switch(datatype)
	{
		case GL_UNSIGNED_BYTE:
			dataSize = 1;
			break;
		case GL_HALF_FLOAT:
			dataSize = 2;
			break;
		case GL_FLOAT:
			dataSize = 4;
			break;
	}


	if(m_LockFlag)
		THROW_EXCEPTION("Locked texture resource");

	SafeDelete(m_LockData);

	m_LockData = new GLubyte[m_Width*m_Height*components*dataSize];

	glGetTexImage(m_Target,level,format,datatype,m_LockData);

	m_LockFlag = true;
	return m_LockData;
}

void Texture2D_GL2::Unlock(unsigned int level){
	glEnable(m_Target);
	glBindTexture(m_Target,m_Texture);

	int components = 3;
	GLenum format = GL_RGB;
	GLenum datatype = GL_UNSIGNED_BYTE;
	GLenum intFormat = GL_RGB8;

	GetGLPropertiesFromFormat(m_Format,&components,&intFormat,&format,&datatype);

	glTexSubImage2D(m_Target,level,0,0,m_Width,m_Height,format,datatype,m_LockData);
	
	SafeDelete(m_LockData);
	m_LockFlag = false;
}


int Texture2D_GL2::Reload() {
    int64_t currentModified = Core::GetFileLastModified(m_Filename);

    if (m_LastModified == 0 || currentModified - m_LastModified < 10) {
        return 0;
    }

    Core::Image * img = NULL;
    try{
        img = Core::Image::Load(m_Filename);
    }
    catch (const Core::Exception & e) {
        THROW_EXCEPTION_STACK("Unable to load texture " + m_Filename, e);
    }

    int width = img->GetWidth();
    int height = img->GetHeight();

    Graph::Format imgFormat;
    if (img->GetFormat() == Core::PIXEL_FORMAT_ALPHA_8B)
        imgFormat = Graph::FORMAT_A_8B;
    else if (img->GetFormat() == Core::PIXEL_FORMAT_RGBA_8B)
        imgFormat = Graph::FORMAT_RGBA_8B;
    else if (img->GetFormat() == Core::PIXEL_FORMAT_RGB_8B)
        imgFormat = Graph::FORMAT_RGB_8B;

    GLuint target;
    glEnable(this->m_Target);
    glGenTextures(1, &target);
    glBindTexture(this->m_Target, target);

    int components = 3;
    GLenum format = GL_RGB;
    GLenum datatype = GL_UNSIGNED_BYTE;
    GLenum intFormat = GL_RGB8;

    GetGLPropertiesFromFormat(imgFormat, &components, &intFormat, &format, &datatype);

    if (m_Mipmap)
        glTexParameteri(target, GL_GENERATE_MIPMAP, GL_TRUE);
    else
        glTexParameteri(target, GL_GENERATE_MIPMAP, GL_FALSE);

    glTexImage2D(target, 0, intFormat, img->GetWidth(), img->GetHeight(), 0, format, datatype, img->GetData());
    
    glBindTexture(target, GL_ZERO);

    SafeDelete(img);

    m_LastModified = currentModified;

    return 1;
}

Texture2D_GL2 * Texture2D_GL2::Load(Device_GL2 *dev,const std::string & filename, bool mipmaps){
	Texture2D_GL2 * tex = new Texture2D_GL2(dev);

#ifdef NCK_GRAPH_RES_PROXY
	ResourceProxy<Texture_GL2> * proxy = NULL;
	Texture2D_GL2 * refTex = NULL;

	if((proxy = dev->GetTextureProxy(filename)) != NULL && (refTex = dynamic_cast<Texture2D_GL2*>(proxy->GetReference())) != NULL){
		tex->m_Width = refTex->m_Width;
		tex->m_Height = refTex->m_Height;
		tex->m_Format = refTex->m_Format;
		tex->m_Target = refTex->m_Target;
		tex->m_Texture = refTex->m_Texture;
		tex->m_Proxy = proxy;
		dev->AddTextureProxy(filename,tex);		
		return tex;
	}
#endif
	Core::Image * img = Core::Image::Load(filename);

	tex->m_Width = img->GetWidth();
	tex->m_Height = img->GetHeight();
	
	if(img->GetFormat() == Core::PIXEL_FORMAT_ALPHA_8B)
		tex->m_Format = Graph::FORMAT_A_8B;
	else if(img->GetFormat() == Core::PIXEL_FORMAT_RGBA_8B)
		tex->m_Format = Graph::FORMAT_RGBA_8B;
	else if(img->GetFormat() == Core::PIXEL_FORMAT_RGB_8B)
		tex->m_Format = Graph::FORMAT_RGB_8B;

	glEnable(tex->m_Target);
	glGenTextures(1,&tex->m_Texture);
	glBindTexture(tex->m_Target,tex->m_Texture);

	int components = 3;
	GLenum format = GL_RGB;
	GLenum datatype = GL_UNSIGNED_BYTE;
	GLenum intFormat = GL_RGB8;

	GetGLPropertiesFromFormat(tex->m_Format,&components,&intFormat,&format,&datatype);

	if(mipmaps)
		glTexParameteri(tex->m_Target, GL_GENERATE_MIPMAP, GL_TRUE);
	else
		glTexParameteri(tex->m_Target, GL_GENERATE_MIPMAP, GL_FALSE);

    tex->m_Mipmap = mipmaps;

	glTexImage2D(tex->m_Target,0,intFormat,tex->m_Width,tex->m_Height,0,format,datatype,img->GetData());

	glBindTexture(tex->m_Target,GL_ZERO);

	SafeDelete(img);
	
#ifdef NCK_GRAPH_RES_PROXY
	tex->m_Proxy = dev->AddTextureProxy(filename,tex);
#endif

	return tex;
}

GLuint Texture2D_GL2::getTextureId(){
	return m_Texture;
}

void Texture2D_GL2::Create(unsigned int width, unsigned int height, Format format, bool renderTarget)
{
	m_Width = width;
	m_Height = height;
	m_Format = format;

	glEnable(m_Target);
	glGenTextures(1,&m_Texture);
	glBindTexture(m_Target,m_Texture);

	int out_components = 3;
	GLenum out_datatype = GL_UNSIGNED_BYTE;
	GLenum out_format = GL_RGB;
	GLenum intFormat = GL_RGB8;

	GetGLPropertiesFromFormat(m_Format,&out_components,&intFormat,&out_format,&out_datatype);

	unsigned int datasize = 0;
	if(out_datatype==GL_UNSIGNED_BYTE)
		datasize = 1;
	else if(out_datatype==GL_HALF_FLOAT)
		datasize = 2;
	else if(out_datatype==GL_FLOAT)
		datasize = 4;

	m_MipFilter = GL_NONE;
	m_MinFilter = GL_LINEAR;
	m_MagFilter = GL_LINEAR;

	unsigned char * data = new unsigned char[out_components*datasize*width*height];

	memset(data,0x00,out_components*datasize*width*height);

	glTexImage2D(m_Target,0, intFormat,m_Width,m_Height,0, out_format,out_datatype,data);

	SafeDelete(data);
}

TextureCubemap_GL2::TextureCubemap_GL2(Device_GL2 *dev):Texture_GL2(dev){
	m_Target = GL_TEXTURE_CUBE_MAP;
}

TextureCubemap_GL2::~TextureCubemap_GL2(){
	m_Device->m_TextureCache.Clear();
	
#ifdef NCK_GRAPH_RES_PROXY
	// Check if GPU resource can be released.
	if(!m_Device->RemoveTextureProxy(this))
		return;
#endif

	if(m_Texture)
		glDeleteTextures(1,&m_Texture);

	m_Texture = GL_ZERO;
}

void TextureCubemap_GL2::Enable(unsigned int sampler_id){
	if(sampler_id >= 8)	return;
	
    if(m_Device->m_TextureCache.GetTexture(sampler_id) != (Texture2D*)this)
    {
        // Disable previous texture.
        if (m_Device->m_TextureCache.GetTexture(sampler_id)) {
            m_Device->m_TextureCache.GetTexture(sampler_id)->Disable(sampler_id);
        }

        m_Device->m_SamplersPerTarget.find(m_Target)->second++;
       
        glEnable(m_Target);
		glActiveTexture(GL_TEXTURE0+sampler_id);
		glBindTexture(m_Target,m_Texture);
	}
	SetFilterAndWrapping();
	m_Device->m_TextureCache.SetTexture((Texture2D*)this,true,sampler_id);
}

void TextureCubemap_GL2::Disable(unsigned int sampler_id){
	if(sampler_id >= 8)	return;
	
    if(!m_Device->m_TextureCache.GetTextureState(sampler_id))
    {
        if (m_Device->m_TextureCache.GetTexture(sampler_id)) {
            m_Device->m_TextureCache.GetTexture(sampler_id)->Disable(sampler_id);
        }

        GLuint count = m_Device->m_SamplersPerTarget.find(m_Target)->second;
        if (count > 0)
            count--;
        m_Device->m_SamplersPerTarget.find(m_Target)->second = count;

		glActiveTexture(GL_TEXTURE0+sampler_id);
		glBindTexture(m_Target,0);

        if (count == 0)
            glDisable(m_Target);
	}
	m_Device->m_TextureCache.SetTexture((Texture2D*)this,false,sampler_id);
}

TextureType TextureCubemap_GL2::GetType(){
	return TEXTURE_CUBEMAP;
}


void * TextureCubemap_GL2::Lock(unsigned int face,unsigned int level){
	return NULL;
}

void TextureCubemap_GL2::Unlock(unsigned int face,unsigned int level){

}

	
TextureCubemap_GL2 * TextureCubemap_GL2::Load(Device_GL2 * dev, const std::string & packageFilename, bool mipmaps)
{
#ifdef NCK_GRAPH_RES_PROXY
	// Check texture proxy cache
	ResourceProxy<Texture_GL2>  * proxy = NULL;
	TextureCubemap_GL2 * refTex = NULL;

	if((proxy = dev->GetTextureProxy(packageFilename)) != NULL && (refTex = dynamic_cast<TextureCubemap_GL2*>(proxy->GetReference())) != NULL)
	{
		TextureCubemap_GL2 * ret = new TextureCubemap_GL2(dev);
		ret->m_Width = refTex->m_Width;
		ret->m_Height = refTex->m_Height;
		ret->m_Format = refTex->m_Format;
		ret->m_Target = refTex->m_Target;
		ret->m_Texture = refTex->m_Texture;
		ret->m_Proxy = proxy;
		dev->AddTextureProxy(packageFilename,ret);		
		return ret;
	}
#endif
	// New resource, read from file
	Core::FileReader * reader = Core::FileReader::Open(packageFilename);

	if(!reader)
		THROW_EXCEPTION("Unable to open package file");

	Core::TarReader tarReader(reader);

	Core::TarReader::Entry * entry = NULL;

	std::map<std::string,Core::Image*> images;

	while((entry = tarReader.Next()) != NULL)
	{
		try{
			Core::ImageType type = Core::Image::GetImageType(entry->GetName());
			Core::Image * img = Core::Image::Load(type,entry);
			images.insert(std::pair<std::string,Core::Image*>(entry->GetName(),img));
		}
		catch(const Core::Exception & ex){
			for(std::map<std::string,Core::Image*>::iterator i = images.begin();i!=images.end();i++)
				delete i->second;
			SafeDelete(entry);
			SafeDelete(reader);
			THROW_EXCEPTION_STACK("Unable to load image plane",ex);
		}

		SafeDelete(entry);
	}

	SafeDelete(reader);

	if(images.size() != 6){
		for(std::map<std::string,Core::Image*>::iterator i = images.begin();i!=images.end();i++)
			delete i->second;
	
		THROW_EXCEPTION("Number of planes for cubemap is invalid");
	}

	std::vector<Core::Image*> imgvec;

	for(std::map<std::string,Core::Image*>::iterator i = images.begin();i!=images.end();i++){
		Core::Image * img = i->second;
		imgvec.push_back(img);
	}

	TextureCubemap_GL2 * ret = Load(dev,imgvec, mipmaps);

	for(std::map<std::string,Core::Image*>::iterator i = images.begin();i!=images.end();i++)
		delete i->second;
#ifdef NCK_GRAPH_RES_PROXY
	ret->m_Proxy = dev->AddTextureProxy(packageFilename,ret);
#endif
	return ret;
}

TextureCubemap_GL2 * TextureCubemap_GL2::Load(Device_GL2 * dev, const std::vector<Core::Image*> & images, bool mipmaps){
	Core::Image * img = images[0];

	int width = img->GetWidth();
	int height = img->GetHeight();
	//int depth = images.size();

	TextureCubemap_GL2 * ret = new TextureCubemap_GL2(dev);

	glEnable(ret->m_Target);
	glGenTextures(1,&ret->m_Texture);
	glBindTexture(ret->m_Target,ret->m_Texture);

	ret->m_Width = width;
	ret->m_Height = height;

	if(img->GetFormat() == Core::PIXEL_FORMAT_ALPHA_8B)
		ret->m_Format = Graph::FORMAT_A_8B;
	else if(img->GetFormat() == Core::PIXEL_FORMAT_RGBA_8B)
		ret->m_Format = Graph::FORMAT_RGBA_8B;
	else if(img->GetFormat() == Core::PIXEL_FORMAT_RGB_8B)
		ret->m_Format = Graph::FORMAT_RGB_8B;

	int components = 3;
	GLenum format = GL_RGB;
	GLenum datatype = GL_UNSIGNED_BYTE;
	GLenum intFormat = GL_RGB8;

	GetGLPropertiesFromFormat(ret->m_Format,&components,&intFormat,&format,&datatype);

	if(mipmaps)
		glTexParameteri(ret->m_Target, GL_GENERATE_MIPMAP, GL_TRUE);
	else
		glTexParameteri(ret->m_Target, GL_GENERATE_MIPMAP, GL_FALSE);

	GLint cubemap_face[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB};

	for(int i = 0;i<6;i++)
	{
		glTexImage2D(cubemap_face[i], 0, (components == 4) ? GL_RGBA: GL_RGB,
			width, height, 0, (components == 4) ? GL_RGBA: GL_RGB, GL_UNSIGNED_BYTE, images[i]->GetData());
	}

	glBindTexture(ret->m_Target,GL_ZERO);

	return ret;
}

RTManager_GL2::RTManager_GL2(Device_GL2 *dev){
	m_Device = dev;
	m_RenderBuffer = 0;
	m_FrameBuffer = 0;
}

RTManager_GL2::~RTManager_GL2(){
	glDeleteFramebuffers(1,&m_FrameBuffer);
	glDeleteRenderbuffers(1,&m_RenderBuffer);
}

bool RTManager_GL2::Enable(){

	glBindFramebuffer( GL_FRAMEBUFFER, m_FrameBuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, m_RenderBuffer );

	int count = 0;

	for(std::list<AttachedTexture>::iterator i = m_Textures.begin();i!=m_Textures.end();i++,count++){
		Texture2D_GL2 * tex = dynamic_cast<Texture2D_GL2*>(i->texture);
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+count, GL_TEXTURE_2D,tex->getTextureId(), i->target );
	}

	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);

	m_Device->m_InsideFBO = true;

	return true;
}

void RTManager_GL2::Load(GLuint width, GLuint height){
	glGenFramebuffers( 1, &m_FrameBuffer );
	glGenRenderbuffers( 1, &m_RenderBuffer );

	glBindRenderbuffer( GL_RENDERBUFFER, m_RenderBuffer);

	GLenum depth = GL_DEPTH_COMPONENT24;

	glRenderbufferStorage( GL_RENDERBUFFER,depth, width, height );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

}

bool RTManager_GL2::Attach(unsigned int target,Texture2D *tex){
	m_Textures.push_back(AttachedTexture(dynamic_cast<Texture_GL2*>(tex),target,0));
	return true;
}

bool RTManager_GL2::Attach(unsigned int target,unsigned int face,TextureCubeMap *tex){
	m_Textures.push_back(AttachedTexture(dynamic_cast<Texture_GL2*>(tex),target,face));
	return true;
}

bool RTManager_GL2::Disable(){
	m_Device->m_InsideFBO = false;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return true;
}

Texture3D_GL2::Texture3D_GL2(Device_GL2 *dev) : Texture_GL2(dev){
	m_Target = GL_TEXTURE_3D;
	m_Depth = 0;
}

Texture3D_GL2::~Texture3D_GL2(){
	m_Device->m_TextureCache.Clear();

#ifdef NCK_GRAPH_RES_PROXY
	// Check if GPU resource can be released.
	if(!m_Device->RemoveTextureProxy(this))
		return;
#endif

	if(m_Texture)
		glDeleteTextures(1,&m_Texture);

	m_Texture=0;
}

void Texture3D_GL2::Enable(unsigned int sampler_id){
	if(sampler_id >= 8)	
		return;

	if(m_Device->m_TextureCache.GetTexture(sampler_id) != (Texture3D*)this)
	{
		// Disable previous texture.
		if(m_Device->m_TextureCache.GetTexture(sampler_id)){
			m_Device->m_TextureCache.GetTexture(sampler_id)->Disable(sampler_id);
		}
        
        m_Device->m_SamplersPerTarget.find(m_Target)->second++;
        
		// Enable this texture.
		glEnable(m_Target);
		glActiveTexture(GL_TEXTURE0+sampler_id);
		glBindTexture(m_Target,m_Texture);
	}

	SetFilterAndWrapping();

	m_Device->m_TextureCache.SetTexture((Graph::Texture3D*)(this),true,sampler_id);
}

void Texture3D_GL2::Disable(unsigned int sampler_id){
	if(sampler_id >= 8)	return;

	// Unbind texture if it was scheduled to be disabled.
	if(!m_Device->m_TextureCache.GetTextureState(sampler_id))
	{
        GLuint count = m_Device->m_SamplersPerTarget.find(m_Target)->second;
        if (count > 0)
            count--;
        m_Device->m_SamplersPerTarget.find(m_Target)->second = count;

		glActiveTexture(GL_TEXTURE0+sampler_id);
		glBindTexture(m_Target,0);

        if (count == 0)
            glDisable(m_Target);

		// Remove from cache
		m_Device->m_TextureCache.SetTexture(NULL,false,sampler_id);
		return;
	}

	// Scheduled texture to be disabled.
	m_Device->m_TextureCache.SetTexture((Graph::Texture3D*)(this),false,sampler_id);
}

TextureType Texture3D_GL2::GetType(){
	return TEXTURE_3D;
}

GLuint Texture3D_GL2::getTextureId(){
	return m_Texture;
}


Texture3D_GL2 * Texture3D_GL2::Load(Device_GL2 * dev,const std::string & packageFilename, bool mipmaps)
{
#ifdef NCK_GRAPH_RES_PROXY
	ResourceProxy<Texture_GL2> * proxy = NULL;
	Texture3D_GL2 * refTex = NULL;

	if((proxy = dev->GetTextureProxy(packageFilename)) != NULL && (refTex = dynamic_cast<Texture3D_GL2*>(proxy->GetReference())) != NULL){
		Texture3D_GL2 * ret = new Texture3D_GL2(dev);
		ret->m_Width = refTex->m_Width;
		ret->m_Height = refTex->m_Height;
		ret->m_Format = refTex->m_Format;
		ret->m_Target = refTex->m_Target;
		ret->m_Texture = refTex->m_Texture;
		ret->m_Proxy = proxy;
		ret->m_Depth = refTex->m_Depth;

		dev->AddTextureProxy(packageFilename,refTex);		
		return ret;
	}
#endif 
	std::map<std::string,Core::Image*> images;

	Core::FileReader * reader = Core::FileReader::Open(packageFilename);
	if(!reader)
		THROW_EXCEPTION("Unable to open package file");

	Core::TarReader tarReader(reader);

	Core::TarReader::Entry * entry = NULL;

	while((entry = tarReader.Next())!=NULL){
		try{
			Core::ImageType type = Core::Image::GetImageType(entry->GetName());
			Core::Image * img = Core::Image::Load(type,entry);
			images.insert(std::pair<std::string,Core::Image*>(entry->GetName(),img));
		}
		catch(const Core::Exception & ex){
			for(std::map<std::string,Core::Image*>::iterator i = images.begin();i!=images.end();i++)
				delete i->second;
			SafeDelete(entry);
			SafeDelete(reader);
			THROW_EXCEPTION_STACK("Unable to load image plane",ex);
		}

		SafeDelete(entry);
	}

	SafeDelete(reader);

	std::vector<Core::Image*> imgvec;

	for(std::map<std::string,Core::Image*>::iterator i = images.begin();i!=images.end();i++){
		Core::Image * img = i->second;
		imgvec.push_back(img);
	}

	Texture3D_GL2 * ret = Load(dev,imgvec, mipmaps);

	for(std::map<std::string,Core::Image*>::iterator i = images.begin();i!=images.end();i++)
		delete i->second;
#ifdef NCK_GRAPH_RES_PROXY
	ret->m_Proxy = dev->AddTextureProxy(packageFilename,ret);	
#endif
	return ret;
}

Texture3D_GL2 * Texture3D_GL2::Load(Device_GL2 * dev, const std::vector<std::string> & filenames, bool mipmaps)
{
	std::vector<Core::Image*> images;
	images.reserve(filenames.size());

	for(size_t i = 0;i<filenames.size();i++){
		try{
			Core::Image * img = Core::Image::Load(filenames[i]);
			images.push_back(img);
		}
		catch(const Core::Exception & ex){
			for(size_t i = 0;i<images.size();i++)
				SafeDelete(images[i]);
			THROW_EXCEPTION_STACK("Image plane not found",ex);
		}
	}

	Texture3D_GL2 * ret = Load(dev,images,mipmaps);

	for(size_t i = 0;i<images.size();i++)
		delete images[i];

	return ret;
}

Texture3D_GL2 * Texture3D_GL2::Load(Device_GL2 * dev, const std::vector<Core::Image*> & images, bool mipmaps)
{
	Core::Image * img = images[0];

	int width = img->GetWidth();
	int height = img->GetHeight();
	int depth = (int)images.size();

	Texture3D_GL2 * ret = new Texture3D_GL2(dev);

	glEnable(ret->m_Target);
	glGenTextures(1,&ret->m_Texture);
	glBindTexture(ret->m_Target,ret->m_Texture);

	ret->m_Width = width;
	ret->m_Height = height;

	if(img->GetFormat() == Core::PIXEL_FORMAT_ALPHA_8B)
		ret->m_Format = Graph::FORMAT_A_8B;
	else if(img->GetFormat() == Core::PIXEL_FORMAT_RGBA_8B)
		ret->m_Format = Graph::FORMAT_RGBA_8B;
	else if(img->GetFormat() == Core::PIXEL_FORMAT_RGB_8B)
		ret->m_Format = Graph::FORMAT_RGB_8B;

	ret->m_Depth = depth;

	int components = 3;
	GLenum format = GL_RGB;
	GLenum datatype = GL_UNSIGNED_BYTE;
	GLenum intFormat = GL_RGB8;

	GetGLPropertiesFromFormat(ret->m_Format,&components,&intFormat,&format,&datatype);

	if(mipmaps)
		glTexParameteri(ret->m_Target, GL_GENERATE_MIPMAP, GL_TRUE);
	else
		glTexParameteri(ret->m_Target, GL_GENERATE_MIPMAP, GL_FALSE);

	unsigned char * texData = new unsigned char[width * height * depth * components];

	for(size_t i = 0;i<images.size();i++){
		memcpy(texData+(width*height)*components*i,images[i]->GetData(),width*height*components);
	}

	glTexImage3D(ret->m_Target,0,components,width,height,depth,0,format,datatype,texData);

	glBindTexture(ret->m_Target,GL_ZERO);

	delete [] texData;

	return ret;
}

unsigned int Texture3D_GL2::GetDepth(){
	return m_Depth;
}

#endif

_GRAPHICS_END
