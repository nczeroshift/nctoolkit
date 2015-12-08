
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_SHAREPTR_H_
#define NCK_SHAREPTR_H_

#include <nckCoreConfig.h>

_CORE_BEGIN

/**
 * Reference counting class.
 */
class ReferenceCounter
{
public:

	void AddRef()
	{
		count++;
	}

	int Release()
	{
		return --count;
	}

private:
	int count;
};

/**
 * Template class for the shared pointer implementation.
 */
template < typename T > class SharedPtr
{
private:
    T * pData;
    ReferenceCounter * reference;
public:

    /// Constructor
    SharedPtr() : pData(0), reference(0)
    {
        reference = new ReferenceCounter();
        reference->AddRef();
    }

    /// Constructor
    SharedPtr(T* pValue) : pData(pValue), reference(0)
    {
        reference = new ReferenceCounter();
        reference->AddRef();
    }

    /// Copy constructor
    SharedPtr(const SharedPtr<T>& sp) : pData(sp.pData), reference(sp.reference)
    {
        reference->AddRef();
    }

    /// Destructor
    ~SharedPtr()
    {
        if(reference->Release() == 0)
        {
            delete pData;
            delete reference;
        }
    }

    T& operator* ()
    {
        return *pData;
    }

    T* operator-> ()
    {
        return pData;
    }

    // Assignment operator
    SharedPtr<T>& operator = (const SharedPtr<T>& sp)
    {
        if (this != &sp) // Avoid self assignment
        {
            if(reference->Release() == 0)
            {
                delete pData;
                delete reference;
            }

            pData = sp.pData;
            reference = sp.reference;
            reference->AddRef();
        }
        return *this;
    }
};

_CORE_END

#endif // #ifndef NCK_SHAREPTR_H_
