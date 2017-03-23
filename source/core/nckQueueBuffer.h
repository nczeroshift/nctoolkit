
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_QUEUE_BUFFER_H
#define NCK_QUEUE_BUFFER_H

#include "nckCoreConfig.h"
#include <stdint.h>

_CORE_BEGIN

class QueueBuffer{
public:
    QueueBuffer();
    QueueBuffer(int64_t maxCapacity);
    ~QueueBuffer();
    
    void Clear();
    int64_t Push(void * data, int64_t size);
    int64_t Size();
    int64_t Pop(void * targetData, int64_t targetSize);
    
    void * GetData();
    void * CloneData();
private:
    void * m_Buffer;
    int64_t m_Capacity;
    int64_t m_Size;
};

_CORE_END

#endif // #ifndef NCK_QUEUE_BUFFER_H


