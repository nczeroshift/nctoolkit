
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckQueueBuffer.h"
#include "nckUtils.h"
#include <string.h>

_CORE_BEGIN

QueueBuffer::QueueBuffer(){
    m_Resize = false;
    m_Capacity = 1024*1024; // 1MiB
    m_Buffer = new uint8_t[m_Capacity];
	m_Size = 0;
}

QueueBuffer::QueueBuffer(int64_t maxCapacity){
    m_Capacity = maxCapacity;
    m_Buffer = NULL;
    m_Resize = false;
    // Start with 0 capacity and resize on demand.
    if (m_Capacity > 0)
        m_Buffer = new uint8_t[m_Capacity];
    else
        m_Resize = true;
	m_Size = 0;
}

QueueBuffer::~QueueBuffer(){
    uint8_t * d = (uint8_t*)m_Buffer;
    SafeArrayDelete( d );
}

void QueueBuffer::Clear() {
    m_Size = 0;
    memset(m_Buffer, 0, m_Capacity);
}
    
int64_t QueueBuffer::Push(void * data, int64_t size){
    if (m_Size + size > m_Capacity) {
        if (m_Resize) {
            m_Capacity = m_Size + size;
            uint8_t * dNew = new uint8_t[m_Capacity];
            memcpy(dNew, m_Buffer, m_Size);
            SafeArrayDelete(m_Buffer);
            m_Buffer = dNew;
        }
        else
            return 0;
    }

    memcpy((uint8_t*)m_Buffer+m_Size,data,size);
    m_Size += size;
    
    return m_Size;
}

int64_t QueueBuffer::Push(const std::string & str) {
    if (m_Size + str.length() > m_Capacity) {
        if (m_Resize) {
            m_Capacity = m_Size + str.length();
            uint8_t * dNew = new uint8_t[m_Capacity];
            memcpy(dNew, m_Buffer, m_Size);
            SafeArrayDelete(m_Buffer);
            m_Buffer = dNew;
        }
        else
            return 0;
    }

    memcpy((uint8_t*)m_Buffer + m_Size, str.c_str(), str.length());
    m_Size += str.length();

    return m_Size;
}

int64_t QueueBuffer::Size(){
    return m_Size;
}

int64_t QueueBuffer::Pop(void * targetData, int64_t targetSize){
    int64_t ret = MIN(targetSize,m_Size);
    
    if(targetData!=NULL)
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

int QueueBuffer::ToString(std::string * str) {
    char * b = new char[Size() + 1];
    b[Size()] = 0;
    memcpy(b, m_Buffer, Size());
    *str = std::string(b);
    SafeArrayDelete(b);
    return Size();
}

_CORE_END
