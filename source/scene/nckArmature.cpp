
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckArmature.h"
#include "nckUtils.h"
#include "nckException.h"
#include "nckTransform.h"

_SCENE_BEGIN

Bone::Bone()
{
    m_Scale= Math::Vec3(1,1,1);
    m_Parent = NULL;
    m_InvBMatrix = m_BMatrix = m_Matrix = Math::Mat44();
}

Bone::~Bone()
{
    
}

void Bone::Read(Core::DataReader *f,Bone **b_array)
{
    f->Read(&m_Name);
    
    int bone_id = 0;
    
    f->Read(&bone_id,sizeof(int));
    
    if(bone_id!=-1)
        m_Parent = b_array[bone_id];
    else
        m_Parent = NULL;
    
    f->Read(&m_Head,3*sizeof(float));
    f->Read(&m_Tail,3*sizeof(float));
}

void Bone::Read(BXON::Map * entry, const std::vector<Bone *> & b_array){
    m_Name = entry->GetString("name");
    
    if(entry->HasKey("parent")){
        BXON::Map * parent = entry->GetMap("parent");
        if(parent->HasKey("id") && parent->GetInteger("id") < b_array.size()){
            m_Parent = b_array[parent->GetInteger("id")];
        }
    }
    
    if(entry->HasKey("bMatrix")){
        m_BMatrix= entry->GetMatrix("bMatrix");
        Math::Mat44 invbMatrix;
        Math::Invert(m_BMatrix, &invbMatrix);
        m_InvBMatrix = invbMatrix;
    }
    
    m_Head = entry->GetVec3("head");
    m_Tail = entry->GetVec3("tail");
}

Armature::Armature(Graph::Device * dev) : Datablock(dev)
{
    
}

Armature::~Armature()
{
    for(unsigned int i = 0;i<m_Bones.size();i++)
        delete m_Bones[i];
    
    MapFor(std::string,ArmatureTrack*,m_Tracks,i){
        delete i->second;
    }
    
    m_Tracks.clear();
}

DatablockType Armature::GetType()
{
    return DATABLOCK_ARMATURE;
}

void Armature::Read(Core::DataReader *f)
{
    f->Read(&m_Name);
    
    int bones_count = 0;
    f->Read(&bones_count,sizeof(int));
    
    m_Bones.reserve(bones_count);
    
    Bone ** b_a = new Bone*[bones_count];
    
    // Allocate all bone references
    for(int i = 0;i<bones_count;i++){
        b_a[i] = new Bone();
        m_Bones.push_back(b_a[i]);
    }
    
    // Load each bone data
    for(int i = 0;i<bones_count;i++){
        b_a[i]->Read(f,	b_a);
    }
    
    // Load Actions
    int actions = 0;
    f->Read(&actions,sizeof(int));
    
    /*m_Actions.reserve(actions);
    
    for(int i = 0;i<actions;i++)
    {
        ArmatureAction *act = new ArmatureAction();
        
        try{
            act->Read(f,b_a);
        }
        catch(Core::Exception & ex){
            THROW_EXCEPTION_STACK("Unable to read armature action",ex);
        }
        
        m_Actions.push_back(act);
    }
     */
    
    delete [] b_a;
    
    for(unsigned int i = 0;i<m_Bones.size();i++)
    {
        Bone *b = m_Bones[i];
        
        /// Process bone transformation matrix.
        b->m_Matrix = Math::Identity();
        
        if(b->m_Parent){
            b->m_Matrix = Math::Translate(b->m_Parent->m_Tail - b->m_Parent->m_Head) *  b->m_Parent->m_Matrix ;
        }
        
        b->m_Matrix = Math::Scale(b->m_Scale) * Math::Translate(b->m_Head + b->m_Location) * b->m_Matrix  ;
    }
}


bool Armature::Play(std::string name,float time,float mixture, bool clear)
{
    MapFor(std::string, ArmatureTrack*, m_Tracks, i){
        for(int j = 0; j < m_Bones.size(); j++)
            i->second->UpdateBone(m_Bones[j], time);
        break;
    }
    
    return false;
}

