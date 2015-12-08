
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_DATABLOCK_H
#define NCK_DATABLOCK_H

#include "nckSceneConfig.h"
#include "nckGraphics.h"

_SCENE_BEGIN

/// Available datablocks types.
enum DatablockType
{
    DATABLOCK_OBJECT,
    DATABLOCK_MODEL,
    DATABLOCK_LAMP,
    DATABLOCK_MATERIAL,
    DATABLOCK_TEXTURE,
    DATABLOCK_CURVE,
    DATABLOCK_ARMATURE,
    DATABLOCK_CAMERA,
    DATABLOCK_COMPOUND,
};

/**
 * Abstract class for each element imported from the compound file.
 */
class Datablock
{
public:
    /// Default constructor.
    Datablock(Graph::Device *dev);
    
    /// Destructor.
    virtual ~Datablock(){}
    
    /// Get datablock type.
    virtual DatablockType GetType()=0;
    
    /// Get datablock name.
    std::string GetName();
    
    /// Set datablock name.
    void SetName(std::string name);
    
protected:
    
    // Datablock name.
    std::string m_Name;
    
    /// Reference to graphics device.
    Graph::Device *m_Device;
};

_SCENE_END

#endif // #ifndef NCK_DATABLOCK_H
