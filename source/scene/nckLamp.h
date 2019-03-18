
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_LAMP_H
#define NCK_LAMP_H

#include "nckColor4.h"
#include "nckObject.h"

_SCENE_BEGIN

#define LAMP_MAX 32

enum LampType {
    LAMP_TYPE_POINT = 0,
    LAMP_TYPE_SPOT  = 1,
    LAMP_TYPE_SUN   = 2,
    LAMP_TYPE_AREA  = 3,
};

class Lamp;

/**
* Collection of lamp variables to the shaders.
*/
class LampUniforms {
public:
    LampUniforms();
    ~LampUniforms();
    void Bind(Graph::Program * program);
private:
    Math::Vec4 lamp_pos[8];     // position, energy
    Math::Vec4 lamp_dir[8];     // normal, type (0 - point, 1 - spot, 2 - sun, 3 -area) 
    Math::Vec4 lamp_color[8];   // rgb, distance
    Math::Vec4 lamp_params[8];  // ????
    friend class Lamp;
};

/**
 * Generic representation of a light source.
 */
class Lamp : public Datablock
{
public:
    /// Constructor.
    Lamp(Graph::Device *dev);
    
    /// Destructor.
    ~Lamp();
    
    /// Load data from file.
    /// @param f Reference to file stream.
    void Read(Core::DataReader *f);
    
#ifdef NCK_BXON
    void Read(BXON::Map * entry);
#endif
    
    /// Set lamp color.
    void SetColor(const Math::Color4f &col);
    
    /// Get lamp color.
    Math::Color4f GetColor();
    
    /// Get lamp power factor.
    float GetEnergy();
    
    /// Set lamp power factor.
    void SetEnergy(float nrg);
    
    /// Get lamp max range.
    float GetDistance();
    
    /// Set lamp max range.
    void SetDistance(float ra);
    
    /// Set lamp transformation manipulator.
    void SetObject(Object *obj);
    
    /// Get lamp transformation manipulator.
    Object *GetObject();
    
    /// Get datablock type.
    DatablockType GetType();
    
    LampType GetLampType();

    static void GenerateUniforms(std::vector<Object*> lampObjs, const Math::Mat44 & modelView, LampUniforms * out);
    static void BindShadow(Graph::Program * p,const Math::Mat44 & lampPVM, int bufferSize, float bias);

    float GetSpotSize();
    float GetSpotBlend();
    float GetClipStart();
    float GetClipEnd();

	bool CastShadow();

	Math::Vec2 GetAreaSize();

	Graph::Texture * GetTexture();
	void SetTexture(Graph::Texture * tex);

private:
    
	Graph::Texture * m_Texture;

    /// Reference to graphics device.
    Graph::Device *m_gDevice;
    
    /// Lamp transformation manipulator.
    Object *m_Object;
    
    /// Lamp color.
    Math::Color4f m_Color;
    
	bool m_CastShadows;

    /// Lamp power factor.
    float m_Energy;
    
    /// Lamp range.
    float m_Distance;

    LampType m_LampType;

    float m_Spot_Size;
    float m_Spot_Blend;

    float m_Clip_Start;
    float m_Clip_End;

	Math::Vec2 m_AreaSize;
};

_SCENE_END

#endif // #ifndef NCK_LAMP_H
