
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_GEOMETRY_H
#define NCK_GEOMETRY_H

#include <string>
#include <list>

#include "nckSceneConfig.h"
#include "nckVec2.h"
#include "nckVec3.h"
#include "nckVec4.h"
#include "nckBoundBox.h"
#include "nckUtils.h"

#ifdef NCK_BXON
#include "bxon.hpp"
#endif

#define _GEOMETRY_BEGIN namespace Geometry{
#define _GEOMETRY_END }

_GEOMETRY_BEGIN

/**
 * Auxiliar class to implement helper objects.
 */
class Auxiliar
{
public:
    virtual ~Auxiliar(){};
};


/**
 * Mesh vertex group, for bone skinning mapping.
 */
class VertexGroup
{
public:
    /// Constructor.
    VertexGroup();
    
    /// Destructor.
    ~VertexGroup();
    
    /// Group Name.
    std::string m_Name;
    
    /// Reference to auxiliar object.
    Auxiliar * auxiliar;
    
#ifdef NCK_BXON
    static void Parse(BXON::Map * mesh, std::vector<VertexGroup*> * v);
#endif
    
};


class Vertex;


/**
 * Mesh vertex skinning, with assignment group and influence.
 */
class VertexSkinning
{
public:
    /// Contructor.
    VertexSkinning();
    
    /// Destructor.
    ~VertexSkinning();
    
    /// Reference to assigned vertice group.
    VertexGroup * m_Group;
    
    /// Influence value to the vertice group, defined from 0.0 to 1.0.
    float m_Weight;
    
    /// Copy operator, creates a new vertice skinning
    /// with the same state as the original.
    void operator = (const VertexSkinning &v);
    
#ifdef NCK_BXON
    static bool Parse(BXON::Map * mesh, std::list<Vertex*> * v, const std::vector<VertexGroup*> & groups);
#endif
};


/**
 * Mesh vertex class with position, normals and skinning information.
 */
class Vertex
{
public:
    /// Constructor.
    Vertex();
    
    /// Destructor.
    ~Vertex();
    
    /// Vertex position.
    Math::Vec3 m_Pos;
    
    /// Vertex normal vector.
    Math::Vec3 m_Nor;
    
    /// Vertex skinning weights and assigned group.
    std::vector<VertexSkinning*> m_Skinning;
    
    /// Reference for auxiliar information used in algoritms.
    Auxiliar * m_Auxiliar;
    
    /// Vertex sequencial identification.
    int m_Id;
    
    /// Copy operator, copies the state of other vertice.
    /// @param v Reference to vertice which will be copied.
    void operator = (const Vertex &v);
    
    /// Method which defines the partitioning rule for the vertices.
    bool Compare(const Math::BoundBox &bb);
    
#ifdef NCK_BXON
    static void Parse(BXON::Map * mesh, std::list<Vertex*> * vOut);
#endif
};

/// Typedef to improve code legibility.
typedef std::list<Vertex*>::iterator VertexIterator;


/**
 * Mesh vertex new positions in a shape.
 */
class ShapeVertex
{
public:
    /// Constructor.
    ShapeVertex();
    
    /// Destructor.
    ~ShapeVertex();
    
    /// Vertex new position.
    Math::Vec3 m_Pos;
    
    /// Vertex new normal.
    Math::Vec3 m_Nor;
    
    /// Iterator with reference to the assigned vertice which
    /// will be modified.
    VertexIterator m_Vertice;
    
#ifdef NCK_BXON
    static void Parse(BXON::Map * mesh, std::list<ShapeVertex*> * vOut, const std::vector<VertexIterator*> & vIter);
#endif
};


/**
 * Shape key with the vertices used in the shape.
 */
class ShapeKey
{
public:
    /// Constructor.
    ShapeKey();
    
    /// Destructor.
    ~ShapeKey();
    
    /// Shape name.
    std::string m_Name;
    
    /// Shape vertices.
    std::list<ShapeVertex*> m_Vertices;
    
#ifdef NCK_BXON
    static void Parse(BXON::Map * mesh, const std::vector<VertexIterator*> & vIter);
#endif
};

