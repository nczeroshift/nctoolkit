
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckLamp.h"
#include "nckUtils.h"

_SCENE_BEGIN

LampUniforms::LampUniforms() {

}

LampUniforms::~LampUniforms() {

}

void LampUniforms::Bind(Graph::Program * program) {
    program->SetArray4f("lamp_pos_mv", 8, (float*)lamp_pos);
    program->SetArray4f("lamp_dir_mv", 8, (float*)lamp_dir);
    program->SetArray4f("lamp_color", 8, (float*)lamp_color);
    program->SetArray4f("lamp_params", 8, (float*)lamp_params);
}

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

void Lamp::GenerateUniforms(std::vector<Object*> lampObjs, const Math::Mat44 & modelView, LampUniforms * out) {
    for (size_t i = 0; i < lampObjs.size() && i < 8; i++) {
        Scene::Object * obj = lampObjs[i];
        Scene::Lamp * lamp = dynamic_cast<Scene::Lamp*>(obj->GetData());

        Math::Vec4 pMV = Math::Vec4(obj->GetPosition(), 1.0) * modelView;
        Math::Vec4 dMV = Math::Vec4(0, 0, 0, 0);

        if (lamp->GetLampType() == Scene::LAMP_TYPE_SPOT || lamp->GetLampType() == Scene::LAMP_TYPE_SUN)
            dMV = (Math::Vec4(0, 0, 1, 0) * obj->GetMatrix()) * modelView;

        out->lamp_pos[i] = Math::Vec4(Math::Vec3(pMV), lamp->GetEnergy());
        out->lamp_dir[i] = Math::Vec4(Math::Vec3(dMV), (float)lamp->GetLampType());
        out->lamp_color[i] = Math::Vec4(lamp->GetColor().GetR(), lamp->GetColor().GetG(), lamp->GetColor().GetB(), lamp->GetDistance());
        out->lamp_params[i] = Math::Vec4();
    }

    if (lampObjs.size() < 8)
        out->lamp_pos[lampObjs.size()] = Math::Vec4(0, 0, 0, -1); // Early terminator.
}

void Lamp::BindShadow(Graph::Program * p, const Math::Mat44 & lampPVM, int bufferSize, float bias) {
    // Set lamp projection model view matrix
    p->SetMatrix("shadow_pmv", (float*)&lampPVM);

    // Set shadow sampling properties.
    p->SetVariable4f("shadow_params", bufferSize, bufferSize, bias, 0);
}

_SCENE_END
