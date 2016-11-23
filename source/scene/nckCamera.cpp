
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckCamera.h"
#include "nckUtils.h"

_SCENE_BEGIN

Camera::Camera(Graph::Device *dev) : Datablock(dev){
    m_Object = NULL;
    m_Aspect = 4.0f/3.0f;
    m_Start = 1.0f;
    m_End = 100.0f;
    m_Fov = 36.0f;
}

Camera::~Camera(){
    
}

float Camera::GetFov(){
    return m_Fov;
}

void Camera::SetFov(float fFov){
    m_Fov = fFov;
}

void Camera::SetStart(float value){
    m_Start = value;
}

float Camera::GetStart(){
    return m_Start;
}

void Camera::SetEnd(float value){
    m_End = value;
}

float Camera::GetEnd(){
    return m_End;
}

void Camera::SetAspect(float value){
    m_Aspect = value;
}

float Camera::GetAspect(){
    return m_Aspect;
}

void Camera::Enable(Graph::MatrixType mat)
{
    if(mat == Graph::MATRIX_PROJECTION)
    {
        m_Device->Perspective(m_Aspect,m_Fov,m_Start,m_End);
    }
    else if(mat == Graph::MATRIX_VIEW && m_Object)
    {
        Math::Quat q = -m_Object->GetRotation();
        m_Device->Quaternion(q.GetX(),q.GetY(),q.GetZ(),q.GetW());
        m_Device->Translate(-m_Object->GetPosition().GetX(),
                            -m_Object->GetPosition().GetY(),
                            -m_Object->GetPosition().GetZ());
    }
}

Math::Mat44 Camera::GetMatrix()
{
    Math::Mat44 mat = Math::Translate(-m_Object->GetPosition()) * Math::Rotate(-m_Object->GetRotation());
    return mat;
}

void Camera::SetObject(Object *obj)
{
    m_Object = obj;
}

Object *Camera::GetObject()
{
    return m_Object;
}

void Camera::Read(Core::DataReader *f)
{
    f->Read(&m_Name);
    
    f->Read(&m_Fov,sizeof(float));
    f->Read(&m_Start,sizeof(float));
    f->Read(&m_End,sizeof(float));
    
    char hasAction = 0x0;
    f->Read(&hasAction,sizeof(char));
}

#ifdef NCK_BXON

void Camera::Read(BXON::Map * entry){
    m_Name = entry->GetString("name");
    m_Fov = entry->GetFloat("fov");
    m_Start = entry->GetFloat("start");
    m_End = entry->GetFloat("end");
}

#endif

DatablockType Camera::GetType()
{
    return DATABLOCK_CAMERA;
}

void Camera::RenderViewportContour()
{
    int vp[4];
    m_Device->GetProperty(Graph::PROPERTY_VIEWPORT,vp);
    
    m_Device->Begin(Graph::PRIMITIVE_LINES);
    m_Device->Vertex(0.1f*vp[2],0.1f*vp[3]);
    m_Device->Vertex(0.9f*vp[2],0.1f*vp[3]);
    
    m_Device->Vertex(0.1f*vp[2],0.1f*vp[3]);
    m_Device->Vertex(0.1f*vp[2],0.9f*vp[3]);
    
    m_Device->Vertex(0.9f*vp[2],0.9f*vp[3]);
    m_Device->Vertex(0.1f*vp[2],0.9f*vp[3]);
    
    m_Device->Vertex(0.9f*vp[2],0.9f*vp[3]);
    m_Device->Vertex(0.9f*vp[2],0.1f*vp[3]);
    m_Device->End();
}

_SCENE_END
