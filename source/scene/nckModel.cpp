
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckModel.h"
#include "nckException.h"
#include "nckUtils.h"

#include "nckArmature.h"

#ifdef NCK_BXON
#include "nckGeometry.h"
#endif

#include "nckQueueBuffer.h"
#include <stdio.h>

_SCENE_BEGIN

Model::Model(Graph::Device *dev) : Datablock(dev){
    m_Mesh = NULL;
    m_Vertices = 0;
    m_Faces = 0;
}

Model::~Model(){
    m_Materials.clear();
    SafeDelete(m_Mesh);
}

//
//void Model::RenderBoundBox()
//{
//	m_Device->Color(255,255,0,255);
//	Math::Vec3 b = m_BoundBox.GetMin();
//	Math::Vec3 a = m_BoundBox.GetMax();
//	m_Device->Begin(Graph::PRIMITIVE_LINES);
//		// bottom
//		m_Device->Vertex(b.GetX(),a.GetY(),b.GetZ());
//		m_Device->Vertex(b.GetX(),b.GetY(),b.GetZ());
//
//		m_Device->Vertex(b.GetX(),b.GetY(),b.GetZ());
//		m_Device->Vertex(a.GetX(),b.GetY(),b.GetZ());
//
//		m_Device->Vertex(a.GetX(),b.GetY(),b.GetZ());
//		m_Device->Vertex(a.GetX(),a.GetY(),b.GetZ());
//
//		m_Device->Vertex(a.GetX(),a.GetY(),b.GetZ());
//		m_Device->Vertex(b.GetX(),a.GetY(),b.GetZ());
//
//		// top
//		m_Device->Vertex(b.GetX(),a.GetY(),a.GetZ());
//		m_Device->Vertex(b.GetX(),b.GetY(),a.GetZ());
//
//		m_Device->Vertex(b.GetX(),b.GetY(),a.GetZ());
//		m_Device->Vertex(a.GetX(),b.GetY(),a.GetZ());
//
//		m_Device->Vertex(a.GetX(),b.GetY(),a.GetZ());
//		m_Device->Vertex(a.GetX(),a.GetY(),a.GetZ());
//
//		m_Device->Vertex(a.GetX(),a.GetY(),a.GetZ());
//		m_Device->Vertex(b.GetX(),a.GetY(),a.GetZ());
//
//		// sides
//		m_Device->Vertex(b.GetX(),a.GetY(),a.GetZ());
//		m_Device->Vertex(b.GetX(),a.GetY(),b.GetZ());
//
//		m_Device->Vertex(a.GetX(),a.GetY(),a.GetZ());
//		m_Device->Vertex(a.GetX(),a.GetY(),b.GetZ());
//
//		m_Device->Vertex(a.GetX(),b.GetY(),a.GetZ());
//		m_Device->Vertex(a.GetX(),b.GetY(),b.GetZ());
//
//		m_Device->Vertex(b.GetX(),b.GetY(),a.GetZ());
//		m_Device->Vertex(b.GetX(),b.GetY(),b.GetZ());
//	m_Device->End();
//}

void Model::Render(Material *overlap)
{
    if(overlap) overlap->Enable();
    
    m_Mesh->Enable();
    for(unsigned int i = 0;i<m_Materials.size();i++)
    {
        if(!overlap)m_Materials[i]->Enable();
        m_Mesh->Render(i);
        if(!overlap)m_Materials[i]->Disable();
    }
    m_Mesh->Disable();
    if(overlap) overlap->Disable();
    
}

Math::BoundBox Model::GetBoundBox()
{
    return m_BoundBox;
}


