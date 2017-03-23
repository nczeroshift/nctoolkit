
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_RESOURCE_PROXY_SOURCE_H
#define NCK_RESOURCE_PROXY_SOURCE_H

#include "nckGraphicsConfig.h"
#include "nckUtils.h"

#include <string>
#include <list>

_GRAPHICS_BEGIN

template <class T>
class ResourceProxy{
public:
	/**
	* Proxy manager constructor.
	* @param resName Resource name (filename for example).
	*/
	ResourceProxy(const std::string & resName){
		m_ResourceName = resName;
	}

	/**
	* Release non-dealocated resources.
	*/
	~ResourceProxy(){
		while(m_Collection.size()){
			T * res = m_Collection.back();
			m_Collection.pop_back();
			SafeDelete(res);
		}
	}

	/**
	* Add resource to queue.
	*/
	void Add(T * tex){
		m_Collection.push_back(tex);
	}

	/**
	* Remove resource from queue.
	* @return Returns true if queue is empty.
 	*/
	bool Remove(T * tex){
		m_Collection.remove(tex);
	
		if(m_Collection.size())
			return false;
		
		return true;
	}

	/**
	* Get queue size.
	*/
	int GetSize() const{
		return (int)m_Collection.size();
	}

	/**
	* Get the first element in queue.
	*/ 
	T * GetReference(){
		return m_Collection.front();
	}

	/**
	* Get managed resource title.
	*/
	std::string GetResourceName() const{
		return m_ResourceName;
	}

private:
	/**
	* Resource name.
	*/
	std::string m_ResourceName;

	/**
	* List of resources sharing a common GPU object.
	*/
	std::list<T*> m_Collection;
};

_GRAPHICS_END

#endif // #ifndef NCK_RESOURCE_PROXY_SOURCE_H
