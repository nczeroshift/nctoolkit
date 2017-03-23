
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_META_GEOMETRY_H
#define NCK_META_GEOMETRY_H

#include "nckGraphics.h"
#include "nckSceneConfig.h"
#include "nckVec3.h"
#include <vector>

/*
 * Note : This rendering system as a limit of 500k triangles and 15M vertices configurable in the cpp.
 */

_SCENE_BEGIN

class MCSphereShape{
public:
    MCSphereShape();
    MCSphereShape(const Math::Vec3 & pos, float s);
    Math::Vec3 position;
    float ssize;
};

class MCPrivate;

class MCRenderer{
public:
    /**
     * Default constructor.
     * @param dev Reference to the graphics device.
     * @param maxDetail Maximum lod.
     * @param targetDetail Active lod in usage.
     * @param dimensions Each axis maximum distance.
     */
    MCRenderer(Graph::Device * dev,int maxDetail, int targetDetail,float dimensions = 20.0f);
    
    /**
     * Default destructor.
     */
    ~MCRenderer();
    
    /**
     * Reset vertices to intial values.
     */
    void Clear();
    
    /**
     * Compute distance and normal functions for all the vertices.
     */
    void ApplySpheres(const std::vector<MCSphereShape> & spheres);
    
    /**
     * Set level of detail and reset.
     */
    void SetDetail(int detailLevel);
    
    /**
     * Get level of detail.
     */
    int GetDetail();
    
    /**
     * Get the maximum level of detail.
     */
    int GetMaxDetail();
    
    /**
     * Compute marching cubes algorithm.
     */
    void Update(float threshold = 1.0f);
    
    /**
     * Render mesh.
     */
    void Render();
    
    /**
     * Get the number of generated vertices.
     */
    int GetVerticesCount();
    
    /**
     * Get the number of generated triangles.
     */
    int GetTrianglesCount();
    
private:
    void Reset(int detailLevel);
    float mDimensions;
    int mMaxGridDetail;
    int mActiveDetail;
    int mActiveVertices;
    int mActiveCubes;
    
    MCPrivate * mPrivate;
    Graph::DynamicMesh * mDynamicMesh;
};

_SCENE_END

#endif // #ifndef NCK_META_GEOMETRY_H