void Model::Read(Core::DataReader *f,std::vector<Material *> *mat_vec){
    unsigned int groups_count = 0;
    
    f->Read(&m_Name);
    
    f->Read(&m_Vertices,sizeof(int));
    f->Read(&groups_count,sizeof(int));
    
    m_Faces = 0;
    
    Math::Vec3 bbMax,bbMin;
    f->Read(&bbMin,sizeof(float)*3);
    f->Read(&bbMax,sizeof(float)*3);
    
    m_BoundBox.SetMax(bbMax);
    m_BoundBox.SetMin(bbMin);
    
    Graph::VertexProfile vp = ReadVProfile(f);
    
    unsigned char *vbb = new unsigned char[vp.GetVertexSize()*m_Vertices];
    
    //int dataread =
    f->Read(vbb,vp.GetVertexSize()*m_Vertices);
    
    unsigned int *faces_count = new unsigned int[groups_count];
    unsigned int **fcb = new unsigned int *[groups_count];
    
    for(unsigned int i = 0;i<groups_count;i++)
    {
        unsigned int mat_id = 0;
        
        f->Read(&(faces_count[i]),sizeof(int));
        f->Read(&mat_id,sizeof(int));
        
        m_Faces += faces_count[i];
        
        fcb[i] = new unsigned int[faces_count[i]*3];
        f->Read(fcb[i],faces_count[i]*sizeof(int)*3);
        
        if(mat_vec)
        {
            if(mat_id > (*mat_vec).size())
                THROW_EXCEPTION("Material id is bigger than material list size");
            
            m_Materials.push_back((*mat_vec)[mat_id]);
        }
    }
    
    try{
        m_Mesh = m_Device->CreateMesh(m_Vertices,vp,vbb,groups_count,faces_count,fcb);
    }
    catch(Core::Exception & ex)
    {
        THROW_EXCEPTION_STACK("Unable to create mesh",ex);
    }
    
    
    if(vbb)
        SafeArrayDelete(vbb);
    
    if(faces_count)
        SafeArrayDelete(faces_count);
    
    for(unsigned int i = 0; i < groups_count; i++)
        if(fcb[i])
            SafeArrayDelete(fcb[i]);
    
    if(fcb)
        SafeArrayDelete(fcb);
}

Graph::VertexProfile CreateVertexProfile(unsigned int uvlayers, bool tangent, bool m_Skinning, bool color)
{
    unsigned int velements = 2;
    
    velements+=uvlayers;
    
    if(tangent)
        velements+=1;
    
    if(m_Skinning)
        velements+=2;
    
    if(color)
        velements+=1;
    
    Graph::VertexProfile ret(velements);
    
    unsigned int components = 3;
    unsigned int type		= 1;
    unsigned int offset		= 0;
    unsigned int layer		= 0;
    
    
    // Vertex.
    components = 3;
    type		= 1;
    ret.PushBack(offset, components, type, layer);
    offset+= 4*3; // 3 *float
    
    // Normal.
    components = 3;
    type		= 2;
    ret.PushBack(offset, components, type, layer);
    offset+= 4*3; // 3 *float
    
    // UV.
    components  = 2;
    type		= 4;
    
    for(unsigned int i = 0;i < uvlayers; i++)
    {
        ret.PushBack(offset, components, type, layer);
        offset+= 4*2; // 2 *float
        layer++;
    }
    
    // Tangent vectors.
    if(tangent)
    {
        // pesos dos bones
        components = 4;
        type		= 4;
        ret.PushBack(offset, components, type, layer);
        offset		+= 4*4; // 4 *float
        layer++;
    }
    
    // Skinning data.
    if(m_Skinning)
    {
        // m_Id dos bones
        components = 4;
        type		= 4;
        ret.PushBack(offset, components, type, layer);
        offset		+= 4*4; // 4 *float
        layer++;
        
        // pesos dos bones
        components = 4;
        type		= 4;
        ret.PushBack(offset, components, type, layer);
        offset		+= 4*4;  // 4 *float
        layer++;
    }
    
    // Color
    if(color)
    {
        components = 4;
        type		= 8;
        layer		= 0;
        ret.PushBack(offset, components, type, layer);
        offset		+= 4; // 4 * unsigned char
    }
    
    return ret;
}


