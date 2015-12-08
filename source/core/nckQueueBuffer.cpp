
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckQueueBuffer.h"
#include "nckUtils.h"
#include <string.h>

_CORE_BEGIN

QueueBuffer::QueueBuffer(){
    m_Capacity = 1024*1024; // 1MiB
    m_Buffer = new uint8_t[m_Capacity];
	m_Size = 0;
}

QueueBuffer::QueueBuffer(int64_t maxCapacity){
    m_Capacity = maxCapacity; // 1MiB
    m_Buffer = new uint8_t[m_Capacity];
	m_Size = 0;
}

QueueBuffer::~QueueBuffer(){
    uint8_t * d = (uint8_t*)m_Buffer;
    SafeArrayDelete( d );
}
    
int64_t QueueBuffer::Push(void * data, int64_t size){
    if(m_Size+size > m_Capacity)
        return 0;
    
    memcpy((uint8_t*)m_Buffer+m_Size,data,size);
    m_Size += size;
    
    return m_Size;
}

int64_t QueueBuffer::Size(){
    return m_Size;
}

int64_t QueueBuffer::Pop(void * targetData, int64_t targetSize){
    int64_t ret = MIN(targetSize,m_Size);
    
    memcpy(targetData, m_Buffer, ret);
    
    m_Size -= ret;
    
    memmove(m_Buffer, (uint8_t*)m_Buffer+ret, m_Size);
    return ret;
}

void * QueueBuffer::GetData(){
    return m_Buffer;
}

void * QueueBuffer::CloneData(){
    uint8_t * ret = new uint8_t[m_Size];
    memcpy(ret, m_Buffer, m_Size);
    return ret;
}

_CORE_END
