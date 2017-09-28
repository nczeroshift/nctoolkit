
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckTexture.h"
#include "nckObject.h"
#include "nckUtils.h"
#include "nckException.h"
#include <stdio.h>

_SCENE_BEGIN

Texture::Texture(Graph::Device *dev) : Datablock(dev){
    m_Device = dev;
    m_Texture = NULL;
}

Texture::~Texture(){
    SafeDelete(m_Texture);
}

DatablockType Texture::GetType(){
    return DATABLOCK_TEXTURE;
}

void Texture::Read(Core::DataReader *f){
    f->Read(&m_Name);
    f->Read(&m_Filename);
    
    try{
        m_Texture = m_Device->LoadTexture("texture://"+m_Filename);
    }
    catch(Core::Exception & ex)
    {
        throw ex;
    }
}

#ifdef NCK_BXON
void Texture::Read(BXON::Map * entry){
    m_Name = entry->GetString("name");
    m_Filename = entry->GetString("filename");
    try{
        m_Texture = m_Device->LoadTexture("texture://"+m_Filename);
    }
    catch(Core::Exception & ex){
        throw ex;
    }
}
#endif

void Texture::SetTexture(Graph::Texture *tex){
    m_Texture = tex;
}

Graph::Texture *Texture::GetTexture(){
    return m_Texture;
}

void Texture::SetFilename(std::string filename){
    m_Filename = filename;
}

std::string Texture::GetFilename(){
    return m_Filename;
}


void Texture::Enable(unsigned int sampler_id){
    if(m_Texture)
        m_Texture->Enable(sampler_id);
}


void Texture::Disable(unsigned int sampler_id){
    if(m_Texture)
        m_Texture->Disable(sampler_id);
}

TextureLayer::TextureLayer(Graph::Device *dev){
    m_Device = dev;
    m_Mapping = NULL;
    m_Texture = NULL;

    m_Factor_Diffuse_Color = 0;
    m_Factor_Alpha = 0;
    m_Factor_Diffuse = 0;
    m_Factor_Translucency = 0;
    m_Factor_Specular = 0;
    m_Factor_Specular_Color = 0;
    m_Factor_Hardness = 0;
    m_Factor_Normal = 0;
    m_Factor_Displacement = 0;
    m_Factor_Shadow = 1.0;

    m_BlendMode = TEX_BLEND_MODE_MIX;
}

TextureLayer::TextureLayer(Texture *tex, Object *manip,
                           Graph::Device *dev){
    m_Device = dev;
    m_Mapping = manip;
    m_Texture = tex;
}

TextureLayer::~TextureLayer(){
    SafeDelete(m_Mapping);
}

void TextureLayer::SetTexture(Texture *tex){
    m_Texture = tex;
}

Texture *TextureLayer::GetTexture(){
    return m_Texture;
}

void TextureLayer::SetMappingObject(Object *obj){
    m_Mapping = obj;
}

Object *TextureLayer::GetMappingObject(){
    return m_Mapping;
}

void TextureLayer::Read(Core::DataReader *f,std::vector<Texture *> * tex_vec)
{
    if(!tex_vec)
        THROW_EXCEPTION("Texture vector is null");
    
    unsigned int id;
    f->Read(&id,sizeof(unsigned int));
    
    if(id < tex_vec->size())
        m_Texture = (*tex_vec)[id];
    else
        THROW_EXCEPTION("Invalid texture ID");
    
    char mapping=0x0;
    f->Read(&mapping,sizeof(char));
    
    if(mapping == 0x1)
    {
        Math::Vec3 offset;
        Math::Vec3 scale;
        
        f->Read(&offset,3*sizeof(float));
        f->Read(&scale,3*sizeof(float));
        
        Object *obj = new Object(m_Device);
        obj->SetPosition(offset);
        obj->SetScale(scale);
        
        m_Mapping = obj;
    }
}