Graph::VertexProfile CreateVertexProfile(unsigned int uvlayers, int channels[8], bool tangent, bool m_Skinning, bool color)
{
    unsigned int velements = 2;

    velements += uvlayers;

    if (tangent)
        velements += 1;

    if (m_Skinning)
        velements += 2;

    if (color)
        velements += 1;

    Graph::VertexProfile ret(velements);

    unsigned int components = 3;
    unsigned int type = 1;
    unsigned int offset = 0;
    unsigned int layer = 0;


    // Vertex
    components = 3;
    type = 1;
    ret.PushBack(offset, components, type, layer);
    offset += 4 * 3; // 3 *float

    // Normal
    components = 3;
    type = 2;
    ret.PushBack(offset, components, type, layer);
    offset += 4 * 3; // 3 *float

    // UV
    type = 4;

    for (unsigned int i = 0; i < uvlayers; i++)
    {
        ret.PushBack(offset, channels[i], type, layer);
        offset += 4 * channels[i];
        layer++;
    }

    // Tangent vectors
    if (tangent)
    {
        // pesos dos bones
        components = 4;
        type = 4;
        ret.PushBack(offset, components, type, layer);
        offset += 4 * 4; // 4 *float
        layer++;
    }

    // Skinning data
    if (m_Skinning)
    {
        // m_Id dos bones
        components = 4;
        type = 4;
        ret.PushBack(offset, components, type, layer);
        offset += 4 * 4; // 4 *float
        layer++;

        // pesos dos bones
        components = 4;
        type = 4;
        ret.PushBack(offset, components, type, layer);
        offset += 4 * 4;  // 4 *float
        layer++;
    }

    // Color
    if (color)
    {
        components = 4;
        type = 8;
        layer = 0;
        ret.PushBack(offset, components, type, layer);
        offset += 4; // 4 * unsigned char
    }

    return ret;
}

