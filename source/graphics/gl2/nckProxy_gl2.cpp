
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckGraphics_gl2.h"
#include "nckUtils.h"

_GRAPHICS_BEGIN

/**
* Shader Program Proxy Manager Class
* Avoids shader programs resource duplication and shares the same GPU resource with 
* different properties (variables states).
*/ 
class ProgramProxyManager_GL2{
public:
	/**
	* Program proxy manager constructor.
	* @param resName Resource name (filename for example).
	* @param dev Rendering Device Reference.
	*/
	ProgramProxyManager_GL2(const std::string & resName);

	/**
	* Release non-dealocated textures.
	*/
	~ProgramProxyManager_GL2();

	/**
	* Add program to queue.
	*/
	void Add(Program_GL2 * tex);

	/**
	* Remove program from queue.
	* @return Returns true if the program isn't shared, false otherwise.
 	*/
	bool Remove(Program_GL2 * tex);

	/**
	* Get queue size.
	*/
	int GetSize();

	/**
	* Get the first program in the queue to serve as a reference.
	*/ 
	Program_GL2 * GetReference();

	/**
	*
	*/
	std::string GetResourceName();
	
private:
	/**
	* Unique name to identify the common GPU object. 
	*/
	std::string m_ResourceName;

	/**
	* List of resources sharing a common GPU object.
	*/
	std::list<Program_GL2*> m_Collection;
};


/**
* Texture Proxy Manager Class.
* Avoids texture resource duplication and shares the same GPU resource with 
* different properties (filtering for example).
*/ 
class TextureProxyManager_GL2{
public:
	/**
	* Texture proxy manager constructor.
	* @param resName Resource name (filename for example).
	* @param dev Rendering Device Reference.
	*/
	TextureProxyManager_GL2(const std::string & resName);

	/**
	* Release non-dealocated textures.
	*/
	~TextureProxyManager_GL2();

	/**
	* Add texture to queue.
	*/
	void Add(Texture_GL2 * tex);

	/**
	* Remove texture from queue.
	* @return Returns true if the texture isn't shared, false otherwise.
 	*/
	bool Remove(Texture_GL2 * tex);

	/**
	* Get queue size.
	*/
	int GetSize();

	/**
	* Get the first texture in the queue to serve as a reference.
	*/ 
	Texture_GL2 * GetReference();

	/**
	*
	*/
	std::string GetResourceName();

private:
	/**
	* Unique name to identify the common GPU object. 
	*/
	std::string m_ResourceName;

	/**
	* List of resources sharing a common GPU object.
	*/
	std::list<Texture_GL2*> m_Collection;
};

/*
TextureProxyManager_GL2::TextureProxyManager_GL2(const std::string & resName){
	m_ResourceName = resName;
}

TextureProxyManager_GL2::~TextureProxyManager_GL2(){
	while(m_Collection.size()){
		Texture_GL2 * tex = m_Collection.back();
		m_Collection.pop_back();
		SafeDelete(tex);
	}
}

void TextureProxyManager_GL2::Add(Texture_GL2 * tex){
	m_Collection.push_back(tex);
}

bool TextureProxyManager_GL2::Remove(Texture_GL2 * tex){
	m_Collection.remove(tex);
}

int TextureProxyManager_GL2::GetSize(){
	return m_Collection.size();
}

Texture_GL2 * TextureProxyManager_GL2::GetReference(){
	return m_Collection.front();
}

std::string TextureProxyManager_GL2::GetResourceName(){
	return m_ResourceName;
}
	
	
	
	
ProgramProxyManager_GL2::ProgramProxyManager_GL2(const std::string & resName){
	m_ResourceName = resName;
}

ProgramProxyManager_GL2::~ProgramProxyManager_GL2(){
	while(m_Collection.size()){
		Program_GL2 * prog = m_Collection.back();
		m_Collection.pop_back();
		SafeDelete(prog);
	}
}

void ProgramProxyManager_GL2::Add(Program_GL2 * prog){
	m_Collection.push_back(prog);
}

bool ProgramProxyManager_GL2::Remove(Program_GL2 * prog){
	m_Collection.remove(prog);
}

int ProgramProxyManager_GL2::GetSize(){
	return m_Collection.size();
}

Program_GL2 * ProgramProxyManager_GL2::GetReference(){
	return m_Collection.front();
}

std::string ProgramProxyManager_GL2::GetResourceName(){
	return m_ResourceName;
}*/
	
_GRAPHICS_END
