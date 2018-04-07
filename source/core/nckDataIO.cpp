
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckDataIO.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "nckUtils.h"
#include <fstream>
#include <time.h>

#include <list>

#if defined(NCK_LINUX)
    #include <unistd.h>
#elif defined(NCK_WINDOWS)
    #include <Windows.h>
#elif defined(NCK_MACOSX)
    #include "macosx/nckWindow_macosx.h"
#endif

#define _FILE_OFFSET_BITS = 64

static std::string * app_data_folder = NULL;
static std::list<Core::TarReader*> * app_mounted_tar = NULL;

void initAppDataFolder(void){
	app_data_folder = new std::string();
}

void releaseAppDataFolder(void){
	SafeDelete(app_data_folder);
}

_CORE_BEGIN

std::string GetDataFolder()
{
	if (app_data_folder == NULL)
		initAppDataFolder();

	if(app_data_folder->length() == 0)
	{
#if defined(NCK_MACOSX)
        *app_data_folder = Application_Filename_MacOSX();
#else
        int bufferCapacity = 50;
		char * tempBuffer = NULL;
    #if defined(NCK_LINUX)
		while(true){
			SafeArrayDelete(tempBuffer);
			tempBuffer = new char[bufferCapacity];	
			int size = readlink("/proc/self/exe",tempBuffer,bufferCapacity);
			if(size<bufferCapacity){
				tempBuffer[size] = '\0';
				break;
			}
			bufferCapacity += 50;
		}
    #elif defined(NCK_WINDOWS)
		while(true){
			SafeArrayDelete(tempBuffer);
			tempBuffer = new char[bufferCapacity];	
			int size = GetModuleFileName(NULL,tempBuffer,bufferCapacity);
			if(size<bufferCapacity){
				tempBuffer[size] = '\0';
				break;
			}
			bufferCapacity += 50;
		}
    #endif
        *app_data_folder = std::string(tempBuffer);
		SafeArrayDelete(tempBuffer);
#endif
        
		size_t lastPathBreak = app_data_folder->find_last_of("/");
		
		if(lastPathBreak == std::string::npos)
			lastPathBreak =  app_data_folder->find_last_of("\\");

		*app_data_folder = app_data_folder->substr(0,lastPathBreak+1);

		size_t projectPosition = -1;

		if(((projectPosition=app_data_folder->find("project/pybuild/")) != std::string::npos) || 
				((projectPosition = app_data_folder->find("project\\vs\\")) != std::string::npos) ||
                ((projectPosition=app_data_folder->find("project/xcode/")) != std::string::npos))
		{
			*app_data_folder = app_data_folder->substr(0,projectPosition);
		}

#if defined(NCK_WINDOWS)
		*app_data_folder += "data\\";
#else
		*app_data_folder += "data/";
#endif
	}

	return *app_data_folder;
}

std::string ResolveFilename(const std::string & filename){
	std::string newFilename = GetDataFolder();
	std::string ret;

	if(filename.find("://") != std::string::npos)
	{
		if(filename.find("audio://") != std::string::npos){
			newFilename += "audio/";
		}
		else if(filename.find("shader://") != std::string::npos){
			newFilename += "shaders/";
		}
		else if(filename.find("texture://") != std::string::npos){
			newFilename += "textures/";
		}
		else if(filename.find("script://") != std::string::npos){
			newFilename += "scripts/";
		}
		else if(filename.find("scene://") != std::string::npos){
			newFilename += "scenes/";
		}
		else if(filename.find("model://") != std::string::npos){
			newFilename += "models/";
		}

		std::string rest = filename.substr(filename.find_first_of("://")+3);
		ret = newFilename + rest;
	}
	else
		ret = filename;

#if defined _WIN32 || defined _WIN64
		for(size_t i = 0;i<ret.length();i++){
			if(ret[i] == '/')
				ret[i] = '\\';
		}
#endif

	return ret;
}

DataReader * DataReader::Open(const std::string & filename) {
    std::string fFilename = filename;

    if (filename.find("://") != std::string::npos)
    {
        std::string newFilename;
        if (filename.find("audio://") != std::string::npos) {
            newFilename += "audio/";
        }
        else if (filename.find("shader://") != std::string::npos) {
            newFilename += "shaders/";
        }
        else if (filename.find("texture://") != std::string::npos) {
            newFilename += "textures/";
        }
        else if (filename.find("script://") != std::string::npos) {
            newFilename += "scripts/";
        }
        else if (filename.find("scene://") != std::string::npos) {
            newFilename += "scenes/";
        }
        else if (filename.find("model://") != std::string::npos) {
            newFilename += "models/";
        }

        std::string rest = filename.substr(filename.find_first_of("://") + 3);
        fFilename = "data/"+newFilename + rest;
    }

    if (app_mounted_tar != NULL) {
        ListFor(TarReader*, (*app_mounted_tar), i) {
            return dynamic_cast<DataReader*>((*i)->Find(fFilename));
        }
    }
    return dynamic_cast<DataReader*>(FileReader::Open(filename));
}

