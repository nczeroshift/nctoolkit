
#include <math.h>
#include "nckGeometry.h"
#include "nckException.h"
#include <string.h>

_GEOMETRY_BEGIN

VertexGroup::VertexGroup()
{
    auxiliar = NULL;
}

VertexGroup::~VertexGroup()
{
    
}

void VertexGroup::Parse(BXON::Map * mesh, std::vector<VertexGroup*> * v){
    if(mesh->HasKey("vertex_groups"))
    {
        BXON::Array * groups = mesh->GetArray("vertex_groups");
        v->reserve(groups->GetSize());
        
        for(uint32_t i = 0;i < groups->GetSize(); i++)
        {
            VertexGroup * vg = new VertexGroup();
            vg->m_Name = groups->GetString(i);
            v->push_back(vg);
        }
    }
}

VertexSkinning::VertexSkinning()
{
    
}

VertexSkinning::~VertexSkinning()
{
    
}

void VertexSkinning::operator = (const VertexSkinning &v){
    this->m_Weight = v.m_Weight;
    this->m_Group = v.m_Group;
}

#ifdef NCK_BXON
bool VertexSkinning::Parse(BXON::Map * mesh, std::list<Vertex*> * v, const std::vector<VertexGroup*> & groups){
    if(mesh->HasKey("vertex_weights"))
    {
        BXON::Array * vw = mesh->GetArray("vertex_weights");
        
        if(vw->GetSize() == v->size()){
            std::list<Vertex*>::iterator vi = v->begin();
            for(uint32_t i = 0;i < vw->GetSize(); i++)
            {
                BXON::Array * va = vw->GetArray(i);
                
                (*vi)->m_Skinning.reserve(va->GetSize()/2);
                
                for(int j = 0; j < va->GetSize(); j+=2){
                    int gId = va->GetInteger(j);
                    float weight = va->GetFloat(j+1);
                    
                    VertexSkinning * vs = new VertexSkinning();
                    vs->m_Group = groups[gId];
                    vs->m_Weight = weight;
                    
                    (*vi)->m_Skinning.push_back(vs);
                }
                vi++;
            }
            return true;
        }
    }
    return false;
}

#endif



Vertex::Vertex()
{
    m_Id = 0;
    m_Auxiliar = NULL;
}

Vertex::~Vertex()
{
    
}

void Vertex::operator = (const Vertex &v)
{
    this->m_Id = v.m_Id;
    this->m_Nor = v.m_Nor;
    this->m_Pos = v.m_Pos;
    
    this->m_Skinning.reserve(v.m_Skinning.size());
    for(unsigned int i = 0;i< v.m_Skinning.size();i++)
    {
        VertexSkinning *vs = new VertexSkinning();
        
        *vs = *v.m_Skinning[i];
        
        this->m_Skinning.push_back(vs);
    }
}

bool Vertex::Compare(const Math::BoundBox & bb)
{
    return Math::Inside(bb,m_Pos);
}

#ifdef NCK_BXON
void Vertex::Parse(BXON::Map * mesh, std::list<Vertex*> * vOut){
    BXON::Array * normals = mesh->GetArray("normals");
    BXON::Array * positions = mesh->GetArray("positions");
    
    if(normals->GetSize()!=positions->GetSize())
        THROW_EXCEPTION("Normals count != Vertices count");
    
    for(uint32_t i = 0, j = 0;i < positions->GetSize(); i += 3){
        Vertex * v = new Vertex();
        
        v->m_Pos.SetX(positions->GetFloat(i));
        v->m_Pos.SetY(positions->GetFloat(i+1));
        v->m_Pos.SetZ(positions->GetFloat(i+2));
        
        v->m_Nor.SetX(normals->GetFloat(i));
        v->m_Nor.SetY(normals->GetFloat(i+1));
        v->m_Nor.SetZ(normals->GetFloat(i+2));
        
        v->m_Id = j++;
        
        vOut->push_back(v);
    }
}
#endif

ShapeVertex::ShapeVertex(){
    
}