void Armature::Update()
{
    /// Update bones matrix
    for(unsigned int i = 0;i<m_Bones.size();i++)
    {
        Bone * b = m_Bones[i];
        
        /// Process bone transformation matrix.
        b->m_Matrix = Math::Identity();
        
        const Math::Quat q = Math::Normalize(b->m_Rotation);
 
        Math::Mat44 rot;
        
        if(b->m_Parent){
            b->m_Matrix = Math::Translate(b->m_Parent->m_Tail - b->m_Parent->m_Head) *  b->m_Parent->m_Matrix ;
        }
        
        rot = Math::Rotate(Math::Rotation(b->m_InvBMatrix * Math::Rotate(q) * b->m_BMatrix));

        b->m_Matrix = Math::Scale(b->m_Scale) * rot * Math::Translate((b->m_Head + b->m_InvBMatrix *b->m_Location)) * b->m_Matrix  ;
    }
}


Bone * Armature::GetBone(const std::string &name)
{
    for(unsigned int i = 0;i<m_Bones.size();i++)
    {
        if(m_Bones[i]->m_Name == name)
        {
            return m_Bones[i];
        }
    }
    return NULL;
}

void Armature::Render()
{
    Graph::Device * dev = m_Device;
    
    for(unsigned int i = 0; i < m_Bones.size(); i++)
    {
        Bone *b = m_Bones[i];
        
        // Begin bone rendering.
        dev->PushMatrix();
        dev->MultMatrix((float*)&b->m_Matrix);
        
        // Process bone orientation.
        Math::Vec3 vec = b->m_Tail-b->m_Head;
        
        float len = vec.Length();
        float pLen = len * 0.1;
        
        Math::Mat44 baseMatrix = Math::VecToMat(vec, Math::VectorAxis::AXIS_Z, Math::VectorAxis::AXIS_Y);
        
        dev->MultMatrix((float*)&baseMatrix);
        
        
        dev->Begin(Graph::PRIMITIVE_LINES);
        
        
        dev->Vertex(pLen,pLen,pLen);
        dev->Vertex(-pLen,pLen,pLen);
        
        dev->Vertex(-pLen,pLen,pLen);
        dev->Vertex(-pLen,-pLen,pLen);
        
        dev->Vertex(-pLen,-pLen,pLen);
        dev->Vertex(pLen,-pLen,pLen);
        
        dev->Vertex(pLen,-pLen,pLen);
        dev->Vertex(pLen,pLen,pLen);
        
        
        dev->Vertex(pLen,pLen,pLen);
        dev->Vertex(0,0,len);
        
        dev->Vertex(-pLen,pLen,pLen);
        dev->Vertex(0,0,len);
        
        dev->Vertex(pLen,-pLen,pLen);
        dev->Vertex(0,0,len);
        
        dev->Vertex(-pLen,-pLen,pLen);
        dev->Vertex(0,0,len);
        
        
        dev->Vertex(pLen,pLen,pLen);
        dev->Vertex(0,0,0);
        
        dev->Vertex(-pLen,pLen,pLen);
        dev->Vertex(0,0,0);
        
        dev->Vertex(pLen,-pLen,pLen);
        dev->Vertex(0,0,0);
       
        dev->Vertex(-pLen,-pLen,pLen);
        dev->Vertex(0,0,0);
     
        
        dev->End();
        
        dev->PopMatrix();
    }
}



#ifdef NCK_BXON

void Armature::Read(BXON::Map * entry){
    m_Name = entry->GetString("name");
    
    BXON::Array * bones = entry->GetArray("bones");
    
    for(uint32_t i = 0;i<bones->GetSize();i++){
        m_Bones.push_back(new Bone());
    }
    
    for(uint32_t i = 0;i<bones->GetSize();i++){
        m_Bones[i]->Read(bones->GetMap(i), m_Bones);
    }
    
    for (unsigned int i = 0; i<m_Bones.size(); i++)
    {
        Bone *b = m_Bones[i];

        /// Process bone transformation matrix.
        b->m_Matrix = Math::Identity();

        if (b->m_Parent) {
            b->m_Matrix = Math::Translate(b->m_Parent->m_Tail - b->m_Parent->m_Head) *  b->m_Parent->m_Matrix;
        }

        b->m_Matrix = Math::Scale(b->m_Scale) * Math::Translate(b->m_Head + b->m_Location) * b->m_Matrix;
    }

    for (unsigned int i = 0; i < m_Bones.size(); i++)
    {
        m_Bones[i]->m_RestPos = Math::TranslationPart(m_Bones[i]->m_Matrix);
    }

    if(entry->HasKey("animation")){
        BXON::Map * anim = entry->GetMap("animation");
        BXON::Array * tracks = anim->GetArray("tracks");
        for(uint32_t i = 0;i<tracks->GetSize();i++){
            BXON::Map * track = tracks->GetMap(i);
            ArmatureTrack * tr = new ArmatureTrack();
            tr->Read(track);
            m_Tracks.insert(std::pair<std::string,ArmatureTrack*>(tr->GetName(),tr));
        }
    }
}

