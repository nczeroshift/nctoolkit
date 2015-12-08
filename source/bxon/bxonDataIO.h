
#ifndef BXON_DATAIO_h
#define BXON_DATAIO_h

#include "bxon.hpp"
#include "nckDataIO.h"

_BXON_BEGIN

class ReaderContext : public BXON::Context{
public:
    ReaderContext(Core::FileReader * reader);
    virtual ~ReaderContext();
    uint32_t Read(int32_t size, uint8_t * d);
    uint32_t Write(int32_t size, uint8_t * d);
    uint64_t Seek(int64_t offset);
    uint64_t Tell();
protected:
    Core::FileReader * reader;
};

_BXON_END

#endif