Core::QueueBuffer * GetMeshVertexBuffer(Geometry::Mesh * mesh,
                                        Graph::VertexProfile * vp,
                                        bool useTangent,
                                        const std::map<std::string, int> & boneIds,
                                        uint32_t faces,
                                        Geometry::XTriangleFace * faces_buffer)
{
    void *                      vertex_uv[8] = { NULL };
    int                         vertex_uv_count[8] = { 0 };
    Math::Vec4 *                vertex_tangent = NULL;
    Math::Vec3 *                vertex_pos = NULL;
    Math::Vec3 *                vertex_nor = NULL;
    Math::Color4ub *            vertex_col = NULL;
    Geometry::XVertexSkinning * vertex_skinning = NULL;
    
    bool l_UseTextures = mesh->m_UVLayers.size() > 0;
    bool l_UseSkinning = mesh->m_HasVertexSkinning;
    bool l_UseColors = mesh->m_ColorLayers.size() > 0;
    
    Geometry::GetVertexBuffer(mesh, &vertex_pos, &vertex_nor);
    
    if(l_UseTextures)
    {
        for (unsigned int i = 0; i < mesh->m_UVLayers.size(); i++)
        {
            int channels = mesh->m_Faces.front()->m_fUV.GetChannels(i);

            if (channels == 2) {
                Geometry::GetTextureCoordsUV(mesh, (Math::Vec2**)(vertex_uv + i), i);
                vertex_uv_count[i] = 2;
            }
            else if (channels == 3) {
                Geometry::GetTextureCoordsUVZ(mesh, (Math::Vec3**)(vertex_uv + i), i);
                vertex_uv_count[i] = 3;
            }
            else if (channels == 4) {
                Geometry::GetTextureCoordsUVZW(mesh, (Math::Vec4**)(vertex_uv + i), i);
                vertex_uv_count[i] = 4;
            }
        }

        if(useTangent){
            if (vertex_uv_count[0] == 2) {
                GetTangentVectorBuffer((uint32_t)mesh->m_Vertices.size(),
                    faces, vertex_pos, vertex_nor, (Math::Vec2*)(vertex_uv[0]),
                    faces_buffer, &vertex_tangent);
            }
            else {
                Core::DebugLog("Base layer is not Vec2");
                useTangent = false;
            }
        }
    }
    
    if(l_UseSkinning){
        Geometry::GetVertexSkinning(mesh, boneIds, &vertex_skinning);
    }
    
    *vp = CreateVertexProfile((unsigned int)mesh->m_UVLayers.size(), vertex_uv_count, useTangent, l_UseSkinning, l_UseColors);
    
    int vertices = (uint32_t)mesh->m_Vertices.size();
    
    Core::QueueBuffer * vertexBuffer = new Core::QueueBuffer(vertices * vp->GetVertexSize());
    
    for(uint32_t i = 0;i < vertices; i++)
    {
        vertexBuffer->Push(vertex_pos+i, sizeof(Math::Vec3));
        vertexBuffer->Push(vertex_nor+i, sizeof(Math::Vec3));
        
        for( unsigned int j = 0 ; j < mesh->m_UVLayers.size() ; j++)
        {
            int count = vertex_uv_count[j];
            if (count == 2) {
                Math::Vec2 uv = ((Math::Vec2*)vertex_uv[j])[i];
                vertexBuffer->Push((uint8_t*)&uv, sizeof(Math::Vec2));
            } else if (count == 3) {
                Math::Vec3 uvz = ((Math::Vec3*)vertex_uv[j])[i];
                vertexBuffer->Push((uint8_t*)&uvz, sizeof(Math::Vec3));
            } else if (count == 4) {
                Math::Vec4 uvz = ((Math::Vec4*)vertex_uv[j])[i];
                vertexBuffer->Push((uint8_t*)&uvz, sizeof(Math::Vec4));
            }
        }
        
        if(useTangent)
            vertexBuffer->Push((uint8_t*)&vertex_tangent[i], sizeof(Math::Vec4));
        
        if(l_UseSkinning){
            vertexBuffer->Push((uint8_t*)&vertex_skinning[i].m_Bone_Id, sizeof(int)*4);
            vertexBuffer->Push((uint8_t*)&vertex_skinning[i].m_Bone_Weight, sizeof(Math::Vec4));
        }
        
        if(l_UseColors)
            vertexBuffer->Push((uint8_t*)&vertex_col[i], sizeof(Math::Color4ub));
    }
    

    for (unsigned int j = 0; j < mesh->m_UVLayers.size(); j++) {
        SafeArrayDelete(vertex_uv[j]);
    }
    
    SafeArrayDelete(vertex_tangent);
    SafeArrayDelete(vertex_pos);
    SafeArrayDelete(vertex_nor);
    SafeArrayDelete(vertex_col);
    SafeArrayDelete(vertex_skinning);
    
    return vertexBuffer;
}

