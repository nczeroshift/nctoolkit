
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_MATERIAL_H
#define NCK_MATERIAL_H

#include "nckColor4.h"
#include "nckTexture.h"
#include "nckDataIO.h"

_SCENE_BEGIN

enum MaterialPass{
	MATERIAL_PASS_DEFAULT,
	MATERIAL_PASS_SHADOW,
};

/// Material flags.
enum MaterialFlag{
    MATERIAL_ALPHA_BLENDING = 1 << 0,   // Use alpha blending for material transparency.
    MATERIAL_ALPHA_TEST     = 1 << 1,   // Use alpha test for billboards.
    MATERIAL_FACE_CULLING   = 1 << 2,   // Set material face culling state.
    MATERIAL_SHADELESS      = 1 << 3,   // Don't use lighting
    MATERIAL_USE_SHADOWS     = 1 << 4,   // Use shadows
};

/// Material properties.
class Material : public Datablock
{
public:
    /// Constructor.
    Material(Graph::Device *dev);
    
    /// Constructor.
    Material(std::string name,Graph::Device *dev);
    
    /// Destructor.
    ~Material();
    
    /**
     * Load material data from file.
     * @param f Reference to file stream.
     * @param tex_vec Reference to loaded textures vector.
     * @throws Exception.
     */
    void Read(Core::DataReader *f, std::vector<Texture *> *tex_vec);
    
#ifdef NCK_BXON
    void Read(BXON::Map * entry, const std::map<std::string, Datablock *> & tMap);
#endif
    
    /// Enable material.
    void Enable(MaterialPass pass = MATERIAL_PASS_DEFAULT);
    
    /// Disable material.
    void Disable(MaterialPass pass = MATERIAL_PASS_DEFAULT);
    
    /// Set material state flags.
    void SetFlag(MaterialFlag flag, bool enabled);
    
    /// Get material state flags.
    bool GetFlag(MaterialFlag flag);
    
    bool GetUseShadows();

    void SetUseShadows(bool flag);

    /// Get face culling mode.
    Graph::CullFaceMode GetCullMode();
    
    /// Set face culling mode.
    void SetCullMode(Graph::CullFaceMode mode);
    
    /// Get diffuse color.
    Math::Color4f GetDiffuse();
    
    /// Get specular color.
    Math::Color4f GetSpecular();
    
    /// Set diffuse color.
    void SetDiffuse	(const Math::Color4f &col);
    
    /// Set specular color.
    void SetSpecular(const Math::Color4f &col);
    
    // Set transparency.
    void SetAlpha(float alpha);

    // Set transparency.
    float GetAlpha();

    /// Get ambient blending factor.
    float GetKAmbient();
    
    /// Get diffuse blending factor.
    float GetKDiffuse();
    
    /// Get specular blending factor.
    float GetKSpecular();
    
    /// Get specular expoent factor.
    float GetKShininess();
    
    /// Set ambient blending factor.
    void SetKAmbient(float value);
    
    /// Set diffuse blending factor.
    void SetKDiffuse(float value);
    
    /// Set specular blending factor.
    void SetKSpecular(float value);
    
    /// Set specular expoent factor.
    void SetKShininess(float value);
    
    /// Assign texture layer to a slot (Max 8).
    void SetTextureLayer(unsigned int layer, TextureLayer *tl);
    
    /// Returns texture layer from a slot.
    TextureLayer *GetTextureLayer(unsigned int layer);
    
    /// Set shader program.
    void SetProgram(Graph::Program *prog, MaterialPass pass = MATERIAL_PASS_DEFAULT);
    
    /// Get shader program.
    Graph::Program *GetProgram(MaterialPass pass = MATERIAL_PASS_DEFAULT);
    
    /// Get datablock type.
    DatablockType GetType();
    
private:
	std::map<MaterialPass, Graph::Program*> m_MappedPrograms;

    /// Reference to shader program.
    //Graph::Program *m_Program;
    
    /// Diffuse color.
    Math::Color4f	m_Diffuse;
    
    /// Specular color.
    Math::Color4f	m_Specular;
    
    /// Diffuse blending factor.
    float m_kDiffuse;
    
    /// Ambient blending factor.
    float m_kAmbient;
    
    /// Specular blending factor.
    float m_kSpecular;
    
    /// Material specular expoent factor.
    float m_kShininess;
    
    /// Transparency value.
    float m_Alpha;

    /// Material alpha blending flag.
    bool m_AlphaBlending;
    
    /// Material alpha test flag.
    bool m_AlphaTest;
    
    /// Material alpha test value.
    float m_AlphaTestValue;
    
    /// Material face culling flag.
    bool m_CullFlag;
    
    bool m_Shadeless;

    bool m_UseShadows;

    /// Material face culling mode.
    Graph::CullFaceMode m_CullingMode;
    
    /// Texture layers references.
    TextureLayer	*m_Layers[8];
};

_SCENE_END

#endif // #ifndef NCK_MATERIAL_H
