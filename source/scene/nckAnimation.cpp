
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckAnimation.h"
#include "nckVec4.h"
#include "nckException.h"
#include "nckUtils.h"
#include "nckMathUtils.h"

_SCENE_BEGIN

// https://stackoverflow.com/questions/5883264/interpolating-values-between-interval-interpolation-as-per-bezier-curve
class BPoint {
public:
    BPoint() {
        X = 0;
        Y = 0;
    }

    BPoint(double x, double y) {
        X = x;
        Y = y;
    }
    double X, Y;
};

class BCurve {
public:
    BCurve(BPoint p0, BPoint p1, BPoint p2, BPoint p3){
        P0 = p0;
        P1 = p1;
        P2 = p2;
        P3 = p3;
    }

    BPoint P0, P1, P2, P3;

    double GetY(double x) {
        // Determine t
        double t;
        if (x == P0.X) {
            // Handle corner cases explicitly to prevent rounding errors
            t = 0;
        }
        else if (x == P3.X) {
            t = 1;
        }
        else {
            // Calculate t
            double a = -P0.X + 3 * P1.X - 3 * P2.X + P3.X;
            double b = 3 * P0.X - 6 * P1.X + 3 * P2.X;
            double c = -3 * P0.X + 3 * P1.X;
            double d = P0.X - x;
            double  tTemp = SolveCubic(a, b, c, d);
            if (tTemp == NAN) return NAN;
            t = tTemp;
        }

        // Calculate y from t
        return Cubed(1 - t) * P0.Y
            + 3 * t * Squared(1 - t) * P1.Y
            + 3 * Squared(t) * (1 - t) * P2.Y
            + Cubed(t) * P3.Y;
    }

    // Solves the equation ax³+bx²+cx+d = 0 for x ϵ ℝ
    // and returns the first result in [0, 1] or null.
    static double SolveCubic(double a, double b, double c, double d) {
        if (a == 0) return SolveQuadratic(b, c, d);
        if (d == 0) return 0;

        b /= a;
        c /= a;
        d /= a;
        double q = (3.0 * c - Squared(b)) / 9.0;
        double r = (-27.0 * d + b * (9.0 * c - 2.0 * Squared(b))) / 54.0;
        double disc = Cubed(q) + Squared(r);
        double term1 = b / 3.0;

        if (disc > 0) {
            double s = r + sqrt(disc);
            s = (s < 0) ? -CubicRoot(-s) : CubicRoot(s);
            double t = r - sqrt(disc);
            t = (t < 0) ? -CubicRoot(-t) : CubicRoot(t);

            double result = -term1 + s + t;
            if (result >= 0 && result <= 1) return result;
        }
        else if (disc == 0) {
            double r13 = (r < 0) ? -CubicRoot(-r) : CubicRoot(r);

            double result = -term1 + 2.0 * r13;
            if (result >= 0 && result <= 1) return result;

            result = -(r13 + term1);
            if (result >= 0 && result <= 1) return result;
        }
        else {
            q = -q;
            double dum1 = q * q * q;
            dum1 = acos(r / sqrt(dum1));
            double r13 = 2.0 * sqrt(q);

            double result = -term1 + r13 * cos(dum1 / 3.0);
            if (result >= 0 && result <= 1) return result;

            result = -term1 + r13 * cos((dum1 + 2.0 * M_PI) / 3.0);
            if (result >= 0 && result <= 1) return result;

            result = -term1 + r13 * cos((dum1 + 4.0 * M_PI) / 3.0);
            if (result >= 0 && result <= 1) return result;
        }

        return NAN;
    }

    // Solves the equation ax² + bx + c = 0 for x ϵ ℝ
    // and returns the first result in [0, 1] or null.
    static double SolveQuadratic(double a, double b, double c) {
        double result = (-b + sqrt(Squared(b) - 4 * a * c)) / (2 * a);
        if (result >= 0 && result <= 1) return result;

        result = (-b - sqrt(Squared(b) - 4 * a * c)) / (2 * a);
        if (result >= 0 && result <= 1) return result;

        return NAN;
    }

    static double Squared(double f) { return f * f; }

    static double Cubed(double f) { return f * f * f; }

