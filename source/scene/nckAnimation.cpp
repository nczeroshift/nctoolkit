
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckAnimation.h"
#include "nckVec4.h"
#include "nckException.h"
#include "nckUtils.h"

_SCENE_BEGIN

AnimationNode::AnimationNode()
{
    m_P_Time = 0.0f;
    m_P_Value = 0.0f;
    m_C_Time = 0.0f;
    m_C_Value = 0.0f;
    m_N_Time = 0.0f;
    m_N_Value = 0.0f;
}

void AnimationNode::Read(Core::DataReader * f){
    f->Read(&m_P_Time,sizeof(float));
    f->Read(&m_P_Value,sizeof(float));
    f->Read(&m_C_Time,sizeof(float));
    f->Read(&m_C_Value,sizeof(float));
    f->Read(&m_N_Time,sizeof(float));
    f->Read(&m_N_Value,sizeof(float));
}

#ifdef NCK_BXON
void AnimationNode::Read(BXON::Array * array, int i){
    m_P_Time  = array->GetFloat(i);
    m_P_Value = array->GetFloat(i+1);
    m_C_Time  = array->GetFloat(i+2);
    m_C_Value = array->GetFloat(i+3);
    m_N_Time  = array->GetFloat(i+4);
    m_N_Value = array->GetFloat(i+5);
}
#endif

AnimationGraph::AnimationGraph()
{
    //	m_Type = ANIMATION_GRAPH_LOC_X;
}

AnimationGraph::~AnimationGraph()
{
    m_Nodes.clear();
    m_BlendMatrix.clear();
}

void AnimationGraph::Read(Core::DataReader *f)
{
    
    //	AnimationType l_type_table[16] =
    //	{
    //		ANIMATION_GRAPH_LOC_X,
    //		ANIMATION_GRAPH_LOC_Y,
    //		ANIMATION_GRAPH_LOC_Z,
    //
    //		ANIMATION_GRAPH_ROT_X,
    //		ANIMATION_GRAPH_ROT_Y,
    //		ANIMATION_GRAPH_ROT_Z,
    //		ANIMATION_GRAPH_ROT_W,
    //
    //		ANIMATION_GRAPH_SCALE_X,
    //		ANIMATION_GRAPH_SCALE_Y,
    //		ANIMATION_GRAPH_SCALE_Z,
    //
    //		ANIMATION_GRAPH_R,
    //		ANIMATION_GRAPH_G,
    //		ANIMATION_GRAPH_B,
    //		ANIMATION_GRAPH_A,
    //
    //		ANIMATION_GRAPH_LENS,
    //		ANIMATION_GRAPH_ENERGY,
    //	};
    
    /*
     unsigned int l_type_id;
     f->Read(&l_type_id,sizeof(int));
     
     if(l_type_id<16)
     m_Type = l_type_table[l_type_id-1];
     
     int number_of_points;
     f->Read(&number_of_points,sizeof(int));
     
     m_Nodes.reserve(number_of_points);
     
     if(number_of_points>1)
     m_BlendMatrix.reserve(number_of_points-1);
     
     for(int i = 0;i< number_of_points;i++)
     {
     AnimationNode n;
     n.Read(f);
     m_Nodes.push_back(n);
     
     if(i>0)
     {
     float start = m_Nodes[i-1].m_C_Time;
     float end = m_Nodes[i].m_C_Time;
     
     float t1 =  (m_Nodes[i-1].m_N_Time - start)/(end-start);
     float t2 =  (m_Nodes[i].m_P_Time - start)/(end-start);
     
     
     const float l_val[16] = {1,0,0,0, 1,t1,0,0, 1,t2,t1,0, 1,1,1,1};
     const float l_standart[16] = {1,0,0,0,-3,3,0,0, 0,-1.5,1.5,0, 2,-1.5,1.5,1};
     
     Math::Mat44 mat(l_val);
     Math::Mat44 res(l_standart);
     
     if(!Math::Invert(mat,&res))
     {
     // Unable to invert matrix, probably bad keyframe values, use the standart bezier coeffs (l_standart)
     //THROW_EXCEPTION("Bad matrix inversion");
     }
     
     m_BlendMatrix.push_back(res);
     }
     }
     */
}