FileReader * FileReader::Open(const std::string & filename){
    std::string resolved = ResolveFilename(filename);
	FILE * temp = fopen(resolved.c_str(),"rb");

	if(!temp)
		return NULL;

	FileReader * reader = new FileReader();

	reader->fHandle = temp;
    reader->fPath = resolved;

	return reader;
}

FileReader::~FileReader(){
	fclose(fHandle);
}

size_t FileReader::Read(void * data, size_t size){
    size_t elements = fread(data, size, 1, fHandle);
    if (elements == 1) {
        size_t sz = elements* size;
        return sz;
    }
    return 0;
}


bool FileReader::Read(std::string * str)
{
	int sz=0;
	fread(&sz,1,sizeof(int),fHandle);
	char *dump = new char[sz+1];
	fread(dump,1,sz,fHandle);
	dump[sz] = '\0';

	*str = dump;
	delete dump;

	return true;
}

int64_t FileReader::Tell(){
	if(feof(fHandle))
		return -1;
#if defined(NCK_WINDOWS)
	return _ftelli64(fHandle);
#else
	return ftello(fHandle);
#endif
}

bool FileReader::Seek(int64_t pos,SeekOffsetMode mode){
	int seekFlag = 0;
	switch(mode){
		case SEEK_OFFSET_BEGIN:
			seekFlag = SEEK_SET;
			break;
		case SEEK_OFFSET_END:
			seekFlag = SEEK_END;
			break;
		case SEEK_OFFSET_CURRENT:
			seekFlag = SEEK_CUR;
			break;
	}
#if defined(NCK_WINDOWS)
	return _fseeki64(fHandle,pos,seekFlag) == 0;
#else
	return fseeko(fHandle,pos,seekFlag) == 0;
#endif
}

bool FileReader::Exists(const std::string & filename){
	FILE * f = fopen(ResolveFilename(filename).c_str(),"rb");
	if(f == NULL)
		return false;
	else
		fclose(f);
	return true;
}

int64_t FileReader::Size(const std::string & filename){
#if defined(NCK_WINDOWS)
     struct __stat64 buf;
     memset(&buf,0,sizeof(struct __stat64));
	_stati64(ResolveFilename(filename).c_str(), &buf);
#else
    struct stat buf;
    memset(&buf,0,sizeof(struct stat));
	stat(ResolveFilename(filename).c_str(), &buf);
#endif
	return buf.st_size;
}

int64_t FileReader::Length(){
#if defined(NCK_WINDOWS)
    struct __stat64 buf;
    memset(&buf, 0, sizeof(struct __stat64));
    _stati64(fPath.c_str(), &buf);
#else
    struct stat buf;
    memset(&buf, 0, sizeof(struct stat));
    stat(ResolveFilename(filename).c_str(), &buf);
#endif
    return buf.st_size;
}

MemoryReader::MemoryReader(uint8_t *data,int64_t size, bool copy){
	if(copy){
		mData = new uint8_t[size];
		memcpy(mData,data,size);
	}
	else
		mData = data;
	mCopy = copy;
	mSize = size;
	mStart = 0;
}

MemoryReader::~MemoryReader(){
	if(mCopy){
		SafeDelete(mData);
	}
}

int64_t MemoryReader::GetSize(){
	return mSize;
}

int64_t MemoryReader::Length() {
    return mSize;
}

size_t MemoryReader::Read(void * data, size_t size){
	int64_t copySize = size;
	if(mStart+copySize > mSize)
		copySize -= (mStart+copySize)-mSize;

	if(copySize <= 0)
		return 0;

	memcpy(data,mData+mStart,copySize);
	mStart += copySize;

	return copySize;
}

bool MemoryReader::Read(std::string * str){
	size_t size = 0;
	if(Read(&size,sizeof(short))!=sizeof(short))
		return false;
	char * buff = new char[size];
	if(Read(buff,size)!=size){
		SafeArrayDelete(buff);
		return false;
	}

	*str = std::string(buff);
	SafeArrayDelete(buff);
	return true;
}

int64_t MemoryReader::Tell(){
	return mStart;
}

bool MemoryReader::Seek(int64_t pos,SeekOffsetMode mode){
	if(mode == SEEK_OFFSET_BEGIN)
		mStart = pos;
	else if(mode == SEEK_OFFSET_CURRENT)
		mStart += pos;
	else if(mode == SEEK_OFFSET_END)
		mStart = mSize-pos;
	return true;
}

struct TarHeader{
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char checksum[8];
	char typeflag[1];
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
	char pad[12];
};

TarReader::Entry::Entry(const std::string & name,uint8_t *data,size_t size) : MemoryReader(data,size,true){
	mName = name;
}

TarReader::Entry::~Entry(){

}

std::string TarReader::Entry::GetName(){return mName;}

TarReader::TarReader(DataReader * reader){
	fReader = reader;
}

TarReader::~TarReader(){

}

