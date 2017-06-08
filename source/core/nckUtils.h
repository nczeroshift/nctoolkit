
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_UTILS_H
#define NCK_UTILS_H

#include "nckCoreConfig.h"
#include <string>
#include <vector>

_CORE_BEGIN

/* Datastructure iterator macros */

/// Shortcut to iterate through std::list
#define	ListFor(for_type,for_list,for_iter) \
    for(std::list<for_type>::iterator for_iter = for_list.begin(); \
        for_iter!=for_list.end();for_iter++)

/// Shorcut to iterate through std::list, without incrementing
#define ListWhile(_type, _list,_iterator) \
    std::list<_type>::iterator _iterator = _list.begin();\
        while(_iterator!=_list.end())

// Shortcut to iterate through std::map
#define MapFor(_key,_value,_map,_iterator) \
    for(std::map<_key,_value>::iterator _iterator = _map.begin();_iterator != _map.end();_iterator++)

// Shortcut to set/replace a pair inside std::map
#define MapSet(map,keyt,objt,keyv,objv) \
	if(map.find(keyv)!=map.end()) map.find(keyv)->second = objv; else map.insert(std::pair<keyt,objt>(keyv, objv));	

/* Memory dealocation macros */

/// Safe pointer delete
#define SafeDelete(_pointer) if (_pointer){delete _pointer;_pointer=NULL;};

/// Safe array delete
#define SafeArrayDelete(_array) if (_array){delete [] _array;_array=NULL;};

/// Safe class release
#define SafeRelease(_class) if (_class){_class->Release();};

/* Basic math defines */

#if !defined(ABS)
// Basic abs definition
#define ABS(val) (val<0?-val:val)
#endif

#if !defined(MIN)
#define MIN(a,b) (a>b?b:a)
#endif

#if !defined(MAX)
#define MAX(a,b) (a>b?a:b)
#endif

/// Check if a number is base of two.
bool IsBaseOfTwo(int number);

/* String manipulation functions */

/// Split string in substrings at the tokens occurences(no regex support).
int StringSplit(const std::string src,
				const std::string & token,
				std::vector<std::string> * splits);

std::string ltrim(const std::string & exp);
std::string rtrim(const std::string & exp);

/* Other */

// Print a message when compiled in debug.
void DebugLog(const std::string & msg);

_CORE_END

#endif // #ifndef NCK_UTILS_H


