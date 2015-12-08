
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckEffect.h"
#include "nckUtils.h"
#include "nckException.h"
#include "nckDataIO.h"

#define DIRECTION_BLUR_KMAX 16

_SCENE_BEGIN

Effect::Effect(Graph::Device *dev){
    m_Device = dev;
}

Effect::~Effect(){
    
}

Effect_DirectionalBlur::Effect_DirectionalBlur(Graph::Device * dev) : Effect(dev) {
    m_Shader = NULL;
    m_KernelSize = DIRECTION_BLUR_KMAX;
    for(int i = 0;i< m_KernelSize*2+1;i++)
        m_Weights.push_back(1.0f/m_KernelSize);
    m_Type = GAUSSIAN;
    m_Size = Math::Vec2(256.0f,256.0f);
    m_Direction = Math::Vec2(1.0f,0.0f);
}
Effect_DirectionalBlur::~Effect_DirectionalBlur(){SafeDelete(m_Shader);}
Effect_DirectionalBlur * Effect_DirectionalBlur::Create(Graph::Device *dev, WeightType type, int kernelSize){
    Graph::Program * prog = dev->LoadProgram("shader://effects/directional_blur.cpp");
    Effect_DirectionalBlur * ret = new Effect_DirectionalBlur(dev);
    ret->m_Shader = prog;
    ret->m_Type = type;
    ret->m_KernelSize = kernelSize;
    ret->Update(true);
    return ret;
}
void Effect_DirectionalBlur::Enable(){m_Shader->Enable();}
void Effect_DirectionalBlur::Disable(){m_Shader->Disable();}
void Effect_DirectionalBlur::SetKernelSize(int size){m_KernelSize = size;Update(true);}
int Effect_DirectionalBlur::GetKernelSize(){return m_KernelSize;}
Math::Vec2 Effect_DirectionalBlur::GetDirection(){return m_Direction;}
void Effect_DirectionalBlur::SetDirection(const Math::Vec2 & dir){
    m_Direction = dir;
    Update(false);
}
Math::Vec2 Effect_DirectionalBlur::GetSize(){return m_Direction;}
void Effect_DirectionalBlur::SetSize(const Math::Vec2 & size){
    m_Size = size;
    Update(false);
}
Effect_DirectionalBlur::WeightType Effect_DirectionalBlur::GetType(){return m_Type;}
void Effect_DirectionalBlur::SetType(const WeightType & type){m_Type = type;Update(true);}
void Effect_DirectionalBlur::Update(bool weights){
    m_Shader->SetVariable4f("direction",m_Direction.GetX(),m_Direction.GetY());
    m_Shader->SetVariable4f("invImageSize",1.0f/m_Size.GetX(),1.0f/m_Size.GetY());
    if(weights){
        const float max = 1.0;
        const float min = 0.1;
        if(m_Type == BOX){
            float value = 1.0f / (m_KernelSize * 2 + 1);
            for(int i = 0;i < m_KernelSize*2 + 1;i++){
                m_Weights[i] = value;
            }
        }else if(m_Type == QUADRATIC){
            float total = 0.0;
            for(int i = 0;i < m_KernelSize * 2 + 1;i++){
                float val = 1 - powf(fabs((float)m_KernelSize-i)/(float)m_KernelSize,2.0f);
                float sVal = min * (1-val) + max * val;
                total += sVal;
                m_Weights[i] = sVal;
            }
            total = 1.0/total;
            for(int i = 0;i < m_KernelSize*2 + 1;i++)
                m_Weights[i] *= total;
        }else if(m_Type == CUBIC){
            float total = 0.0;
            for(int i = 0;i < m_KernelSize * 2 + 1;i++){
                float val = 1 - powf(fabs((float)m_KernelSize-i)/(float)m_KernelSize,3.0f);
                float sVal = min * (1-val) + max * val;
                total += sVal;
                m_Weights[i] = sVal;
            }
            total = 1.0/total;
            for(int i = 0;i < m_KernelSize * 2 + 1;i++)
                m_Weights[i] *= total;
        }else if(m_Type == GAUSSIAN){
            float total = 0.0;
            float sigma = 0.5; // variance
            for(int i = 0;i < m_KernelSize*2 + 1;i++){
                float val = expf(-powf((fabs((float)m_KernelSize-i)/(float)m_KernelSize)/sigma,2));
                float sVal = min * (1-val) + max * val;
                total += sVal;
                m_Weights[i] = sVal;
            }
            total = 1.0 / total;
            for(int i = 0;i < m_KernelSize*2 + 1;i++)
                m_Weights[i] *= total;
        }else if(m_Type == TENT){
            float total = 0.0;
            for(int i = 0;i < m_KernelSize*2 + 1;i++){
                float val = 1 - fabs((float)m_KernelSize-i)/(float)m_KernelSize;
                float sVal = min * (1-val) + max * val;
                total += sVal;
                m_Weights[i] = sVal;
            }
            total = 1.0/total;
            for(int i = 0;i < m_KernelSize*2 + 1;i++)
                m_Weights[i] *= total;
        }
        m_Shader->SetVariable1f("kernelSize",m_KernelSize);
        float * v = &m_Weights[0];
        m_Shader->SetArray1f("kernelWeights",DIRECTION_BLUR_KMAX*2+1,v);
    }
}

