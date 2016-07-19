
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_EXCEPTION_H
#define NCK_EXCEPTION_H

#include "nckCoreConfig.h"
#include <string>
#include <list>
#include <exception>

_CORE_BEGIN

/// Exception handling class.
class Exception : public std::exception 
{
public:
	/// Default constructor.
	Exception(const std::string & text,const std::string & method="",const std::string & source="",
		unsigned int line = 0);

	/// Stack trace constructor
	Exception(const std::string & text,const std::string & method,const std::string & source,
		unsigned int line,const Exception & stack);

	virtual ~Exception() throw () {}

	/// Get exception type.
	std::string GetText()const{return m_Text;}

	/// Get the line number where the exception was thrown.
	unsigned int GetLine()const{return m_Line;}

	/// Get source file where the exception was thrown.
	std::string GetSource()const{return m_Source;}

	/// Get method where the exception was thrown.
	std::string GetMethod()const{return m_Method;}

	/// Get exception stack.
	void GetStack(std::list<Exception> * ret) const { *ret =m_Stack; }

	/// Set exception metadata text.
	void SetMetadata(const std::string & metadata){m_Metadata = metadata;}

	/// Get exception metadata text.
	std::string GetMetadata() const{return m_Metadata;}

	/// Set exception ID code.
	void SetCode(int code){m_Code = code;}

	/// Get exception ID code.
	int GetCode() const{return m_Code;}

	/// Print exception stack trace to output streams.
	void PrintStackTrace() const;

    /// Convert exception stack to string
    std::string ToString() const;
private:
	int m_Code;

	/// Exception metadata.
	std::string m_Metadata;

	/// Exception text.
	std::string m_Text;

	/// Source method where the exception was thrown.
	std::string m_Method;

	/// Source file where the exception was thrown.
	std::string m_Source;

	/// Line where the exception was thrown.
	unsigned int m_Line;

	/// Stack of previously throwned exceptions.
	std::list<Exception> m_Stack;
};
_CORE_END
	
#define THROW_EXCEPTION(what) throw Core::Exception(what,__FUNCTION__,__FILE__,__LINE__)
#define THROW_EXCEPTION_STACK(what, stack) throw Core::Exception(what,__FUNCTION__,__FILE__,__LINE__,stack)

#endif // #ifndef NCK_EXCEPTION_H
