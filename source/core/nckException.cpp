
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckException.h"
#include <stdio.h>
#include <stdlib.h>
#include "nckDataIO.h"

#define EXCEPTION_LOGFILE
#define EXCEPTION_STDOUT

static int nckExceptionCount = 0;

_CORE_BEGIN

Exception::Exception(const std::string & text,const std::string & method,const std::string & source,
	unsigned int line){
	m_Text = text;
	m_Method = method;
	m_Source = source.substr(source.find_last_of("\\")+1,source.length());
	m_Line = line;
	m_Code = 0;
}

Exception::Exception(const std::string & text,const std::string & method,const std::string & source,
	unsigned int line,const Exception & stack)
{
	m_Code = 0;
	m_Text = text;
	m_Method = method;
	m_Source = source.substr(source.find_last_of("\\")+1,source.length());
	m_Line = line;

	m_Stack.push_back(stack);

	std::list<Exception> prevStack;
	stack.GetStack(&prevStack);

	for(std::list<Exception>::iterator i = prevStack.begin();
		i!=prevStack.end();i++)
	{
		m_Stack.push_back(*i);
	}
}

void Exception::PrintStackTrace() const
{

#ifdef EXCEPTION_LOGFILE
	FILE * f = NULL;
	std::string logpath = GetDataFolder()+"exception.log";

	if(nckExceptionCount == 0)
		f = fopen(logpath.c_str(),"w");
	else 
		f = fopen(logpath.c_str(),"a");

	if(f == NULL)
	{
		if(nckExceptionCount == 0)
			f = fopen("exception.log","w");
		else 
			f = fopen("exception.log","a");
	}

	nckExceptionCount++;

#endif

	std::list<Exception> tmpList;
	GetStack(&tmpList);

	tmpList.push_front(*this);

	for(std::list<Exception>::iterator i = tmpList.begin();
		i!=tmpList.end();i++)
	{
#ifdef EXCEPTION_LOGFILE
		fprintf(f,"Source:%s, Method:%s, Line:%d",(*i).GetSource().c_str(),(*i).GetMethod().c_str(),(*i).GetLine());
		fprintf(f,", Code:%d",(*i).GetCode());
		if((*i).GetText().size()>0)
			fprintf(f,"\n\t, Text:%s",(*i).GetText().c_str());
		if((*i).GetMetadata().size()>0)
			fprintf(f,"\n\t, Metadata:%s",(*i).GetMetadata().c_str());
		fprintf(f,"\n");
		//fprintf(f,"Source:%s, Method:%s, Line:%d, Text:%s\n",(*i).GetSource().c_str(),(*i).GetMethod().c_str(),(*i).GetLine(),(*i).GetText().c_str());
#endif
#ifdef EXCEPTION_STDOUT
		printf("Exception, %s, %s, %d, %s\n",(*i).GetSource().c_str(),(*i).GetMethod().c_str(),(*i).GetLine(),(*i).GetText().c_str());
#endif
	}

#ifdef EXCEPTION_LOGFILE
	fclose(f);
#endif
}


std::string Exception::ToString() const{
    std::string ret;

    std::list<Exception> tmpList;
    GetStack(&tmpList);

    tmpList.push_front(*this);

    for (std::list<Exception>::iterator i = tmpList.begin(); i != tmpList.end(); i++)
    {
        char line[64] = "";
        sprintf(line, "%d", +(*i).GetLine());
        if (i != tmpList.begin())
            ret += "\n";
        ret += "Exception, " + (*i).GetSource() + "," + (*i).GetMethod() + "," + std::string(line) + "," + (*i).GetText().c_str();
    }

    return ret;
}

_CORE_END
