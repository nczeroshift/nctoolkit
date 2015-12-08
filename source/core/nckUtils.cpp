
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckUtils.h"
#include "nckDataIO.h"
#include <stdlib.h>
#include <stdio.h>

#if defined(_WIN32) || defined (_WIN64)
#include <Windows.h>
#endif

_CORE_BEGIN

bool IsBaseOfTwo(int number){
	bool ret = false;
	for(int i = 0;i<32;i++){
		if(number>>i & 0x1)
		{
			if(!ret){ret = true;
			}
		else
		{
			ret = false;break;}
		}
	}
	return ret;
}

int StringSplit(std::string src, const std::string & token,std::vector<std::string> * splits){
	if(splits == NULL)
		return 0;
    
	while(true){
		size_t p = src.find_first_of(token);
		if(p!= src.npos){
			if(p>0)
				splits->push_back(src.substr(0,p));
            
			src = src.substr(p+token.size(),src.length());
		}
		else{
			splits->push_back(src.substr(0,src.length()));
			break;
		}
		
	}
    
	return (int)splits->size();
}

void DebugLog(const std::string & msg){
#if _DEBUG && ( defined(_WIN32) || defined (_WIN64))
	OutputDebugStringA(msg.c_str());
#else
	printf("%s",msg.c_str());
#endif
}

std::string ltrim(const std::string & exp){
    for(size_t i = 0;i<exp.length();i++)
        if(!isspace(exp[i]))
            return exp.substr(i);
    return exp;
}

std::string rtrim(const std::string & exp){
    for(int i = (int)exp.length()-1;i >= 0;i--)
        if(!isspace(exp[i]))
            return exp.substr(0,i+1);
    return exp;
}

_CORE_END