void AnimationGraph::ComputeBlendMatrix(){
    if(m_Nodes.size()>1)
        m_BlendMatrix.reserve(m_Nodes.size()-1);
    
    for(int i = 0;i< m_Nodes.size();i++)
    {
        if(i>0)
        {
            float start = m_Nodes[i-1].m_C_Time;
            float end = m_Nodes[i].m_C_Time;
            
            float t1 =  (m_Nodes[i-1].m_N_Time - start)/(end-start);
            float t2 =  (m_Nodes[i].m_P_Time - start)/(end-start);
            
            
            const float l_val[16] = {1,0,0,0, 1,t1,0,0, 1,t2,t1,0, 1,1,1,1};
            const float l_standart[16] = {1,0,0,0,-3,3,0,0, 0,-1.5,1.5,0, 2,-1.5,1.5,1};
            
            Math::Mat44 mat(l_val);
            Math::Mat44 res(l_standart);
            
            if(!Math::Invert(mat,&res)){
                // Unable to invert matrix, probably bad keyframe values, use the standart bezier coeffs (l_standart)
                // THROW_EXCEPTION("Bad matrix inversion");
                Core::DebugLog("Bad matrix inversion");
            }
            
            m_BlendMatrix.push_back(res);
        }
    }
}
float AnimationGraph::GetValue(float time)
{
    if(m_Nodes.size() == 0)
        return 0;
    
    if(m_Nodes.size() == 1)
        return m_Nodes[0].m_C_Value;
    
    if(time == 0)
        return m_Nodes[0].m_C_Value;
    
    if(time < m_Nodes[0].m_C_Time)
        return m_Nodes[0].m_C_Value;
    
    if(time >= m_Nodes[m_Nodes.size()-1].m_C_Time)
        return m_Nodes[m_Nodes.size()-1].m_C_Value;
    
    size_t min = 0;
    size_t max = m_Nodes.size();
    size_t mid = 0;
    
    // binary search
    while(true){
        mid = (min+max)/2;
        
        if(time >= m_Nodes[min].m_C_Time && time < m_Nodes[mid].m_C_Time)
            max = mid;
        else
            min = mid;
        
        if(time >= m_Nodes[min].m_C_Time &&
           time < m_Nodes[min+1].m_C_Time)
            break;
    }
    
    float rt = (time-m_Nodes[min].m_C_Time)/(m_Nodes[min+1].m_C_Time-m_Nodes[min].m_C_Time);
    
    // U Vector.
    Math::Vec4 U = Math::Vec4(1.0,rt, rt*rt, rt*rt*rt);
    
    // A Matrix = UxB
    Math::Vec4 A = U * m_BlendMatrix[min];
    
    float val = A.GetX() * m_Nodes[min].m_C_Value +
				A.GetY() * m_Nodes[min].m_N_Value +
				A.GetZ() * m_Nodes[min+1].m_P_Value +
				A.GetW() * m_Nodes[min+1].m_C_Value;
    
    return val;
}

void AnimationGraph::SetChannel(AnimationChannel channel)
{
    m_Channel = channel;
}


AnimationChannel AnimationGraph::GetChannel() const
{
    return m_Channel;
}

void AnimationGraph::Read(BXON::Array * entry){
    uint32_t nodes = entry->GetSize() / 6;
    m_Nodes.reserve(nodes);
    for(uint32_t i = 0;i<entry->GetSize();i+=6){
        AnimationNode node;
        node.Read(entry, i);
        m_Nodes.push_back(node);
    }
    
    ComputeBlendMatrix();
}

AnimationStrip::AnimationStrip(){
    m_ChannelsMask = 0;
    m_Repeat = 0;
    m_Graphs.reserve(ANIMATION_GRAPH_RESERVED);
    for(uint32_t i = 0; i < ANIMATION_GRAPH_RESERVED; i++){
        m_Graphs.push_back(NULL);
    }
}

AnimationStrip::~AnimationStrip(){
    for(uint32_t i = 0; i < ANIMATION_GRAPH_RESERVED;i++){
        if(m_Graphs[i] != NULL)
            SafeDelete(m_Graphs[i]);
    }
}

float AnimationStrip::GetStart(){
    return m_Start;
}

float AnimationStrip::GetEnd(){
    return m_End;
}

int AnimationStrip::GetRepeat(){
    return m_Repeat;
}


Math::Vec3 AnimationStrip::GetPosition(float t){
    if(HasPosition()){
        float x = 0;
        float y = 0;
        float z = 0;
        if(m_Graphs[ANIMATION_CHANNEL_LOC_X]!=NULL)
            x = m_Graphs[ANIMATION_CHANNEL_LOC_X]->GetValue(t);
        if(m_Graphs[ANIMATION_CHANNEL_LOC_Y]!=NULL)
            y = m_Graphs[ANIMATION_CHANNEL_LOC_Y]->GetValue(t);
        if(m_Graphs[ANIMATION_CHANNEL_LOC_Z]!=NULL)
            z = m_Graphs[ANIMATION_CHANNEL_LOC_Z]->GetValue(t);
        return Math::Vec3(x,y,z);
    }
    return Math::Vec3();
}