ShapeVertex::~ShapeVertex()
{
    
}

#ifdef NCK_BXON
void ShapeVertex::Parse(BXON::Map * mesh,std::list<ShapeVertex*> * vOut, const std::vector<VertexIterator*> & vIter){
    
}
#endif


ShapeKey::ShapeKey()
{
    
    
}

ShapeKey::~ShapeKey()
{
    ListFor(ShapeVertex*,m_Vertices,i)
    delete (*i);
    
    m_Vertices.clear();
}

#ifdef NCK_BXON
void ShapeKey::Parse(BXON::Map * mesh, const std::vector<VertexIterator*> & vIter){
    
}
#endif

FaceUV::FaceUV() {

}

FaceUV::~FaceUV() {
}

void FaceUV::Check(int layer, int vid) {
    if (layer >= m_Channels.size()) {
        for (size_t i = m_Channels.size(); i <= layer; i++) {
            m_Channels.push_back(0);
            m_Coordinates.push_back(std::vector<Math::Vec4>());
        }
    }

    if (vid >= m_Coordinates[layer].size()) {
        for (size_t i = m_Coordinates[layer].size(); i <= vid; i++) {
            m_Coordinates[layer].push_back(Math::Vec4());
        }
    }
}

void FaceUV::SetUV(int layer, int vId, float u, float v) {
    Check(layer, vId);
    m_Channels[layer] = 2;
    m_Coordinates[layer][vId] = Math::Vec4(u, v, 0, 0);
}

void FaceUV::SetUV(int layer, int vId, const Math::Vec2 & uv) {
    Check(layer, vId);
    m_Channels[layer] = 2;
    m_Coordinates[layer][vId] = Math::Vec4(uv.GetX(), uv.GetY(), 0, 0);
}

Math::Vec2 FaceUV::GetUV(int layer, int vId) const{
    if (layer < m_Coordinates.size())
        if (vId < m_Coordinates[layer].size())
            return Math::Vec2(Math::Vec3(m_Coordinates[layer][vId]));
    return Math::Vec2();
}

void FaceUV::SetUVZ(int layer, int vId, float u, float v, float z) {
    Check(layer, vId);
    m_Channels[layer] = 3;
    m_Coordinates[layer][vId] = Math::Vec4(u, v, z, 0);
}

void FaceUV::SetUVZ(int layer, int vId, const Math::Vec3 & uvz) {
    Check(layer, vId);
    m_Channels[layer] = 3;
    m_Coordinates[layer][vId] = Math::Vec4(uvz.GetX(), uvz.GetY(), uvz.GetZ(), 0);
}

Math::Vec3 FaceUV::GetUVZ(int layer, int vId) const{
    if (layer < m_Coordinates.size())
        if (vId < m_Coordinates[layer].size())
            return m_Coordinates[layer][vId];
    return Math::Vec3();
}

void FaceUV::SetUVZW(int layer, int vId, float u, float v, float z, float w) {
    Check(layer, vId);
    m_Channels[layer] = 3;
    m_Coordinates[layer][vId] = Math::Vec4(u, v, z, 0);
}

void FaceUV::SetUVZW(int layer, int vId, const Math::Vec4 & uvzw) {
    Check(layer, vId);
    m_Channels[layer] = 3;
    m_Coordinates[layer][vId] = Math::Vec4(uvzw.GetX(), uvzw.GetY(), uvzw.GetZ(), uvzw.GetW());
}

Math::Vec4 FaceUV::GetUVZW(int layer, int vId) const {
    if (layer < m_Coordinates.size())
        if (vId < m_Coordinates[layer].size())
            return m_Coordinates[layer][vId];
    return Math::Vec4();
}

int FaceUV::GetLayers() const{
    return m_Coordinates.size();
}

int FaceUV::GetChannels(int layer) const{
    return m_Channels[layer];
}

Face::Face()
{
    m_Id = 0;
    m_Auxiliar = NULL;
    m_Verts.reserve(4);
    m_MatIndex = 0;
}

