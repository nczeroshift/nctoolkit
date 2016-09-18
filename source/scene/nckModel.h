
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_MODEL_H
#define NCK_MODEL_H

#include "nckMaterial.h"
#include "nckBoundBox.h"
#include "nckProcessor.h"

_SCENE_BEGIN

/// Encapsulation class for GphMesh.
class Model : public Datablock
{
public:
    /// Constructor.
    Model(Graph::Device *dev);
    
    /// Destructor.
    ~Model();
    
    /// Read model information from file.
    /// @param f Reference to file stream.
    /// @param tex_vec Reference to loaded materials vector.
    /// @throws Exception.
    void Read(Core::DataReader * f,std::vector<Material *> * mat_vec);
    
#ifdef NCK_BXON
    void Read(BXON::Map * entry, const std::map<std::string, Datablock *> & mMap, const std::map<std::string, Datablock*> & aMap, Processor  * processor = NULL);
#endif
    
    /// Render model
    void Render(Material *overlap = NULL);
    
    /// Get model bound box.
    Math::BoundBox GetBoundBox();
    
    /// Get the number of vertices.
    unsigned int GetVertices();
    
    /// Get the number of faces.
    unsigned int GetFaces();
    
    /// Get a copy of materials vector.
    std::vector<Material*> GetMaterials();
    
    /// Get datablock type.
    DatablockType GetType();
private:
    
    /// Read vertex configuration from file.
    Graph::VertexProfile ReadVProfile( Core::DataReader *f );
    
    /// Model mesh.
    Graph::Mesh *m_Mesh;
    
    /// Mesh vertices count.
    unsigned int m_Vertices;
    
    /// Mesh faces count.
    unsigned int m_Faces;
    
    /// Material reference per mesh subset.
    std::vector<Material*> m_Materials;
    
    /// Model boundbox.
    Math::BoundBox m_BoundBox;
};

_SCENE_END

#endif // #ifndef NCK_MODEL_H