/**
 * Store face uv data.
 */
class FaceUV {
public:
    FaceUV();
    ~FaceUV();

    void SetUV(int layer,int vId, float u, float v);
    void SetUV(int layer,int vId, const Math::Vec2 & uv);
    Math::Vec2 GetUV(int layer, int vId);

    void SetUVZ(int layer, int vId, float u, float v, float z);
    void SetUVZ(int layer, int vId, const Math::Vec3 & uvz);
    Math::Vec3 GetUVZ(int layer, int vId);

    void SetUVZW(int layer, int vId, float u, float v, float z, float w);
    void SetUVZW(int layer, int vId, const Math::Vec4 & uvzw);
    Math::Vec4 GetUVZW(int layer, int vId);

    int GetLayers();
    int GetChannels(int layer);
private:

    void Check(int layer, int vid);
    std::vector<int> m_Channels;
    std::vector<std::vector<Math::Vec4>> m_Coordinates;
};

/**
 * Mesh face, with 3 or 4 vertices plus uv and color values.
 */
class Face
{
public:
    /// Constructor.
    Face();
    
    /// Destructor.
    ~Face();
    
    /// Array with the iterators to vertices references.
    std::vector<VertexIterator> m_Verts;
    
    /// Assigned material index.
    int m_MatIndex;
    
    /// Assigned uv mapping coordinates.
    std::vector<std::vector<Math::Vec2>> m_UV;
    
    FaceUV m_fUV;

    /// Assigned vertex color values.
    std::vector<std::vector<Math::Color4ub>> m_Color;
    
    /// Reference for auxiliar information used in algoritms.
    Auxiliar * m_Auxiliar;
    
    /// Face sequencial identification.
    int m_Id;
    
    /// Method which defines the partitioning rule for the faces.
    bool Compare(const Math::BoundBox & bb);
    
    /// Copy operator, creates a new vertice with the same state as the original.
    void operator =(const Face &f);
    
#ifdef NCK_BXON
    static void Parse(BXON::Map * mesh, std::list<Face*> * fOut, const std::vector<VertexIterator> & vIter);
#endif
};


/// Typedef to improve code legibility.
typedef std::list<Face*>::iterator FaceIterator;


class XVertexSkinning
{
public:
    /// Constructor.
    XVertexSkinning();
    
    /// Bone id's
    int m_Bone_Id[16];
    
    /// Bones weights (0.0f - 1.0f)
    float m_Bone_Weight[16];
    
    /// Number of used bones.
    int m_Bone_Count;
};


class XTriangleFace
{
public:
    /// Constructor.
    XTriangleFace();
    
    /// Vertices index.
    int m_Vertices[3];
    
    /// Material id.
    int m_MaterialIndex;
    
    /// Copy operator.
    void operator=(const XTriangleFace & tf);
};


/**
 * Mesh geometry class with collection of vertices, faces, and geometry processing tools.
 */
class Mesh
{
public:
    /// Constructor.
    Mesh();
    
    /// Destructor.
    ~Mesh();
    
    /// Mesh boundaries.
    Math::BoundBox m_BoundBox;
    
    /// List of vertices.
    std::list<Vertex*> m_Vertices;
    
    /// List of faces.
    std::list<Face*> m_Faces;
    
    /// Array of groups names.
    std::vector<VertexGroup*> m_Groups;
    
    /// Array of shape keys.
    std::vector<ShapeKey *> m_ShapeKeys;
    
    /// UV layers names.
    std::vector<std::string> m_UVLayers;
    std::vector<std::pair<std::string,uint32_t>> m_UVLayers2;
    
    // Color layers names.
    std::vector<std::string> m_ColorLayers;
    
    bool m_HasVertexSkinning;
    
#ifdef NCK_BXON
    static Mesh * Parse(BXON::Map * entry);
#endif
    