Face::~Face()
{
    
}

bool Face::Compare(const Math::BoundBox & bb)
{
    for(unsigned int i = 0; i < m_Verts.size(); i++){
        if(Math::Inside(bb, (*m_Verts[i])->m_Pos))
            return true;
    }
    
    return false;
}

void Face::operator =(const Face &f)
{
    m_Verts.reserve(f.m_Verts.size());
    
    for(unsigned int i = 0; i < f.m_Verts.size(); i++){
        m_Verts.push_back(f.m_Verts[i]);
    }
    
    if (f.m_fUV.GetLayers() > 0) {
        for (unsigned int i = 0; i < f.m_fUV.GetLayers(); i++) {
            int channels = f.m_fUV.GetChannels(i);
            for (unsigned int j = 0; j < f.m_Verts.size(); j++) {
                if (channels == 2) {
                    m_fUV.SetUV(i,j,f.m_fUV.GetUV(i, j));
                }
                else if (channels == 3) {
                    m_fUV.SetUVZ(i, j, f.m_fUV.GetUVZ(i, j));
                }
                else if (channels == 4) {
                    m_fUV.SetUVZW(i, j, f.m_fUV.GetUVZW(i, j));
                }
            }
        }
    }
    
    if(f.m_Color.size() > 0){
        m_Color.reserve(f.m_Color.size());
        for(unsigned int i = 0; i < f.m_Color.size(); i++){
            m_Color[i].reserve(4);
            for(unsigned int j = 0; j < 4; j++)
                m_Color[i][j] = f.m_Color[i][j];
        }
    }
    
    m_MatIndex = f.m_MatIndex;
}

#ifdef NCK_BXON
void Face::Parse(BXON::Map * mesh, std::list<Face*> * fOut, const std::vector<VertexIterator> & vIter)
{
    BXON::Array * faces = mesh->GetArray("faces_vertices");
    BXON::Array * mats = NULL;

    if (mesh->HasKey("faces_materials"))
        mats = mesh->GetArray("faces_materials");

    int32_t vIterSize = (int32_t)vIter.size();
    
    int uvlCount = mesh->HasKey("uv_layers") ? mesh->GetArray("uv_layers")->GetSize() : 0;
    
  
       
    for(uint32_t i = 0, j = 0; i < faces->GetSize(); i += 4){
        int f1 = faces->GetInteger(i);
        int f2 = faces->GetInteger(i+1);
        int f3 = faces->GetInteger(i+2);
        int f4 = faces->GetInteger(i+3);
        
        if(f1 >= vIterSize || f2 >= vIterSize || f3 >= vIterSize || f4 >= vIterSize)
            THROW_EXCEPTION("Face vertex index out of range");
        
        Face * f = new Face();
        
        f->m_Verts.push_back(vIter[f1]);
        f->m_Verts.push_back(vIter[f2]);
        f->m_Verts.push_back(vIter[f3]);
        
        if(f4 >= 0){
            f->m_Verts.push_back(vIter[f4]);
        }
        
        if (mats != NULL)
            f->m_MatIndex = mats->GetInteger(j);

        f->m_Id = j++;
      
        fOut->push_back(f);
    }
    
    if(mesh->HasKey("faces_uv")){
        BXON::Array * uvs = mesh->GetArray("faces_uv");
        
        int uvid = 0;
        ListFor(Face*, (*fOut), i){
            
            Face * f = (*i);
            
            for(int j = 0; j < (*i)->m_Verts.size(); j++){
                float x = uvs->GetFloat(uvid++);
                float y = uvs->GetFloat(uvid++);
                
                for(int uvl = 0;uvl < uvlCount; uvl++){
                    f->m_fUV.SetUV(uvl,j, Math::Vec2(x, y));
                }
            }
        }
    }
    
}
#endif

Mesh::Mesh(){
    
}

