
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
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
#include "nckGeometry.h"
#include "nckCurve.h"
#include <list>

_SCENE_BEGIN

/**
* Layers mask flags.
*/

enum LayerMask{
    LAYER_ALL = 0xFFFFFFFF,
    LAYER_1 = 1 << 0,
    LAYER_2 = 1 << 1,
    LAYER_3 = 1 << 2,
    LAYER_4 = 1 << 3,
    LAYER_5 = 1 << 4,
    LAYER_6 = 1 << 5,
    LAYER_7 = 1 << 6,
    LAYER_8 = 1 << 7,
    LAYER_9 = 1 << 8,
    LAYER_10 = 1 << 9,
    LAYER_11 = 1 << 10,
    LAYER_12 = 1 << 11,
    LAYER_13 = 1 << 12,
    LAYER_14 = 1 << 13,
    LAYER_15 = 1 << 14,
    LAYER_16 = 1 << 15,
    LAYER_17 = 1 << 16,
    LAYER_18 = 1 << 17,
    LAYER_19 = 1 << 18,
    LAYER_20 = 1 << 19,
};

/**
* Compound rendering statistics.
*/
class RenderStatistics {
public:
    RenderStatistics() { checkedObjects = 0; renderedObjects = 0; }
    RenderStatistics(int checked, int rendered) { checkedObjects = checked; renderedObjects = rendered; }
    int GetCheckedObjects() { return checkedObjects; }
    int GetRenderedObjects() { return renderedObjects; }
private:
    int checkedObjects;
    int renderedObjects;
};

/**
* Abstract class to load and store scene elements.
*/
class Compound : public Datablock
{
public:
    /// Constructor.
    Compound(Graph::Device *dev) : Datablock(dev){};
    
    /// Destructor.
    virtual ~Compound();
    
    /// Load compound data from stream in "nc" file format.
    /// @throws Exception
    bool Read(Core::DataReader *f);
    
#ifdef NCK_BXON
    /// Load compound data from BXON file data.
    /// @param entry Reference to BXON map with compound scene data.
    /// @param processor Reference to pre/post process methods implementation class.
    /// @throws Exception
    bool ReadBXON(BXON::Map * entry, Processor * processor = NULL);
#endif
    
    /// Get datablock reference by type and name.
    Datablock *GetDatablock(DatablockType type, std::string name);
    
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
    
    /// List of curves datablocks.
    std::list<Curve*> m_Curves;

    /// List of armatures datablocks.
    std::list<Armature*> m_Armatures;
};


/**
* Base compound implementation, for rendering and other utilities.
*/
class Compound_Base : public Compound
{
public:
    /// Constructor.
    Compound_Base(Graph::Device * dev);
    
    /// Destructor.
    virtual ~Compound_Base();
    
    /// Render compound models.
    /// @param fr Reference to frustum used to clip object rendering.
    /// @param overlap Reference to material used to replace all
    /// @param layer_mask Filter model rendering by layers
    /// models materials.
    RenderStatistics Render(Math::Frustum * fr = NULL, Material * overlap = NULL, int layer_mask = LAYER_ALL);
    
    /// Load data data from filename.
    /// @throws Exception
    void Load(const std::string & filename, Processor * processor = NULL);
    
    /// Load 
    void Load(BXON::Map * entry, Processor * processor = NULL);
    
    /// Get the boundbox wrapping all elements.
    Math::BoundBox GetBoundBox();
    
    int GetEmptyObjects(std::vector<Object*> * objects, int layer_mask = LAYER_ALL);
    int GetObjectsWithLayer(std::vector<Object*> * datablocks, int layer_mask = LAYER_ALL);
    int GetObjectsWithLayer(std::vector<Object*> * datablocks, DatablockType type, int layer_mask = LAYER_ALL);

    int GetAllMaterials(std::vector<Material*> * materials);
    int GetAllTextures(std::vector<Texture*> * textures);

    Object * GetObject(const std::string & name);
    Material * GetMaterial(const std::string & name);
    Model * GetModel(const std::string & name);
    Texture * GetTexture(const std::string & name);
    Armature * GetArmature(const std::string & name);
    Curve * GetCurve(const std::string & name);
    Camera * GetCamera(const std::string & name);

    static std::vector< std::pair<float, Scene::Object*> > fetchCamerasWithKeyframes(BXON::Map * map, Compound_Base * compound);
private:
    Math::BoundBox m_Boundbox;
    
    /// List of objects assigned with models.
    std::list<Object*> m_MObjects;
};


_SCENE_END

#endif // #ifndef NCK_COMPOUND_H