Math::Vec3 AnimationStrip::GetScale(float t){
    if(HasScale()){
        float x = 0;
        float y = 0;
        float z = 0;
        if(m_Graphs[ANIMATION_CHANNEL_SCALE_X]!=NULL)
            x = m_Graphs[ANIMATION_CHANNEL_SCALE_X]->GetValue(t);
        if(m_Graphs[ANIMATION_CHANNEL_SCALE_Y]!=NULL)
            y = m_Graphs[ANIMATION_CHANNEL_SCALE_Y]->GetValue(t);
        if(m_Graphs[ANIMATION_CHANNEL_SCALE_Z]!=NULL)
            z = m_Graphs[ANIMATION_CHANNEL_SCALE_Z]->GetValue(t);
        return Math::Vec3(x,y,z);
    }
    return Math::Vec3(1,1,1);
}

Math::Vec3 AnimationStrip::GetEuler(float t){
    if(HasEuler()){
        float x = 0;
        float y = 0;
        float z = 0;
        if(m_Graphs[ANIMATION_CHANNEL_ROT_X]!=NULL)
            x = m_Graphs[ANIMATION_CHANNEL_ROT_X]->GetValue(t);
        if(m_Graphs[ANIMATION_CHANNEL_ROT_Y]!=NULL)
            y = m_Graphs[ANIMATION_CHANNEL_ROT_Y]->GetValue(t);
        if(m_Graphs[ANIMATION_CHANNEL_ROT_Z]!=NULL)
            z = m_Graphs[ANIMATION_CHANNEL_ROT_Z]->GetValue(t);
        return Math::Vec3(x,y,z);
    }
    return Math::Vec3();
}

Math::Quat AnimationStrip::GetQuaternion(float t){
    if(HasQuaternion()){
        float x = m_Graphs[ANIMATION_CHANNEL_ROT_X]->GetValue(t);
        float y = m_Graphs[ANIMATION_CHANNEL_ROT_Y]->GetValue(t);
        float z = m_Graphs[ANIMATION_CHANNEL_ROT_Z]->GetValue(t);
        float w = m_Graphs[ANIMATION_CHANNEL_ROT_W]->GetValue(t);
        return Math::Quat(x,y,z,w);
    }
    return Math::Quat();
}

bool AnimationStrip::HasPosition(){
    return (m_ChannelsMask & ANIMATION_GRAPH_LOC) != 0;
}

bool AnimationStrip::HasScale(){
    return (m_ChannelsMask & ANIMATION_GRAPH_SCALE) != 0;
}

bool AnimationStrip::HasEuler(){
    return (m_ChannelsMask & ANIMATION_GRAPH_ROT) != 0 && m_Graphs[ANIMATION_CHANNEL_ROT_W] == NULL;
}

bool AnimationStrip::HasQuaternion(){
    return m_Graphs[ANIMATION_CHANNEL_ROT_X] != NULL &&
    m_Graphs[ANIMATION_CHANNEL_ROT_Y] != NULL &&
    m_Graphs[ANIMATION_CHANNEL_ROT_Z] != NULL &&
    m_Graphs[ANIMATION_CHANNEL_ROT_W] != NULL;
}

#ifdef NCK_BXON

void AnimationStrip::Read(BXON::Map * entry){
    Math::Vec2 range = entry->GetVec2("range");
    m_Start = range.GetX();
    m_End = range.GetY();
    
    
    if(entry->HasKey("euler")){
        BXON::Array * array = entry->GetArray("euler");
        for(uint32_t i = 0; i < array->GetSize(); i++){
            AnimationGraph * g = new AnimationGraph();
            g->Read(array->GetArray(i));
            g->SetChannel((AnimationChannel)(ANIMATION_CHANNEL_ROT_X+i));
            m_Graphs[ANIMATION_GRAPH_ROT+i] = g;
        }
        m_ChannelsMask |= ANIMATION_GRAPH_ROT;
    }
    
    if(entry->HasKey("position")){
        BXON::Array * array = entry->GetArray("position");
        for(uint32_t i = 0; i < array->GetSize(); i++){
            AnimationGraph * g = new AnimationGraph();
            g->Read(array->GetArray(i));
            g->SetChannel((AnimationChannel)(ANIMATION_CHANNEL_LOC_X+i));
            m_Graphs[ANIMATION_GRAPH_LOC+i] = g;
        }
        m_ChannelsMask |= ANIMATION_GRAPH_LOC;
    }
    
    if(entry->HasKey("scale")){
        BXON::Array * array = entry->GetArray("scale");
        for(uint32_t i = 0; i < array->GetSize(); i++){
            AnimationGraph * g = new AnimationGraph();
            g->Read(array->GetArray(i));
            g->SetChannel((AnimationChannel)(ANIMATION_CHANNEL_SCALE_X+i));
            m_Graphs[ANIMATION_GRAPH_SCALE+i] = g;
        }
        m_ChannelsMask |= ANIMATION_GRAPH_SCALE;
    }
    
    if(entry->HasKey("quat")){
        BXON::Array * array = entry->GetArray("quat");
        for(uint32_t i = 0; i < array->GetSize(); i++){
            AnimationGraph * g = new AnimationGraph();
            g->Read(array->GetArray(i));
            g->SetChannel((AnimationChannel)(ANIMATION_CHANNEL_ROT_X+i));
            m_Graphs[ANIMATION_GRAPH_ROT+i] = g;
        }
        m_ChannelsMask |= ANIMATION_GRAPH_ROT;
    }
    
    if(entry->HasKey("color")){
        BXON::Array * array = entry->GetArray("color");
        for(uint32_t i = 0; i < array->GetSize(); i++){
            AnimationGraph * g = new AnimationGraph();
            g->Read(array->GetArray(i));
            g->SetChannel((AnimationChannel)(ANIMATION_CHANNEL_COLOR_R+i));
            m_Graphs[ANIMATION_GRAPH_COLOR+i] = g;
        }
        m_ChannelsMask |= ANIMATION_GRAPH_COLOR;
    }
}