Mesh::~Mesh()
{
    for(FaceIterator i = m_Faces.begin();i!=m_Faces.end();i++)
        delete (*i);
    m_Faces.clear();
    
    for(VertexIterator i = m_Vertices.begin();i!=m_Vertices.end();i++)
        delete (*i);
    m_Vertices.clear();
    
    for(unsigned int i = 0;i<m_ShapeKeys.size();i++)
        delete m_ShapeKeys[i];
    
    m_ShapeKeys.clear();
}

#ifdef NCK_BXON
Mesh * Mesh::Parse(BXON::Map * entry){
    Mesh * ret = new Mesh();
    std::vector<VertexIterator> vIterators;
    
    Vertex::Parse(entry, &ret->m_Vertices);
    
    vIterators.reserve(ret->m_Vertices.size());
    
    ListFor(Vertex*, ret->m_Vertices, i){
        vIterators.push_back(i);
    }
    
    Face::Parse(entry, &ret->m_Faces, vIterators);
    
    BXON::Array * uv_layers = NULL;
    if(entry->HasKey("uv_layers")){
        uv_layers = entry->GetArray("uv_layers");
        for(uint32_t i = 0;i<uv_layers->GetSize();i++){
            ret->m_UVLayers.push_back(uv_layers->GetString(i));
            ret->m_UVOptimization.push_back(true);
        }
    }
    
    VertexGroup::Parse(entry, &ret->m_Groups);
    
    ret->m_HasVertexSkinning = VertexSkinning::Parse(entry, &ret->m_Vertices, ret->m_Groups);
    
    return ret;
}
#endif

void GetVertexBuffer(Mesh * mesh, Math::Vec3 ** pos, Math::Vec3 ** nor)
{
    Math::Vec3 *p = new Math::Vec3[mesh->m_Vertices.size()];
    Math::Vec3 *n = new Math::Vec3[mesh->m_Vertices.size()];
    
    int count = 0;
    
    ListFor(Vertex*,mesh->m_Vertices,i)
    {
        p[count] = (*i)->m_Pos;
        n[count++] = (*i)->m_Nor;
    }
    
    *pos = p;
    *nor = n;
}

void GetVertexSkinning(Mesh * mesh, const std::map<std::string, int> & boneIds, XVertexSkinning ** vsk)
{
    *vsk = new XVertexSkinning[mesh->m_Vertices.size()];
    
    int count = 0;
    
    bool ret = true;
    ListFor(Vertex*,mesh->m_Vertices,i)
    {
        (*vsk)[count].m_Bone_Count = (int)(*i)->m_Skinning.size();
        for(int j = 0;j<(int)(*i)->m_Skinning.size() && j <16 ;j++)
        {
            
            if(boneIds.find((*i)->m_Skinning[j]->m_Group->m_Name) != boneIds.end())
            {
                int id = boneIds.find((*i)->m_Skinning[j]->m_Group->m_Name)->second;
                
                (*vsk)[count].m_Bone_Id[j] = id;
                (*vsk)[count].m_Bone_Weight[j] = (*i)->m_Skinning[j]->m_Weight;
            }
            else
            {
                ret = false;
            }
        }
        count++;
    }
    
    if(!ret){
        Core::DebugLog("Critical error, some groups may not be assigned to a bone\n");
    }
}


void GetVertexColor(Mesh * mesh, Math::Color4ub ** col, unsigned int color_id)
{
    *col = new Math::Color4ub[mesh->m_Vertices.size()];
    
    ListFor(Face*,mesh->m_Faces,f)
    {
        for(int fv_id = 0;fv_id <= (int)(*f)->m_Verts.size();fv_id++)
        {
            int v_id = (*(*f)->m_Verts[fv_id])->m_Id;
            (*col)[v_id] = (*f)->m_Color[color_id][fv_id];
        }
    }
}



void GetTextureCoords(Mesh * mesh, Math::Vec2 ** uv, unsigned int layer_id)
{
    Math::Vec2 * uvt = new Math::Vec2[mesh->m_Vertices.size()];

    ListFor(Face*,mesh->m_Faces,f)
    {
        for(int fv_id = 0;fv_id < (int)(*f)->m_Verts.size();fv_id++)
        {
            int v_id = (*(*f)->m_Verts[fv_id])->m_Id;
            uvt[v_id] = (*f)->m_fUV.GetUV(layer_id, fv_id);
        }
    }

    *uv = uvt;
}

