
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckMaterial.h"
#include "nckUtils.h"
#include "nckException.h"
#include <stdio.h>

_SCENE_BEGIN

Material::Material(Graph::Device *dev): Datablock(dev){
    m_AlphaTestValue = 1.0f;
    m_Diffuse = Math::Color4f(1,1,1,1);
    m_Specular = Math::Color4f(1,1,1,1);
    m_Alpha = 1.0;
    m_kDiffuse = 1.0f;
    m_kSpecular = 0.3f;
    m_kShininess = 16.0f;
    m_kAmbient = 0.1f;
    m_AlphaBlending = false;
    m_AlphaTest = false;
    m_CullFlag = true;
    m_CullingMode = Graph::CULL_BACK;
    m_Program = NULL;
    
    for (unsigned int i = 0;i<8;i++)
        m_Layers[i] = NULL;
}

Material::~Material(){
    for (unsigned int i = 0;i<8;i++)
        SafeDelete(m_Layers[i]);
}

DatablockType Material::GetType(){
    return DATABLOCK_MATERIAL;
}

void Material::Read(Core::DataReader *f, std::vector<Texture *> *tex_vec){
    if(!tex_vec)
        THROW_EXCEPTION("Texture vector is null");
    
    f->Read(&m_Name);
    
    f->Read(&m_Diffuse,4*sizeof(float));
    f->Read(&m_Specular,4*sizeof(float));
    f->Read(&m_kDiffuse,sizeof(float));
    f->Read(&m_kSpecular,sizeof(float));
    f->Read(&m_kShininess,sizeof(float));
    f->Read(&m_kAmbient,sizeof(float));
    
    unsigned int layers;
    f->Read(&layers,sizeof(unsigned int));
    
    char hasAction = 0x0;
    f->Read(&hasAction,sizeof(char));
    
    for(unsigned int i = 0;i<layers;i++){
        TextureLayer *tex_layer = new TextureLayer(m_Device);
        
        try{
            tex_layer->Read(f,tex_vec);
        }
        catch(Core::Exception & ex){
            delete tex_layer;
            THROW_EXCEPTION_STACK("Unable to load texture layer",ex);
        }
        
        m_Layers[i] = tex_layer;
    }
}

void Material::Enable(){
    
    if(m_CullFlag){
        m_Device->Enable(Graph::STATE_CULL_FACE);
        m_Device->CullMode(m_CullingMode);
    }
    else
        m_Device->Disable(Graph::STATE_CULL_FACE);
    
    if(m_AlphaTest)
        m_Device->Enable(Graph::STATE_ALPHA_TEST);
    
    Math::Color4f diff = Math::Color4f(m_Diffuse.GetR(), m_Diffuse.GetG(), m_Diffuse.GetB(), m_kDiffuse);
    Math::Color4f spec = Math::Color4f(m_Specular.GetR(), m_Specular.GetG(), m_Specular.GetB(), m_kSpecular); 
    
    m_Device->Material(Graph::MATERIAL_DIFFUSE_COLOR,(float*)&(diff));
    m_Device->Material(Graph::MATERIAL_SPECULAR_COLOR,(float*)&(spec));
    m_Device->Material(Graph::MATERIAL_SHININESS,&m_kShininess);
    m_Device->Material(Graph::MATERIAL_AMBIENT,(float*)&(m_kAmbient));
    m_Device->Material(Graph::MATERIAL_ALPHA, (float*)&(m_Alpha));

    if(m_Program)
        m_Program->Enable();
    
    for(int i = 0;i<8;i++)
        if(!m_Layers[i])
            break;
        else
            m_Layers[i]->Enable(i);
}

void Material::Disable(){
    
    if(m_Program)
        m_Program->Disable();
    
    if(m_AlphaTest)
        m_Device->Disable(Graph::STATE_ALPHA_TEST);
    
    for(int i = 0;i<8;i++)
        if(!m_Layers[i])
            break;
        else
            m_Layers[i]->Disable(i);
}

void Material::SetFlag(MaterialFlag flag, bool enabled){
    switch(flag)
    {
        case MATERIAL_ALPHA_BLENDING:
            m_AlphaBlending = enabled;return;
        case MATERIAL_ALPHA_TEST:
            m_AlphaTest = enabled;return;
        case MATERIAL_FACE_CULLING:
            m_CullFlag = enabled;return;
    }
}

bool Material::GetFlag(MaterialFlag flag){
    switch(flag)
    {
        case MATERIAL_ALPHA_BLENDING:
            return m_AlphaBlending;
        case MATERIAL_ALPHA_TEST:
            return m_AlphaTest;
        case MATERIAL_FACE_CULLING:
            return m_CullFlag;
    }
    return false;
}

Graph::CullFaceMode Material::GetCullMode(){
    return m_CullingMode;
}

void Material::SetCullMode(Graph::CullFaceMode mode){
    m_CullingMode = mode;
}

Math::Color4f Material::GetDiffuse(){
    return m_Diffuse;
}

Math::Color4f Material::GetSpecular(){
    return m_Specular;
}

void Material::SetDiffuse(const Math::Color4f &col){
    m_Diffuse = col;
}

void Material::SetSpecular(const Math::Color4f &col){
    m_Specular = col;
}

float Material::GetKAmbient(){
    return m_kAmbient;
}

float Material::GetKDiffuse(){
    return m_kDiffuse;
}

float Material::GetKSpecular(){
    return m_kSpecular;
}

float Material::GetKShininess(){
    return m_kShininess;
}

void Material::SetKAmbient(float value){
    m_kAmbient = value;
}

void Material::SetKDiffuse(float value){
    m_kDiffuse = value;
}

void Material::SetKSpecular(float value){
    m_kSpecular = value;
}

void Material::SetKShininess(float value){
    m_kShininess=value;
}

void Material::SetTextureLayer(unsigned int layer, TextureLayer *tl){
    if(layer>8)
        return;
    m_Layers[layer]= tl;
    
}

TextureLayer *Material::GetTextureLayer(unsigned int layer){
    if(layer>8)
        NULL;
    return m_Layers[layer];
}

void Material::SetProgram(Graph::Program *prog){
    m_Program  = prog;
}

Graph::Program *Material::GetProgram(){
    return m_Program;
}

#ifdef NCK_BXON

void Material::Read(BXON::Map * entry, const std::map<std::string, Datablock *> & tMap){
    m_Name = entry->GetString("name");
    
    m_Diffuse = entry->GetColor3f("diffuse");
    m_Specular = entry->GetColor3f("specular");

    if (entry->HasKey("alpha")) {
        m_Alpha = entry->GetFloat("alpha");
    }

    if (entry->HasKey("diffuse_intensity")) {
        m_kDiffuse = entry->GetFloat("diffuse_intensity");
    }

    if (entry->HasKey("specular_hardness")) {
        m_kShininess = entry->GetInteger("specular_hardness");
    }

    if (entry->HasKey("specular_intensity")) {
        m_kSpecular = entry->GetFloat("specular_intensity");
    }

    if(entry->HasKey("textures")){
        BXON::Array * textures = entry->GetArray("textures");
        
        for(uint32_t i = 0;i<textures->GetSize();i++){
            TextureLayer * l = new TextureLayer(m_Device);
            l->Read(textures->GetMap(i), tMap);
            m_Layers[i] = l;
        }
        
    }
}

#endif

_SCENE_END
