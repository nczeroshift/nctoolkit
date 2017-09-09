

#include "bxonDataIO.h"

_BXON_BEGIN

ReaderContext::ReaderContext(Core::DataReader * reader){
    this->reader = reader;
}

ReaderContext::~ReaderContext(){
        
}
    
uint32_t ReaderContext::Read(int32_t size, uint8_t * d){
    if(reader->Read(d, size) == size)
        return size;
    return 0;
}
    
uint32_t ReaderContext::Write(int32_t size, uint8_t * d){
    return 0;
}
    
uint64_t ReaderContext::Seek(int64_t offset){
    return reader->Seek(offset,Core::SEEK_OFFSET_BEGIN);
}
    
uint64_t ReaderContext::Tell(){
    return reader->Tell();
}

_BXON_END