void GetTextureCoordsUV(Mesh * mesh, Math::Vec2 ** uv, unsigned int layer_id){
    Math::Vec2 * uvt = new Math::Vec2[mesh->m_Vertices.size()];

    ListFor(Face*,mesh->m_Faces,f)
    {
        for(int fv_id = 0;fv_id < (int)(*f)->m_Verts.size();fv_id++)
        {
            int v_id = (*(*f)->m_Verts[fv_id])->m_Id;
            uvt[v_id] = (*f)->m_fUV.GetUV(layer_id, fv_id);
        }
    }

    *uv = uvt;
}

void GetTextureCoordsUVZ(Mesh * mesh, Math::Vec3 ** uv, unsigned int layer_id){
    Math::Vec3 * uvt = new Math::Vec3[mesh->m_Vertices.size()];

    ListFor(Face*,mesh->m_Faces,f)
    {
        for(int fv_id = 0;fv_id < (int)(*f)->m_Verts.size();fv_id++)
        {
            int v_id = (*(*f)->m_Verts[fv_id])->m_Id;
            uvt[v_id] = (*f)->m_fUV.GetUVZ(layer_id, fv_id);
        }
    }

    *uv = uvt;
}

void GetTextureCoordsUVZW(Mesh * mesh, Math::Vec4 ** uv, unsigned int layer_id){
    Math::Vec4 * uvt = new Math::Vec4[mesh->m_Vertices.size()];

    ListFor(Face*,mesh->m_Faces,f)
    {
        for(int fv_id = 0;fv_id < (int)(*f)->m_Verts.size();fv_id++)
        {
            int v_id = (*(*f)->m_Verts[fv_id])->m_Id;
            uvt[v_id] = (*f)->m_fUV.GetUVZW(layer_id, fv_id);
        }
    }

    *uv = uvt;
}
/**
 * Helper class used create a new vertice when the uv coordinates
 * in the faces which share the same vertice are diferent.
 */
class Vertex_Optimizer_Seams: public Auxiliar
{
public:
    /// Constructor
    /// @param uv First face uv coordinate
    /// @param v Reference to vertice which will be assigned
    /// with the first uv coordinate.
    Vertex_Optimizer_Seams(const Math::Vec4 & uv, Vertex *v)
    {
        m_Coords.push_front(uv);
        m_Original=v;
    }
    
    /// Destructor
    ~Vertex_Optimizer_Seams()
    {
        m_Coords.clear();
        m_Vertices.clear();
    }
    
    /// Check if the uv mapping, is coerent with other vertices.
    /// @param *fc Reference to face.
    /// @param *vert_list Reference to mesh vertices list.
    /// @param uv The uv coordinate to test.
    /// @return True if a new vertex was added to the mesh to hold
    /// the new uv coordinate.
    bool Check(Face * fc,unsigned int id, std::list<Vertex*> &vert_list, const Math::Vec4 & uv)
    {
        // Search if the uv already exists.
        bool found = false;
        int v_uv_id = 0;
        
        ListFor(Math::Vec4,m_Coords,co)
        {
            if((*co).GetX() == uv.GetX() &&
                (*co).GetY() == uv.GetY() &&
                (*co).GetZ() == uv.GetZ() &&
                (*co).GetW() == uv.GetW())
            {
                found = true;
                break;
            }
            v_uv_id++;
        }
        
        // There is no valid vertice for the uvcoordinate, add a new.
        if(!found)
        {
            Vertex *vn = new Vertex();
            
            //Copy vertex state
            *vn = *m_Original;
            vn->m_Id = (int)vert_list.size();
            
            vert_list.push_back(vn);
            VertexIterator it = vert_list.end();
            it--;
            
            //
            m_Vertices.push_back(it);
            m_Coords.push_back(uv);
            fc->m_Verts[id] = it;
            return true;
        }
        // Seek for vertice which as the correct uv coordinate.
        else if(found && v_uv_id)
        {
            int count = 0;
            ListFor(VertexIterator,m_Vertices,vi)
            if(v_uv_id-1 == count++){
                fc->m_Verts[id] = (*vi);
                break;
            }
            
            return false;
        }
        
        return false;
    }
    
