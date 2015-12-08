
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_LAMP_H
#define NCK_LAMP_H

#include "nckColor4.h"
#include "nckObject.h"

_SCENE_BEGIN

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
    
    /// Ajustar as propriedades
    //void SetFlags(unsigned int flags);
    
    /// Obter as propriedades
    //unsigned int GetFlags();
    
    /// Set lamp transformation manipulator.
    void	SetObject(Object *obj);
    
    /// Get lamp transformation manipulator.
    Object *GetObject();
    
    /// Get datablock type.
    DatablockType GetType();
    
private:
    
    /// Reference to graphics device.
    Graph::Device *m_gDevice;
    
    /// Lamp transformation manipulator.
    Object *m_Object;
    
    /// Lamp color.
    Math::Color4f m_Color;
    
    /// Lamp power factor.
    float m_Energy;
    
    /// Lamp range.
    float m_Distance;
    
};

_SCENE_END

#endif // #ifndef NCK_LAMP_H