#endif



AnimationTrack::AnimationTrack(){
    
}

AnimationTrack::~AnimationTrack(){
    for(uint32_t i = 0;i<m_Strips.size();i++)
        SafeDelete(m_Strips[i]);
}

void AnimationTrack::Read(BXON::Map * entry){
    BXON::Array * strips = entry->GetArray("strips");
    m_Name = entry->GetString("name");
    for(uint32_t i = 0; i < strips->GetSize(); i++){
        AnimationStrip * s = new AnimationStrip();
        s->Read(strips->GetMap(i));
        m_Strips.push_back(s);
    }
}

std::string AnimationTrack::GetName(){
    return m_Name;
}

Math::Vec3 AnimationTrack::GetPosition(float t){
    for(uint32_t i = 0; i < m_Strips.size(); i++){
        if(t >= m_Strips[i]->GetStart() && t < m_Strips[i]->GetEnd()){
            return m_Strips[i]->GetPosition(t);
        }
    }
    return Math::Vec3();
}

Math::Vec3 AnimationTrack::GetScale(float t){
    for(uint32_t i = 0; i < m_Strips.size(); i++){
        if(t >= m_Strips[i]->GetStart() && t < m_Strips[i]->GetEnd()){
            return m_Strips[i]->GetScale(t);
        }
    }
    return Math::Vec3(1,1,1);
}

Math::Vec3 AnimationTrack::GetEuler(float t){
    for(uint32_t i = 0; i < m_Strips.size(); i++){
        if(t >= m_Strips[i]->GetStart() && t < m_Strips[i]->GetEnd()){
            return m_Strips[i]->GetEuler(t);
        }
    }
    return Math::Vec3();
}

Math::Quat AnimationTrack::GetQuaternion(float t){
    for(uint32_t i = 0; i < m_Strips.size(); i++){
        if(t >= m_Strips[i]->GetStart() && t < m_Strips[i]->GetEnd()){
            return m_Strips[i]->GetQuaternion(t);
        }
    }
    return Math::Quat();
}

bool AnimationTrack::HasPosition(float t){
    for(uint32_t i = 0; i < m_Strips.size(); i++){
        if(t >= m_Strips[i]->GetStart() && t < m_Strips[i]->GetEnd()){
            return m_Strips[i]->HasPosition();
        }
    }
    return false;
}

bool AnimationTrack::HasScale(float t){
    for(uint32_t i = 0; i < m_Strips.size(); i++){
        if(t >= m_Strips[i]->GetStart() && t < m_Strips[i]->GetEnd()){
            return m_Strips[i]->HasScale();
        }
    }
    return false;
}

bool AnimationTrack::HasEuler(float t){
    for(uint32_t i = 0; i < m_Strips.size(); i++){
        if(t >= m_Strips[i]->GetStart() && t < m_Strips[i]->GetEnd()){
            return m_Strips[i]->HasEuler();
        }
    }
    return false;
}

bool AnimationTrack::HasQuaternion(float t){
    for(uint32_t i = 0; i < m_Strips.size(); i++){
        if(t >= m_Strips[i]->GetStart() && t < m_Strips[i]->GetEnd()){
            return m_Strips[i]->HasQuaternion();
        }
    }
    return false;
}


_SCENE_END