#ifdef NCK_BXON
void TextureLayer::Read(BXON::Map * entry, const std::map<std::string, Datablock *> & tMap){
    std::string texIdName = entry->GetString("texture");
    
    if(tMap.find(texIdName) != tMap.end())
        m_Texture = dynamic_cast<Texture*>(tMap.find(texIdName)->second);
    else
        THROW_EXCEPTION("Invalid texture ID");
    
    std::string mapping = entry->GetString("mapping");
    std::string projection = entry->GetString("projection");
    
    if(entry->HasKey("uv_layer")){
        std::string uvName = entry->GetString("uv_layer");
        if (uvName.size() > 0)
        {
            m_LayerName = uvName;
        }
    }
    
    if(entry->HasKey("offset") && entry->HasKey("scale")){
        BXON::Array * offset = entry->GetArray("offset");
        BXON::Array * scale = entry->GetArray("scale");
        
        Object * obj = new Object(m_Device);
        
        obj->SetPosition(Math::Vec3(offset->GetFloat(0), offset->GetFloat(1), offset->GetFloat(2)));
        obj->SetScale(Math::Vec3(scale->GetFloat(0), scale->GetFloat(1), scale->GetFloat(2)));
        
        m_Mapping = obj;
    }
    
    if (entry->HasKey("blend")) {
        std::string mode = entry->GetString("blend");
        if (mode == "MULTIPLY") {
            m_BlendMode = TEX_BLEND_MODE_MULTIPLY;
        }
    }

    if(entry->HasKey("properties")){
        BXON::Map * properties = entry->GetMap("properties");
        
        m_FactorFlag = 0;
        
        if(properties->HasKey("diffuse_color_factor")){
            m_FactorFlag |= FACTOR_DIFFUSE_COLOR;
            m_Factor_Diffuse_Color = properties->GetFloat("diffuse_color_factor");
        }
        
        if(properties->HasKey("alpha_factor")){
            m_FactorFlag |= FACTOR_ALPHA;
            m_Factor_Alpha = properties->GetFloat("alpha_factor");
        }
        
        if(properties->HasKey("diffuse_factor")){
            m_FactorFlag |= FACTOR_DIFFUSE;
            m_Factor_Diffuse = properties->GetFloat("diffuse_factor");
        }
        
        if(properties->HasKey("translucency_factor")){
            m_FactorFlag |= FACTOR_TRANSLUCENCY;
            m_Factor_Translucency = properties->GetFloat("translucency_factor");
        }
        
        if(properties->HasKey("specular_factor")){
            m_FactorFlag |= FACTOR_SPECULAR;
            m_Factor_Specular = properties->GetFloat("specular_factor");
        }
        
        if(properties->HasKey("specular_color_factor")){
            m_FactorFlag |= FACTOR_SPECULAR_COLOR;
            m_Factor_Specular_Color = properties->GetFloat("specular_color_factor");
        }
        
        if(properties->HasKey("hardness_factor")){
            m_FactorFlag |= FACTOR_HARDNESS;
            m_Factor_Hardness = properties->GetFloat("hardness_factor");
        }
        
        if(properties->HasKey("normal_factor")){
            m_FactorFlag |= FACTOR_NORMAL;
            m_Factor_Normal = properties->GetFloat("normal_factor");
        }
        
        if(properties->HasKey("displacement_factor")){
            m_FactorFlag |= FACTOR_DISPLACEMENT;
            m_Factor_Displacement = properties->GetFloat("displacement_factor");
        }
    }
}
#endif

std::string TextureLayer::GetLayerName() {
    return m_LayerName;
}

uint32_t TextureLayer::GetFactorFlag(){
    return m_FactorFlag;
}

void TextureLayer::SetFactorFlag(uint32_t flags) {
    m_FactorFlag = flags;
}

float TextureLayer::GetShadowFactor() {
    return m_Factor_Shadow;
}

void TextureLayer::SetShadowFactor(float value) {
    m_Factor_Shadow = value;
}

void TextureLayer::Enable(unsigned int sampler_id)
{
    // Apply transformation to texture matrix.
    //	if(m_Mapping)
    //	{
    //		// Clean previous transformations to texture matrix.
    //		m_Device->MatrixMode(Graph::MATRIX_TEXTURE);
    //		m_Device->Identity();
    //
    //		// Enable transformation
    //		m_Mapping->Bind(Graph::MATRIX_TEXTURE);
    //	}
    
    if(m_Texture)
        m_Texture->Enable(sampler_id);
}

void TextureLayer::Disable(unsigned int sampler_id){
    if(m_Texture)
        m_Texture->Disable(sampler_id);
    
    if(m_Mapping)
    {
        // Restore matrix mode to model.
        m_Device->MatrixMode(Graph::MATRIX_MODEL);
    }
}

float TextureLayer::GetFactorDiffuseColor(){
    return m_Factor_Diffuse_Color;
}

float TextureLayer::GetFactorAlpha(){
    return m_Factor_Alpha;
}

float TextureLayer::GetFactorDiffuse(){
    return m_Factor_Diffuse;
}

float TextureLayer::GetFactorTranslucency(){
    return m_Factor_Translucency;
}

float TextureLayer::GetFactorSpecular(){
    return m_Factor_Specular;
}

float TextureLayer::GetFactorSpecularColor(){
    return m_Factor_Specular_Color;
}

float TextureLayer::GetFactorHardness(){
    return m_Factor_Hardness;
}

float TextureLayer::GetFactorNormal(){
    return m_Factor_Normal;
}

float TextureLayer::GetFactorDisplacement(){
    return m_Factor_Displacement;
}


_SCENE_END
