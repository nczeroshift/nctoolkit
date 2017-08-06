
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckLamp.h"
#include "nckUtils.h"

_SCENE_BEGIN

Lamp::Lamp(Graph::Device *dev): Datablock(dev){
    m_Distance = 10.0;
    m_Energy = 10.0;
    m_Color = Math::Color4f(1,1,1,1);
    m_Object = NULL;
    m_gDevice = NULL;
    m_Clip_End = 20;
    m_Clip_Start = 0;
    m_Spot_Size = 45;
    m_Spot_Blend = 0.150;
    m_LampType = LAMP_TYPE_POINT;
}

Lamp::~Lamp()
{
    
}

void Lamp::Read(Core::DataReader *f)
{
    f->Read(&m_Name);
    
    f->Read(&m_Distance,sizeof(float));
    f->Read(&m_Energy,sizeof(float));
    f->Read(&m_Color,sizeof(Math::Color4f));
    
    char hasAction = 0x0;
    f->Read(&hasAction,sizeof(char));
}

#ifdef NCK_BXON

void Lamp::Read(BXON::Map * entry){
    m_Name = entry->GetString("name");
    m_Distance = entry->GetFloat("distance");
    m_Energy = entry->GetFloat("energy");
    m_Color = entry->GetColor3f("color");

    if (entry->HasKey("type"))
        m_LampType = (LampType)entry->GetInteger("type");
    
    if (entry->HasKey("clip_start")) 
        m_Clip_Start = entry->GetFloat("clip_start");

    if (entry->HasKey("clip_end"))
        m_Clip_End = entry->GetFloat("clip_end");

    if (entry->HasKey("spot_blend"))
        m_Spot_Blend = entry->GetFloat("spot_blend");

    if (entry->HasKey("spot_size"))
        m_Spot_Size = entry->GetFloat("spot_size");
}

#endif

void Lamp::SetColor(const Math::Color4f &col)
{
    m_Color = col;
}

float Lamp::GetEnergy()
{
    return m_Energy;
}

void Lamp::SetEnergy(float nrg)
{
    m_Energy = nrg;
}

float Lamp::GetDistance()
{
    return m_Distance;
}

void Lamp::SetDistance(float dist)
{
    m_Distance = dist;
}

Math::Color4f Lamp::GetColor()
{
    return m_Color;
}

void Lamp::SetObject(Object *obj)
{
    m_Object = obj;
}

Object *Lamp::GetObject()
{
    return m_Object;
}

DatablockType Lamp::GetType()
{
    return DATABLOCK_LAMP;
}


LampType Lamp::GetLampType() {
    return m_LampType;
}

float Lamp::GetSpotSize() {
    return m_Spot_Size;
}

float Lamp::GetSpotBlend() {
    return m_Spot_Blend;
}

float Lamp::GetClipStart() {
    return m_Clip_Start;
}

float Lamp::GetClipEnd() {
    return m_Clip_End;
}

_SCENE_END