Effect_Invert::Effect_Invert(Graph::Device *dev) : Effect(dev){
    m_Shader = NULL;
}
Effect_Invert::~Effect_Invert(){
    SafeDelete(m_Shader);
}
Effect_Invert * Effect_Invert::Create(Graph::Device *dev){
    Graph::Program * shader = dev->LoadProgram("shader://effects/invert.cpp");
    Effect_Invert * ret = new Effect_Invert(dev);
    ret->m_Shader = shader;
    return ret;
}
void Effect_Invert::Enable(){m_Shader->Enable();}
void Effect_Invert::Disable(){m_Shader->Disable();}

Effect_ChromaticAberration::Effect_ChromaticAberration(Graph::Device *dev) : Effect(dev){
    m_Shader = NULL;
    m_Size = Math::Vec2(256.0f,256.0f);
    m_Offset = Math::Vec2(0.0f,0.0f);
    m_Distortion = Math::Vec4(1.0f,2.0f,3.0f,4.0f);
}
Effect_ChromaticAberration::~Effect_ChromaticAberration(){
    SafeDelete(m_Shader);
}
Effect_ChromaticAberration * Effect_ChromaticAberration::Create(Graph::Device *dev){
    Graph::Program * shader = dev->LoadProgram("shader://effects/chromatic_aberration.cpp");
    Effect_ChromaticAberration * ret = new Effect_ChromaticAberration(dev);
    ret->m_Shader = shader;
    ret->Update();
    return ret;
}
void Effect_ChromaticAberration::Enable(){
    m_Shader->Enable();
}
void Effect_ChromaticAberration::Disable(){
    m_Shader->Disable();
}
Math::Vec2 Effect_ChromaticAberration::GetSize(){return m_Size;}
void Effect_ChromaticAberration::SetSize(const Math::Vec2 & size){m_Size = size;Update();}
Math::Vec4 Effect_ChromaticAberration::GetDistortion(){return m_Distortion;}
void Effect_ChromaticAberration::SetDistortion(const Math::Vec4 & distortion){m_Distortion = distortion;}
Math::Vec2 Effect_ChromaticAberration::GetOffset(){return m_Offset;}
void Effect_ChromaticAberration::SetOffset(const Math::Vec2 & offset){m_Offset = offset;}
void Effect_ChromaticAberration::Update(){
    m_Shader->SetVariable4f("imageSize",m_Size.GetX(),m_Size.GetY());
    m_Shader->SetVariable4f("distortion",m_Distortion.GetX(),m_Distortion.GetY(),m_Distortion.GetZ(),m_Distortion.GetW());
    m_Shader->SetVariable4f("offset",m_Offset.GetX(),m_Offset.GetY());
}

Effect_NormalShading::Effect_NormalShading(Graph::Device *dev) : Effect(dev){
    m_Shader = NULL;
}
Effect_NormalShading::~Effect_NormalShading(){SafeDelete(m_Shader);}
Effect_NormalShading * Effect_NormalShading::Create(Graph::Device *dev, bool textured){
    Graph::Program * shader = NULL;
    if(textured)
        shader = dev->LoadProgram("shader://effects/normal_textured.cpp");
    else
        shader = dev->LoadProgram("shader://effects/normal_solid.cpp");
    Effect_NormalShading * ret = new Effect_NormalShading(dev);
    ret->m_Shader = shader;
    return ret;
}
void Effect_NormalShading::Enable(){
    m_Shader->Enable();
}
void Effect_NormalShading::Disable(){
    m_Shader->Disable();
}

_SCENE_END
