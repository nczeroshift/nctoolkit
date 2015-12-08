
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_COMPOUND_H
#define NCK_COMPOUND_H

#include "nckObject.h"
#include "nckModel.h"
#include "nckCamera.h"
#include "nckFrustum.h"
#include "nckLamp.h"
#include "nckArmature.h"
#include <list>

_SCENE_BEGIN

//-------------------------------------------------------------------

/// Abstract class for data loading from the compound (*.nc) files.
class Compound : public Datablock
{
public:
    /// Constructor.
    Compound(Graph::Device *dev) : Datablock(dev){};
    
    /// Destructor.
    virtual ~Compound();
    
    /// Load compound data from data stream.
    /// @throws Exception
    bool Read(Core::DataReader *f);
    
#ifdef NCK_BXON
    bool ReadBXON(BXON::Map * entry);
#endif
    
    /// Get datablock reference by type and name.
    Datablock *GetDatablock(DatablockType type,
                            std::string name);
    
    /// Get datablock type.
    DatablockType GetType();
    
protected:
    
    /// List of objects datablocks
    std::list<Object*> m_Objects;
    
    /// List of textures datablocks.
    std::list<Texture*> m_Textures;
    
    /// List of materials datablocks.
    std::list<Material*> m_Materials;
    
    /// List of models datablocks.
    std::list<Model*> m_Models;
    
    /// List of cameras datablocks.
    std::list<Camera*> m_Cameras;
    
    /// List of lamps datablocks.
    std::list<Lamp*> m_Lamps;
    
    /// List of armatures datablocks.
    std::list<Armature*> m_Armatures;
};

//-------------------------------------------------------------------

/// Basic compound implementation for loading and rendering.
class Compound_Basic : public Compound
{
public:
    /// Constructor.
    Compound_Basic(Graph::Device * dev);
    
    /// Destructor.
    ~Compound_Basic();
    
    /// Render compound models.
    /// @param fr Reference to frustum used to clip object rendering.
    /// @param overlap Reference to material used to replace all
    /// models materials.
    void Render(Math::Frustum * fr = NULL, Material * overlap = NULL);
    
    /// Load models data from file.
    /// @throws Exception
    void Load(const std::string & filename);
    
    void Load(BXON::Map * entry);
    
    Math::BoundBox GetBoundBox();
    
private:
    Math::BoundBox m_Boundbox;
    
    /// List of objects assigned with models.
    std::list<Object*> m_MObjects;
};

//-------------------------------------------------------------------

_SCENE_END

#endif // #ifndef NCK_COMPOUND_H