    static double CubicRoot(double f) { return pow(f, 1.0 / 3.0); }
};

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
    /*if(m_Nodes.size()>1)
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
    }*/
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
    
    /*
    float rt = (time-m_Nodes[min].m_C_Time)/(m_Nodes[min+1].m_C_Time-m_Nodes[min].m_C_Time);
    
    // U Vector.
    Math::Vec4 U = Math::Vec4(1.0,rt, rt*rt, rt*rt*rt);
    
    // A Matrix = UxB
    Math::Vec4 A = U * m_BlendMatrix[min];
    */
    
    /*float val = A.GetX() * m_Nodes[min].m_C_Value +
				A.GetY() * m_Nodes[min].m_N_Value +
				A.GetZ() * m_Nodes[min+1].m_P_Value +
				A.GetW() * m_Nodes[min+1].m_C_Value;
    
    return val;*/

    //return m_Nodes[min].m_C_Value * (1 - rt) + m_Nodes[min + 1].m_C_Value * rt;
    BCurve c(BPoint(m_Nodes[min].m_C_Time, m_Nodes[min].m_C_Value),
        BPoint(m_Nodes[min].m_N_Time, m_Nodes[min].m_N_Value),
        BPoint(m_Nodes[min+1].m_P_Time, m_Nodes[min+1].m_P_Value),
        BPoint(m_Nodes[min+1].m_C_Time, m_Nodes[min+1].m_C_Value)
        );
    return c.GetY(time);
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
        if(t >= m_Strips[i]->GetStart() && t < m_Strips[i]->GetEnd())
            return m_Strips[i]->GetPosition(t);
        else if(t < m_Strips[i]->GetStart())
            return m_Strips[i]->GetPosition(m_Strips[i]->GetStart());
        else if (t > m_Strips[i]->GetEnd())
            return m_Strips[i]->GetPosition(m_Strips[i]->GetEnd());
    }
    return Math::Vec3();
}

Math::Vec3 AnimationTrack::GetScale(float t){
    for(uint32_t i = 0; i < m_Strips.size(); i++){
        if(t >= m_Strips[i]->GetStart() && t < m_Strips[i]->GetEnd())
            return m_Strips[i]->GetScale(t);
        else if (t < m_Strips[i]->GetStart())
            return m_Strips[i]->GetScale(m_Strips[i]->GetStart());
        else if (t > m_Strips[i]->GetEnd())
            return m_Strips[i]->GetScale(m_Strips[i]->GetEnd());
    }
    return Math::Vec3(1,1,1);
}

Math::Vec3 AnimationTrack::GetEuler(float t){
    for(uint32_t i = 0; i < m_Strips.size(); i++){
        if(t >= m_Strips[i]->GetStart() && t < m_Strips[i]->GetEnd())
            return m_Strips[i]->GetEuler(t);
        else if (t < m_Strips[i]->GetStart())
            return m_Strips[i]->GetEuler(m_Strips[i]->GetStart());
        else if (t > m_Strips[i]->GetEnd())
            return m_Strips[i]->GetEuler(m_Strips[i]->GetEnd());
    }
    return Math::Vec3();
}

Math::Quat AnimationTrack::GetQuaternion(float t){
    for(uint32_t i = 0; i < m_Strips.size(); i++){
        if(t >= m_Strips[i]->GetStart() && t < m_Strips[i]->GetEnd())
            return m_Strips[i]->GetQuaternion(t);
        else if (t < m_Strips[i]->GetStart())
            return m_Strips[i]->GetQuaternion(m_Strips[i]->GetStart());
        else if (t > m_Strips[i]->GetEnd())
            return m_Strips[i]->GetQuaternion(m_Strips[i]->GetEnd());
    }
    return Math::Quat();
}

bool AnimationTrack::HasPosition(float t){
    for(uint32_t i = 0; i < m_Strips.size(); i++){
        return m_Strips[i]->HasPosition();
    }
    return false;
}

bool AnimationTrack::HasScale(float t){
    for(uint32_t i = 0; i < m_Strips.size(); i++){
        return m_Strips[i]->HasScale();
    }
    return false;
}

bool AnimationTrack::HasEuler(float t){
    for(uint32_t i = 0; i < m_Strips.size(); i++){
        return m_Strips[i]->HasEuler();
    }
    return false;
}

bool AnimationTrack::HasQuaternion(float t){
    for(uint32_t i = 0; i < m_Strips.size(); i++){
            return m_Strips[i]->HasQuaternion();
    }
    return false;
}

float GetKeyFrameForTime(float fps, float time) {
    return time * fps;
}

_SCENE_END
