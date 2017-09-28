
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckMathUtils.h"
#include "nckObject.h"
#include "nckUtils.h"
#include "nckException.h"
#include <stdio.h>

_SCENE_BEGIN

Object::Object(Graph::Device *dev) : Datablock(dev){
    m_Name = "temp";
    m_Parent = NULL;
    m_Local = Math::Vec3(0,0,0);
    m_Rotation = Math::Quat(0,0,0,1);
    m_Scale = Math::Vec3(1,1,1);
    m_Data = NULL;
    m_Layer = 0;
    m_Visible = true;
}

Object::~Object(){
    MapFor(std::string, AnimationTrack*, m_Tracks, i){
        delete i->second;
    }
}

Object *Object::GetParent(){
    return m_Parent;
}

void Object::SetParent(Object *parent){
    m_Parent = parent;
}

void Object::Update()
{
    Math::Mat44 tmp = Math::Scale(m_Scale) * Math::Rotate(m_Rotation) * Math::Translate(m_Local);
    m_Matrix = tmp;
}

void Object::Bind(Graph::MatrixType type)
{
    if (m_Parent)
        m_Parent->Bind(type);

    m_Device->MatrixMode(type);
    m_Device->MultMatrix((float*)&m_Matrix);
}

void Object::Read(Core::DataReader * f, std::vector<Object *> * obj_vec,
                  std::vector<Datablock*> *mod_vec,
                  std::vector<Datablock*> *cam_vec,
                  std::vector<Datablock*> *lmp_vec,
                  std::vector<Datablock*> *arm_vec){
    
    if(!obj_vec)
        THROW_EXCEPTION("Object vector is null");
    
    f->Read(&m_Name);
    
    f->Read(&m_Local,3*sizeof(float));
    f->Read(&m_Rotation,4*sizeof(float));
    f->Read(&m_Scale,3*sizeof(float));
    
    Update();
    
    // Load parent reference.
    int parent_id = -1;
    f->Read(&parent_id,sizeof(int));
    if(parent_id>=0)
        m_Parent = (*obj_vec)[parent_id];
    
    char hasAction = 0x0;
    f->Read(&hasAction,sizeof(char));
    
    //	if(hasAction)
    //	{
    //		m_Action = new ObjectAction();
    //		m_Action->Read(f);
    //	}
    
    DatablockType convert_types[4] = {DATABLOCK_MODEL,DATABLOCK_CAMERA, DATABLOCK_LAMP,DATABLOCK_ARMATURE};
    
    int data_type = -1;
    f->Read(&data_type,sizeof(int));
    
    if(data_type!= -1)
    {
        int data_id = -1;
        f->Read(&data_id,sizeof(int));
        
        switch(convert_types[data_type])
        {
            case DATABLOCK_MODEL:
                m_Data = (*mod_vec)[data_id];
                break;
                
            case DATABLOCK_CAMERA:
                m_Data = (*cam_vec)[data_id];
                break;
                
            case DATABLOCK_LAMP:
                m_Data = (*lmp_vec)[data_id];
                break;
                
            case DATABLOCK_ARMATURE:
                m_Data = (*arm_vec)[data_id];
                break;
                
            default:
                break;
        }
    }
}


void Object::Parse(BXON::Map * entry, const std::map<std::string, Object *> & objMap,
                   const std::map<std::string, Datablock*> & modMap,
                   const std::map<std::string, Datablock*> & camMap,
                   const std::map<std::string, Datablock*> & lampMap,
                   const std::map<std::string, Datablock*> & armMap,
                    const std::map<std::string, Datablock*> & curMap){
    m_Name = entry->GetString("name");
    
    m_Local = entry->GetVec3("position");
    m_Rotation = entry->GetQuat("quaternion");
    m_Scale = entry->GetVec3("scale");
    
    if (entry->HasKey("layer"))
        m_Layer = entry->GetInteger("layer");

    if (entry->HasKey("parent")) {
        std::string parentId = entry->GetString("parent");
        m_Parent = objMap.find(parentId)->second;
    }

    if(entry->HasKey("datablock"))
    {
        BXON::Map * db = entry->GetMap("datablock");
        
        if(db->HasKey("type") && db->HasKey("name"))
        {
            std::string dbType = db->GetString("type");
            std::string dbName = db->GetString("name");
            
            if(dbType == "mesh" && modMap.find(dbName) != modMap.end() ){
                m_Data = modMap.find(dbName)->second;
            }else if(dbType == "camera" && camMap.find(dbName) != camMap.end()){
                m_Data = camMap.find(dbName)->second;
            }else if(dbType == "lamp" && lampMap.find(dbName) != lampMap.end()){
                m_Data = lampMap.find(dbName)->second;
            }else if(dbType == "armature" && armMap.find(dbName) != armMap.end()){
                m_Data = armMap.find(dbName)->second;
            }else if (dbType == "camp" && curMap.find(dbName) != curMap.end()) {
                m_Data = curMap.find(dbName)->second;
            }
        }
    }
    
    if(entry->HasKey("animation")){
        BXON::Map * anim = entry->GetMap("animation");
        BXON::Array * tracks = anim->GetArray("tracks");
        for(uint32_t i = 0; i < tracks->GetSize(); i++){
            BXON::Map * track = tracks->GetMap(i);
            
            AnimationTrack * tr = new AnimationTrack();
            tr->Read(track);
            m_Tracks.insert(std::pair<std::string,AnimationTrack*>(tr->GetName(),tr));
        }
    }
    
    Update();
}

void Object::SetData(Datablock *data)
{
    m_Data = data;
}


Datablock * Object::GetData()
{
    return m_Data;
}

DatablockType Object::GetType()
{
    return DATABLOCK_OBJECT;
}

uint8_t Object::GetLayer() const{
    return m_Layer;
}

void Object::SetLayer(uint8_t layer) {
    m_Layer = layer;
}

void Object::Play(float time)
{
    MapFor(std::string, AnimationTrack*, m_Tracks, i){
        AnimationTrack * track = i->second;
        
        if(track->HasPosition(time))
            m_Local = track->GetPosition(time);
        
        if(track->HasScale(time))
            m_Scale= track->GetScale(time);
        
        if(track->HasEuler(time))
            m_Rotation = Math::EulerToQuat(track->GetEuler(time));
        
        Update();
        
        break;
    }
    
}
/// Return object visiblity flag.
bool Object::GetVisible() {
    return m_Visible;
}

/// Set object visiblity flag.
void Object::SetVisible(bool flag) {
    m_Visible = flag;
}

_SCENE_END