    /// Reference to the first vertex.
    Vertex *m_Original;
    /// List of uv coordinates.
    std::list<Math::Vec4> m_Coords;
    /// List of new vertices added.
    std::list<VertexIterator> m_Vertices;
};


unsigned int OptimizeSeams(Mesh * mesh, unsigned int layer_id)
{
    int new_verts = 0;

    ListFor(Face*,mesh->m_Faces,f_i)
    {
        Face * f = (*f_i);

        for(unsigned int i = 0;i < f->m_Verts.size() ; i++)
        {
            Vertex *v = (*f->m_Verts[i]);
            
            Math::Vec4 uvzw = f->m_fUV.GetUVZW(layer_id, i);

            if(!v->m_Auxiliar)
            {
                v->m_Auxiliar = new Vertex_Optimizer_Seams(uvzw,v);
            }
            else
                if(((Vertex_Optimizer_Seams*)v->m_Auxiliar)->Check(f,i,mesh->m_Vertices, uvzw))
                    new_verts++;
        }
    }
    
    // Cleanup
    ListFor(Vertex *, mesh->m_Vertices,v)
    {
        if((*v)->m_Auxiliar)
        {
            delete (*v)->m_Auxiliar;
            (*v)->m_Auxiliar = NULL;
        }
    }
    
    return new_verts;
}


void GetFaceBuffer(Mesh * mesh, XTriangleFace ** tf, unsigned int * face_count, bool sort)
{
    int total_tri_faces = 0;
    
    ListFor(Face *,mesh->m_Faces,i)
    {
        int size = (int)(*i)->m_Verts.size();
        
        if(size == 3)
            total_tri_faces++;
        else if(size == 4)
            total_tri_faces+=2;
    }
    
    *face_count = total_tri_faces;
    
    *tf = new XTriangleFace[total_tri_faces];
    
    int j = 0;
    
    ListFor(Face*,mesh->m_Faces,i)
    {
        int size = (int)(*i)->m_Verts.size();
        
        if(size == 3)
        {
            (*tf)[j].m_MaterialIndex = (*i)->m_MatIndex;
            (*tf)[j].m_Vertices[0] = (*(*i)->m_Verts[0])->m_Id;
            (*tf)[j].m_Vertices[1] = (*(*i)->m_Verts[1])->m_Id;
            (*tf)[j].m_Vertices[2] = (*(*i)->m_Verts[2])->m_Id;
            j++;
        }
        else if(size == 4)
        {
            (*tf)[j].m_MaterialIndex = (*i)->m_MatIndex;
            (*tf)[j].m_Vertices[0] = (*(*i)->m_Verts[0])->m_Id;
            (*tf)[j].m_Vertices[1] = (*(*i)->m_Verts[1])->m_Id;
            (*tf)[j].m_Vertices[2] = (*(*i)->m_Verts[3])->m_Id;
            
            (*tf)[j+1].m_MaterialIndex = (*i)->m_MatIndex;
            (*tf)[j+1].m_Vertices[0] = (*(*i)->m_Verts[1])->m_Id;
            (*tf)[j+1].m_Vertices[1] = (*(*i)->m_Verts[2])->m_Id;
            (*tf)[j+1].m_Vertices[2] = (*(*i)->m_Verts[3])->m_Id;
            
            j+=2;
        }
    }
    
    // Apply selection sort, to sort faces by material id.
    if(sort)
    {
        int i,j,min;
        for(i = 0;i< total_tri_faces-1;i++)
        {
            min = i;
            
            for(j = i+1;j< total_tri_faces;j++)
            {
                if( (*tf)[min].m_MaterialIndex > (*tf)[j].m_MaterialIndex)
                {
                    min = j;
                }
            }
            
            if(min!=i)
            {
                XTriangleFace temp = (*tf)[min];
                
                (*tf)[min] = (*tf)[j];
                (*tf)[j] = temp;
            }
        }
    }
}

