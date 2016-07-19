
/**
 * BXON - Binary eXchange Object Notation
 * Under MIT License 
 * Copyright © Luís F. Loureiro
 * https://github.com/nczeroshift/bxon
 * Version ??.??.??
 * 2015-08-05
 */

#ifndef _BXON_HPP_
#define _BXON_HPP_

#include <stdint.h>
#include <string>
#include <map>
#include <vector>

#include "nckVec2.h"
#include "nckVec3.h"
#include "nckVec4.h"
#include "nckQuat.h"
#include "nckColor4.h"
#include "nckMat44.h"

#define _BXON_BEGIN namespace BXON{
#define _BXON_END };

_BXON_BEGIN

// Native Types
typedef enum{
    TYPE_NIL        = 0,
    TYPE_STRING     = 1,
    TYPE_BOOLEAN    = 2,
    TYPE_INT        = 3,
    TYPE_LONG       = 4,
    TYPE_FLOAT      = 5,
    TYPE_DOUBLE     = 6,
    TYPE_BYTE       = 7,
} NativeType;
    
// Length Flag
typedef enum{
    LENGTH_8   = 0x00,
    LENGTH_16  = 0x10,
    LENGTH_32  = 0x20,
    LENGTH_64  = 0x30,
} LengthFlag;
    
// Object Flag
typedef enum{
    FLAG_OBJECT     = 0x00,
    FLAG_ARRAY      = 0x40,
    FLAG_MAP        = 0x80,
} ObjectFlag;

// Exception Codes
typedef enum{
	EXCEPTION_WRONG_TYPE		   = 1,
    EXCEPTION_ARRAY_OUT_OF_BOUNDS  = 2,
    EXCEPTION_ARRAY_NULL_ENTRY     = 3,
    EXCEPTION_READ_FAILURE         = 4,
    EXCEPTION_WRITE_FAILURE        = 5,
    EXCEPTION_MAP_ENTRY_NOT_FOUND  = 6
} ErrorCode;

// Byte Masks
#define BXON_MASK_TYPE      0x0F
#define BXON_MASK_LENGTH    0x30
#define BXON_MASK_FLAG      0xC0

class Context{
public:
    virtual ~Context(){};
    virtual uint32_t Read(int32_t size, uint8_t * d) = 0;
    virtual uint32_t Write(int32_t size, uint8_t * d) = 0;
    virtual uint64_t Seek(int64_t offset) = 0;
    virtual uint64_t Tell() = 0;
};

class Header{
public:
    Header();
    Header(const Header & hdr);
    ~Header();
    
    void SetType(NativeType type);
    NativeType GetType();
    LengthFlag GetLenFlag();
    void SetFlag(ObjectFlag flag);
    ObjectFlag GetFlag();
    void SetLength(int64_t len);
    int64_t GetLength();
    
    void Read(Context * context);
protected:
    uint8_t type;
    uint64_t length;
};

class Array;
class Map;

class Object{
public:
    Object();
    virtual ~Object();
    
    static uint8_t SizeForNative(NativeType type);
    
    NativeType GetType();
    ObjectFlag GetFlag();
    int64_t GetLength();
    
    static Object * Parse(Context * ctx);
    static std::string ToJSON(BXON::Object * obj, int depth = 0, int spaces = 4);
    
protected:
    friend Array;
    friend Map;
    virtual void Read(const Header & hdr, Context * ctx) = 0;
    
    Header header;
    Context * context;
};

class Map;
class Array;

class Native: public Object{
public:
    Native();
    virtual ~Native();
    
    static Native * Nil ();
    static Native * Boolean (bool value);
    static Native * Byte (uint8_t value);
    static Native * Integer (int32_t value);
    static Native * Long (int64_t value);
    static Native * Float (float value);
    static Native * Double (double value);
    static Native * String (const std::string & value);
    
    static Native * WithData(NativeType type, void * data, int length);
    
    bool IsNil();
    bool GetBoolean();
    uint8_t GetByte();
    int32_t GetInt();
    int64_t GetLong();
    float GetFloat();
    double GetDouble();
    std::string GetString();
    
protected:
    void Read(const Header & hdr,Context * ctx);
    void * data;
};

class Array : public Object{
public:
    virtual ~Array();
    
    virtual uint32_t GetSize() = 0;
    virtual uint32_t GetCapacity() = 0;
    
    virtual Object * GetObject (int i) = 0;
    virtual Array * GetArray (int i) = 0;
    virtual Map * GetMap (int i) = 0;
    
    static Array * WithCapacity(uint32_t capacity = 10);
    static Array * WithType(NativeType type, uint32_t capacity = 10);
    
    static Array * WithVec3(const Math::Vec3 & value);
    static Array * WithVec4(const Math::Vec4 & value);
    static Array * WithMat44(const Math::Mat44 & value);
    static Array * WithQuat(const Math::Quat & value);
    static Array * WithColor4f(const Math::Color4f & value);

    virtual bool IsNil (int i) = 0;
    virtual bool GetBoolean (int i) = 0;
    virtual uint8_t GetByte (int i) = 0;
    virtual int32_t GetInteger (int i) = 0;
    virtual int64_t GetLong (int i) = 0;
    virtual float GetFloat (int i) = 0;
    virtual double GetDouble (int i) = 0;
    virtual std::string GetString (int i) = 0;

protected:
    virtual void Read(const Header & hdr,Context * ctx) = 0;
};

class Map : public Object{
public:
    Map();
    virtual ~Map();
    
    uint32_t GetSize();
    std::vector<std::string> GetKeys();
    
    bool HasKey (const std::string & key);
    
    Object * GetObject (const std::string & key);
    Array * GetArray (const std::string & key);
    Map *  GetMap (const std::string & key);
    
    Object * Put(const std::string & key, Object * obj);
   
    bool IsNil (const std::string & key);
    bool GetBoolean (const std::string & key);
    uint8_t GetByte (const std::string & key);
    int32_t GetInteger (const std::string & key);
    int64_t GetLong (const std::string & key);
    float GetFloat (const std::string & key);
    double GetDouble (const std::string & key);
    std::string GetString (const std::string & key);

    Math::Vec2 GetVec2(const std::string & key);
    Math::Vec3 GetVec3(const std::string & key);
    Math::Vec4 GetVec4(const std::string & key);
    Math::Mat44 GetMatrix(const std::string & key);
    Math::Quat GetQuat(const std::string & key);
    Math::Color4f GetColor3f(const std::string & key);
    Math::Color4f GetColor4f(const std::string & key);
    
protected:
    virtual void Read(const Header & hdr,Context * ctx);    
    std::map<std::string, Object*> map;
};

_BXON_END


#endif
