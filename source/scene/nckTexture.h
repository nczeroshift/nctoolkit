
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_TEXTURE_H
#define NCK_TEXTURE_H

#include "nckGraphics.h"
#include "nckDatablock.h"
#include "nckDataIO.h"
#include "bxon.hpp"
#include <vector>
#include <string>

_SCENE_BEGIN

/// Encapsulator class for the GphTexture.
class Texture : public Datablock
{
public:
    /// Constructor.
    Texture(Graph::Device *dev);
    
    /// Destructor.
    ~Texture();
    
    /// Set texture reference.
    void SetTexture(Graph::Texture *tex);
    
    /// Get texture reference.
    Graph::Texture *GetTexture();
    
    /// Set texture filename.
    void SetFilename(std::string filename);
    
    /// Get texture filename.
    std::string GetFilename();
    
    /// Read texture data from file.
    /// @param f Reference to file stream.
    /// @throws Exception.
    void Read(Core::DataReader *f);
    
#ifdef NCK_BXON
    void Read(BXON::Map * entry);
#endif
    
    /// Assign the texture to a sampler.
    void Enable(unsigned int sampler_id = 0);
    
    /// Disable the texture to a sampler.
    void Disable(unsigned int sampler_id = 0);
    
    /// Get datablock type.
    DatablockType GetType();
    
private:
    /// Reference to graphics device texture.
    Graph::Texture *m_Texture;
    
    /// texture filename.
    std::string m_Filename;
};

class Object;

enum FactorFlag{
    FACTOR_DIFFUSE_COLOR  = 1,
    FACTOR_ALPHA          = 2,
    FACTOR_DIFFUSE        = 4,
    FACTOR_TRANSLUCENCY   = 8,
    FACTOR_SPECULAR       = 16,
    FACTOR_SPECULAR_COLOR = 32,
    FACTOR_HARDNESS       = 64,
    FACTOR_NORMAL         = 128,
    FACTOR_DISPLACEMENT   = 256
};

/// Texture layer properties.
class TextureLayer
{
public:
    /// Constructor.
    TextureLayer(Graph::Device *dev);
    
    /// Constructor.
    TextureLayer(Texture * tex, Object * manip, Graph::Device * dev);
    
    /// Destructor.
    /// Note : Deletes the texture manipulator
    /// object from memory.
    ~TextureLayer();
    
    /// Set the texture reference.
    void SetTexture(Texture *tex);
    
    /// Get the texture reference.
    Texture *GetTexture();
    
    /// Set the mapping object reference.
    void SetMappingObject(Object *obj);
    
    /// Get the mapping object reference.
    Object *GetMappingObject();
    
    /// Load texture layer data from file.
    /// @throws Core::Exception
    void Read(Core::DataReader *f,std::vector<Texture *> * tex_vec);
    
#ifdef NCK_BXON
    void Read(BXON::Map * entry, const std::map<std::string, Datablock *> & tMap);
#endif
    
    /// Assign the texture to a sampler.
    void Enable(unsigned int sampler_id = 0);
    
    /// Disable the texture to a sampler.
    void Disable(unsigned int sampler_id = 0);
    
    uint32_t GetFactorFlag();
    
    float GetFactorDiffuseColor();
    float GetFactorAlpha();
    float GetFactorDiffuse();
    float GetFactorTranslucency();
    float GetFactorSpecular();
    float GetFactorSpecularColor();
    float GetFactorHardness();
    float GetFactorNormal();
    float GetFactorDisplacement();
    
private:
    
    float m_Factor_Diffuse_Color;
    float m_Factor_Alpha;
    float m_Factor_Diffuse;
    float m_Factor_Translucency;
    float m_Factor_Specular;
    float m_Factor_Specular_Color;
    float m_Factor_Hardness;
    float m_Factor_Normal;
    float m_Factor_Displacement;
    
    uint32_t m_FactorFlag;
    
    /// Texture mapping transformation data.
    Object	*m_Mapping;
    
    /// Reference to texture.
    Texture *m_Texture;
    
    /// Reference to the graphics device.
    Graph::Device *m_Device;
};

_SCENE_END

#endif // #ifndef NCK_TEXTURE_H