#endif

std::vector<Bone*> Armature::GetBones(){
    return m_Bones;
}

ArmatureTrack::ArmatureTrack(){
    
}

ArmatureTrack::~ArmatureTrack(){
    for(uint32_t i = 0; i < m_Strips.size(); i++){
        delete m_Strips[i];
    }
    m_Strips.clear();
}

#ifdef NCK_BXON

void ArmatureTrack::Read(BXON::Map * entry){
    m_Name = entry->GetString("name");
    BXON::Array * strips = entry->GetArray("strips");
    for(uint32_t i = 0; i < strips->GetSize(); i++){
        ArmatureStrip * s = new ArmatureStrip();
        s->Read(strips->GetMap(i));
        m_Strips.push_back(s);
    }
}

#endif

void ArmatureTrack::UpdateBone(Bone * b, float time){
    for(uint32_t i = 0;i< m_Strips.size();i++){
        ArmatureStrip * s= m_Strips[i];
        if(time >= s->GetStart()  && time < s->GetEnd() ){
            if(s->HasQuaternion(b->m_Name))
                b->m_Rotation = s->GetQuaternion(b->m_Name, time);
            if(s->HasPosition(b->m_Name))
                b->m_Location = s->GetPosition(b->m_Name, time);
            
        }
    }
}

std::string ArmatureTrack::GetName(){
    return m_Name;
}

ArmatureStrip::ArmatureStrip(){
    
}

ArmatureStrip::~ArmatureStrip(){
    MapFor(std::string, AnimationStrip*, m_BoneStrips, i){
        delete i->second;
    }
    m_BoneStrips.clear();
}

float ArmatureStrip::GetStart(){
    return m_Start;
}

float ArmatureStrip::GetEnd(){
    return m_End;
}

Math::Vec3 ArmatureStrip::GetPosition(const std::string & name, float t){
    AnimationStrip * strip = GetStrip(name);
    if(strip!=NULL)
        return strip->GetPosition(t);
    return Math::Vec3();
}

Math::Vec3 ArmatureStrip::GetScale(const std::string & name, float t){
    AnimationStrip * strip = GetStrip(name);
    if(strip!=NULL)
        return strip->GetScale(t);
    return Math::Vec3();
}

Math::Vec3 ArmatureStrip::GetEuler(const std::string & name, float t){
    AnimationStrip * strip = GetStrip(name);
    if(strip!=NULL)
        return strip->GetEuler(t);
    return Math::Vec3();
}

Math::Quat ArmatureStrip::GetQuaternion(const std::string & name, float t){
    AnimationStrip * strip = GetStrip(name);
    if(strip!=NULL)
        return strip->GetQuaternion(t);
    return Math::Quat();
}

bool ArmatureStrip::HasPosition(const std::string & name){
    AnimationStrip * strip = GetStrip(name);
    return strip!= NULL && strip->HasPosition();
}

bool ArmatureStrip::HasScale(const std::string & name){
    AnimationStrip * strip = GetStrip(name);
    return strip!= NULL && strip->HasScale();
}

bool ArmatureStrip::HasEuler(const std::string & name){
    AnimationStrip * strip = GetStrip(name);
    return strip!= NULL && strip->HasEuler();
}

bool ArmatureStrip::HasQuaternion(const std::string & name){
    AnimationStrip * strip = GetStrip(name);
    return strip!= NULL && strip->HasQuaternion();
}

AnimationStrip * ArmatureStrip::GetStrip(const std::string & name){
    std::map<std::string,AnimationStrip*>::iterator i = m_BoneStrips.find(name);
    if(i!=m_BoneStrips.end())
        return i->second;
    return NULL;
}

#ifdef NCK_BXON
void ArmatureStrip::Read(BXON::Map * entry){
    std::vector<std::string> bones = entry->GetKeys();

    for(uint32_t i = 0;i<bones.size();i++){
        BXON::Map * boneStrip = entry->GetMap(bones[i]);
        AnimationStrip * as = new AnimationStrip();
        as->Read(boneStrip);
        
        if(i == 0){
            m_Start = as->GetStart();
            m_End = as->GetEnd();
        }else{
            if(as->GetStart() < m_Start)
                m_Start = as->GetStart();
            if(as->GetEnd() > m_End)
                m_End = as->GetEnd();
        }

        m_BoneStrips.insert(std::pair<std::string,AnimationStrip*>(bones[i],as));
    }
}
#endif

_SCENE_END
