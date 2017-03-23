
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_EFFECT_H
#define NCK_EFFECT_H

#include "nckSceneConfig.h"
#include "nckGraphics.h"
#include "nckVec2.h"
#include "nckVec4.h"
#include <vector>
#include <string>

_SCENE_BEGIN

/// Abstract class for effects implementation.
class Effect{
public:
    /// Default constructor.
    Effect(Graph::Device *dev);
    
    /// Destructor.
    virtual ~Effect();
    
    /// Enable active effect.
    virtual void Enable() = 0;
    
    /// Disable active effect.
    virtual void Disable() = 0;
    
protected:
    /// Graphics device reference.
    Graph::Device * m_Device;
};

/// Weighted pixel sum effect for postprocessing.
class Effect_DirectionalBlur : public Effect{
public:
    enum WeightType{BOX,GAUSSIAN,TENT,QUADRATIC,CUBIC,};
    static Effect_DirectionalBlur * Create(Graph::Device *dev, WeightType type, int kernelSize);
    Effect_DirectionalBlur(Graph::Device *dev);
    virtual ~Effect_DirectionalBlur();
    void Enable();
    void Disable();
    void SetKernelSize(int size);
    int GetKernelSize();
    Math::Vec2 GetDirection();
    void SetDirection(const Math::Vec2 & dir);
    Math::Vec2 GetSize();
    void SetSize(const Math::Vec2 & size);
    WeightType GetType();
    void SetType(const WeightType & type);
protected:
    Graph::Program * m_Shader;
    Math::Vec2 m_Direction;
    Math::Vec2 m_Size;
    int m_KernelSize;
    WeightType m_Type;
    std::vector<float> m_Weights;
    void Update(bool weights);
};

/// Color invertion for postprocessing.
class Effect_Invert : public Effect {
public:
    Effect_Invert(Graph::Device *dev);
    virtual ~Effect_Invert();
    static Effect_Invert * Create(Graph::Device *dev);
    void Enable();
    void Disable();
protected:
    Graph::Program * m_Shader;
};

/// Chromatic aberration for postprocessing.
class Effect_ChromaticAberration : public Effect {
public:
    Effect_ChromaticAberration(Graph::Device *dev);
    virtual ~Effect_ChromaticAberration();
    static Effect_ChromaticAberration * Create(Graph::Device *dev);
    void Enable();
    void Disable();
    Math::Vec2 GetSize();
    void SetSize(const Math::Vec2 & size);
    Math::Vec4 GetDistortion();
    void SetDistortion(const Math::Vec4 & distortion);
    Math::Vec2 GetOffset();
    void SetOffset(const Math::Vec2 & offset);
protected:
    void Update();
    Graph::Program * m_Shader;
    Math::Vec2 m_Size;
    Math::Vec4 m_Distortion;
    Math::Vec2 m_Offset;
};

/// Normal shading for 3D models rendering.
class Effect_NormalShading: public Effect{
public:
    Effect_NormalShading(Graph::Device *dev);
    virtual ~Effect_NormalShading();
    static Effect_NormalShading * Create(Graph::Device *dev, bool textured = true);
    void Enable();
    void Disable();
protected:
    Graph::Program * m_Shader;
};

_SCENE_END

#endif // #ifndef NCK_EFFECT_H