void GetTangentVectorBuffer(int vertices,int faces, Math::Vec3 * vb,
                            Math::Vec3 * nb, Math::Vec2 * uv, XTriangleFace * fcb,
                            Math::Vec4 ** t_coords)
{
    Math::Vec3 * tan1 = new Math::Vec3[vertices * 2];
    Math::Vec3 * tan2 = tan1 + vertices;
    
    Math::Vec4 * tangent = new Math::Vec4[vertices];
    
    memset(tan1,0, vertices * sizeof(Math::Vec3) * 2);
    
    for (int a = 0; a < faces; a++)
    {
        int i1 = fcb[a].m_Vertices[0];
        int i2 = fcb[a].m_Vertices[1];
        int i3 = fcb[a].m_Vertices[2];
        
        Math::Vec3 v1 = vb[i1];
        Math::Vec3 v2 = vb[i2];
        Math::Vec3 v3 = vb[i3];
        
        Math::Vec2 w1 = uv[i1];
        Math::Vec2 w2 = uv[i2];
        Math::Vec2 w3 = uv[i3];
        
        float x1 = v2.GetX() - v1.GetX();
        float x2 = v3.GetX() - v1.GetX();
        float y1 = v2.GetY() - v1.GetY();
        float y2 = v3.GetY() - v1.GetY();
        float z1 = v2.GetZ() - v1.GetZ();
        float z2 = v3.GetZ() - v1.GetZ();
        
        
        float s1 = w2.GetX() - w1.GetX();
        
        float s2 = w3.GetX() - w1.GetX();
        float t1 = w2.GetY() - w1.GetY();
        float t2 = w3.GetY() - w1.GetY();
        
        float r = 1.0F / (s1 * t2 - s2 * t1);
        Math::Vec3 sdir	(	(t2 * x1 - t1 * x2) * r,
                         (t2 * y1 - t1 * y2) * r,
                         (t2 * z1 - t1 * z2) * r	);
        
        Math::Vec3 tdir	(	(s1 * x2 - s2 * x1) * r,
                         (s1 * y2 - s2 * y1) * r,
                         (s1 * z2 - s2 * z1) * r	);
        
        tan1[i1] += sdir;
        tan1[i2] += sdir;
        tan1[i3] += sdir;
        
        tan2[i1] += tdir;
        tan2[i2] += tdir;
        tan2[i3] += tdir;
        
    }
    
    for (int a = 0; a < vertices; a++)
    {
        Math::Vec3 n = nb[a];
        Math::Vec3 t = tan1[a];
        
        // Gram-Schmidt orthogonalize
        tangent[a] = Math::Vec4(	Math::Normalize(t - n * (Math::Dot(n,t))) ,
                                (( Math::Dot(Cross(n,t),tan2[a]) )  < 0.0F) ? -1.0F : 1.0F );
    }
    
    delete[] tan1;
    
    *t_coords = tangent;
}


XVertexSkinning::XVertexSkinning()
{
    memset(m_Bone_Id,0,sizeof(float)*16);
    memset(m_Bone_Weight,0,sizeof(float)*16);
    m_Bone_Count = 0;
}

XTriangleFace::XTriangleFace()
{
    m_Vertices[0] = 0;
    m_Vertices[1] = 0;
    m_Vertices[2] = 0;
    m_MaterialIndex= 0;
}

void XTriangleFace::operator=(const XTriangleFace &tf)
{
    m_Vertices[0] = tf.m_Vertices[0];
    m_Vertices[1] = tf.m_Vertices[1];
    m_Vertices[2] = tf.m_Vertices[2];
    m_MaterialIndex= tf.m_MaterialIndex;
}

_GEOMETRY_END