#ifdef NCK_BXON
void Model::Read(BXON::Map * entry, const std::map<std::string, Datablock *> & mMap, const std::map<std::string, Datablock*> & aMap, Processor  * processor)
{
    m_Name = entry->GetString("name");
    
    Geometry::XTriangleFace * faces_buffer = NULL;
    
    BXON::Array * mats = entry->GetArray("materials");
    
    bool useTangent = false;
    
    if(mats != NULL)
    {
        m_Materials.reserve(mats->GetSize());
        
        for(uint32_t i = 0; i < mats->GetSize(); i++)
        {
            std::string name = mats->GetString(i);
            if(mMap.find(name) != mMap.end()){
                Material * m = dynamic_cast<Material*>(mMap.find(name)->second);
                m_Materials.push_back(m);
                
                for(uint32_t j = 0;j<8;j++)
                    if(m->GetTextureLayer(j) != NULL && m->GetTextureLayer(j)->GetFactorFlag() & FACTOR_NORMAL)
                        useTangent = true;
            }
        }
    }
    
    Geometry::Mesh * msh = Geometry::Mesh::Parse(entry);

    if (processor) msh = processor->HandleGeometry(msh);

    if(msh->m_UVLayers.size() > 0)
    {
        for(unsigned int i = 0;i<msh->m_UVLayers.size();i++)
        {
            if(msh->m_UVOptimization[i])
                Geometry::OptimizeSeams(msh, i);
        }
    }
    
    Geometry::GetFaceBuffer(msh, &faces_buffer, &m_Faces, useTangent);
    m_BoundBox = msh->m_BoundBox;
    
    m_Vertices = (uint32_t)msh->m_Vertices.size();
    
    Graph::VertexProfile vp;
    
    std::map<std::string,int> boneIds;
    if(entry->HasKey("armature"))
    {
        if(aMap.find(entry->GetString("armature")) != aMap.end()){
            Armature * arm = dynamic_cast<Armature*>(aMap.find(entry->GetString("armature"))->second);
            std::vector<Bone*> bones = arm->GetBones();
            for(uint32_t i = 0; i < bones.size(); i++){
                boneIds.insert(std::pair<std::string,int>(bones[i]->m_Name,i));
            }
        }
    }
    Core::QueueBuffer * vertexBuffer = GetMeshVertexBuffer(msh, &vp, useTangent, boneIds, m_Faces, faces_buffer);
    
    uint32_t groupsCount = m_Materials.size() > 0 ? (uint32_t)m_Materials.size() : 1;
    
    std::vector<std::list<Geometry::XTriangleFace*> > facesPerGroup(groupsCount);
    
    for(uint32_t i = 0; i < m_Faces; i++)
        facesPerGroup[faces_buffer[i].m_MaterialIndex].push_back(faces_buffer + i);
    
    uint32_t * fbCount = new unsigned int[groupsCount];
    uint32_t ** fbVertices = new unsigned int * [groupsCount];
    
    for(unsigned int i = 0 ; i < groupsCount ; i++)
    {
        fbVertices[i] = new uint32_t[facesPerGroup[i].size() * 3];
        fbCount[i] = (uint32_t)facesPerGroup[i].size();
        
        int j = 0;
        ListFor(Geometry::XTriangleFace *, facesPerGroup[i], f_i)
        {
            fbVertices[i][j++] = (*f_i)->m_Vertices[0];
            fbVertices[i][j++] = (*f_i)->m_Vertices[1];
            fbVertices[i][j++] = (*f_i)->m_Vertices[2];
        }
    }
    
    try{
        m_Mesh = m_Device->CreateMesh(m_Vertices, vp, vertexBuffer->GetData(), groupsCount, fbCount, fbVertices);
    }
    catch(Core::Exception & ex){
        THROW_EXCEPTION_STACK("Unable to create mesh",ex);
    }
    
    SafeDelete(vertexBuffer);
    
    for(uint32_t i = 0;i < groupsCount;i++)
        SafeArrayDelete(fbVertices[i]);
    
    SafeArrayDelete(fbCount);
    SafeArrayDelete(faces_buffer);
    
    SafeDelete(msh);
}
#endif

Graph::VertexProfile Model::ReadVProfile( Core::DataReader *f )
{
    unsigned int elements = 0;
    f->Read(&elements,sizeof(unsigned int));
    Graph::VertexProfile vp(elements);
    
    for(unsigned int i = 0;i<elements;i++)
    {
        unsigned int type;
        unsigned int components;
        unsigned int offset;
        unsigned int index;
        
        f->Read(&components,sizeof(unsigned int));
        f->Read(&type,sizeof(unsigned int));
        f->Read(&offset,sizeof(unsigned int));
        f->Read(&index,sizeof(unsigned int));
        
        vp.PushBack(offset,components,type,index);
    }
    
    return vp;
}

DatablockType Model::GetType()
{
    return DATABLOCK_MODEL;
}

std::vector<Material*> Model::GetMaterials(){
    return m_Materials;
}

_SCENE_END