void TarReader::Mount() {
    if (app_mounted_tar == NULL) {
        app_mounted_tar = new std::list<TarReader*>();
    }
    app_mounted_tar->push_back(this);
}

TarReader::Entry * TarReader::Next()
{
	TarHeader header;
	size_t hsize = fReader->Read(&header,sizeof(TarHeader));

	if(hsize!=sizeof(TarHeader))
		return NULL;

	size_t fsize = strtol(header.size,NULL,0);

	if (fsize == 0) return 0;

	size_t maxSize = ((fsize + 512 - 1) / 512) * 512;
	uint8_t * data = new uint8_t[maxSize];

	size_t rsize = fReader->Read(data,(uint32_t)maxSize);

	if(rsize!= maxSize){
		SafeDelete(data);
		return NULL;
	}

	TarReader::Entry * ret = new TarReader::Entry(std::string(header.name),data,fsize);

	SafeDelete(data);

	return ret;
}

TarReader::Entry * TarReader::Find(const std::string & filename){
    fReader->Seek(0, SEEK_OFFSET_BEGIN);

    do {
        TarHeader header;
        size_t hsize = fReader->Read(&header, sizeof(TarHeader));

        if (hsize != sizeof(TarHeader))
            return NULL;

        size_t fsize = strtol(header.size, NULL, 0);

        std::string name = std::string(header.name);

        if (fsize == 0)
            continue;

        size_t maxSize = ((fsize + 512 - 1) / 512) * 512;

        if (name == filename) {
            uint8_t * data = new uint8_t[maxSize];

            size_t rsize = fReader->Read(data, (uint32_t)maxSize);

            if (rsize != maxSize) {
                SafeDelete(data);
                return NULL;
            }

            TarReader::Entry * ret = new TarReader::Entry(name, data, fsize);

            SafeDelete(data);

            return ret;
        }
        else {
            if (!fReader->Seek(maxSize, Core::SEEK_OFFSET_CURRENT))
                return NULL;
        }

    } while (true);

    return NULL;
}

std::string FindExtension(const std::string & filename){
	size_t extPos = filename.find_last_of(".");
	if(extPos != std::string::npos){
		return filename.substr(extPos+1,filename.length()-extPos-1);				
	}
	return "";
}

int64_t GetFileLastModified(const std::string & filename) {
    std::string path = ResolveFilename(filename);
#if defined _WIN32 || defined _WIN64
    HANDLE hFile = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
      return 0;
    
    FILETIME ftCreate, ftAccess, ftWrite;
    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
        return FALSE;

    SYSTEMTIME stUTC, stLocal;
    FileTimeToSystemTime(&ftWrite, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

    tm t;
    memset(&t, 0, sizeof(tm));
    t.tm_year = stLocal.wYear-1900;
    t.tm_mon = stLocal.wMonth-1;
    t.tm_mday = stLocal.wDay;
    t.tm_hour = stLocal.wHour;
    t.tm_min = stLocal.wMinute;
    t.tm_sec = stLocal.wSecond;
    t.tm_isdst = -1;

    CloseHandle(hFile);

    time_t tt = mktime(&t);

    return tt;
#elif defined(NCK_MACOSX)
    return FileModified_MacOSX(path);
#else
    return 0;
#endif
}

FileWriter::~FileWriter() {
    if (fHandle)
        fclose(fHandle);
    fHandle = NULL;
}

FileWriter * FileWriter::Open(const std::string & filename, bool append) {
    int64_t length = 0;
    if (FileReader::Exists(filename))
        length = FileReader::Size(filename);
    
    std::string resolved = ResolveFilename(filename);
    FILE * file = fopen(resolved.c_str(), "wb");

    if (file == NULL)
        return NULL;

    FileWriter * ret = new FileWriter();
    ret->fHandle = file;
    ret->m_Length = length;
    return ret;
}

size_t FileWriter::Write(uint8_t * data, size_t size) {
    size_t elements = fwrite(data, size, 1, fHandle);
    if (elements == 1) {
        size_t sz = elements* size;
        m_Length += sz;
        return sz;
    }
    return 0;
}

int64_t FileWriter::Tell() 
{  
    if (feof(fHandle))
        return -1;
#if defined(NCK_WINDOWS)
    return _ftelli64(fHandle);
#else
    return ftello(fHandle);
#endif
}

int64_t FileWriter::Length() {
    return m_Length;
}

bool FileWriter::Seek(int64_t pos, SeekOffsetMode mode) {
    int seekFlag = 0;
    switch (mode) {
    case SEEK_OFFSET_BEGIN:
        seekFlag = SEEK_SET;
        break;
    case SEEK_OFFSET_END:
        seekFlag = SEEK_END;
        break;
    case SEEK_OFFSET_CURRENT:
        seekFlag = SEEK_CUR;
        break;
    }
#if defined(NCK_WINDOWS)
    return _fseeki64(fHandle, pos, seekFlag) == 0;
#else
    return fseeko(fHandle, pos, seekFlag) == 0;
#endif
}

_CORE_END