    /**
     * Get vertex buffer basic data, vertices position and normal.
     * @parm mesh Reference to the mesh.
     * @parm pos Reference to array where the the vertices positions
     * will be written.
     * @parm pos Reference to array where the the vertices normals
     * will be written.
     */
    friend void GetVertexBuffer(Mesh * mesh, Math::Vec3 ** pos, Math::Vec3 ** nor);
    
    /**
     * Get vertex buffer skinning data, bones id and influences.
     * @parm mesh Reference to the mesh.
     * @parm vsk Reference to array where the the vertex skinning
     * data will be written.
     */
    friend void GetVertexSkinning(Mesh * mesh, const std::map<std::string,int> & boneIds, XVertexSkinning ** vsk);
    
    /**
     * Get vertex buffer colors.
     * @
     */
    friend void GetVertexColor(Mesh * mesh, Math::Color4ub ** col);
    
    /**
     * Get vertices texture coordinates
     * @parm mesh Reference to the mesh.
     * @parm uv Reference to array where the the vertex texture
     * coords data will be written.
     * @param layer_id UV layer id.
     */
    friend void GetTextureCoords(Mesh * mesh, Math::Vec2 ** uv, unsigned int layer_id);
    
    friend void GetTextureCoordsUV(Mesh * mesh, Math::Vec2 ** uv, unsigned int layer_id);
    friend void GetTextureCoordsUVZ(Mesh * mesh, Math::Vec3 ** uv, unsigned int layer_id);
    friend void GetTextureCoordsUVZW(Mesh * mesh, Math::Vec4 ** uv, unsigned int layer_id);
    
    /**
     * Solve mesh seams issues by creating new vertices to store
     * the problematic uv-coordinates.
     * @param mesh Reference to the mesh.
     * @param layer_id UV layer id.
     * @return Number of exploded
     */
    friend unsigned int OptimizeSeams(Mesh * mesh, unsigned int layer_id);
    
    /**
     * Compute mesh decals.
     * @param mesh Reference to the mesh.
     * @param facto Scale factor.
     */
    friend void MaterialBlending(Mesh * mesh, float factor, bool border_scale);
    
    /**
     * Get vertices index use in each face.
     * @param mesh Reference to the mesh.
     * @param tf Reference to array where the face data will be stored.
     * @param face_count Reference to atribute where the number of
     * faces whill be stored.
     * @param sort Enables face sorting by material id.
     */
    friend void GetFaceBuffer(Mesh * mesh , XTriangleFace ** tf,
                              unsigned int *face_count, bool sort );
    
    /**
     * Compute tangent vectors to mesh vertices.
     * @param vertices
     * @param faces
     * @param vb
     * @param nb
     * @param uv
     * @param fcb
     * @param t_coords
     */
    friend void GetTangentVectorBuffer(int vertices, int faces , Math::Vec3 * vb,
                                       Math::Vec3 * nb, Math::Vec2 * uv, XTriangleFace * fcb,
                                       Math::Vec4 ** t_coords);
};


void GetVertexBuffer(Mesh * mesh, Math::Vec3 **pos, Math::Vec3 ** nor);
void GetVertexSkinning(Mesh * mesh, const std::map<std::string, int> & boneIds, XVertexSkinning ** vsk);
void GetVertexColor(Mesh * mesh, Math::Color4ub **col);
void GetTextureCoords(Mesh * mesh, Math::Vec2 ** uv, unsigned int layer_id);
unsigned int OptimizeSeams(Mesh * mesh, unsigned int layer_id);
unsigned int OptimizeSeams2(Mesh * mesh, unsigned int layer_id);
void MaterialBlending(Mesh * mesh, float factor, bool border_scale);
void GetFaceBuffer(Mesh * mesh, XTriangleFace ** tf, unsigned int *face_count, bool sort);
void GetTangentVectorBuffer(int vertices,int faces,Math::Vec3 * vb, Math::Vec3 * nb, Math::Vec2 * uv, XTriangleFace *fcb, Math::Vec4 ** t_coords);


/// Typedef to improve code legibility.
typedef std::list<Mesh*>::iterator MeshIterator;

_GEOMETRY_END


#endif
