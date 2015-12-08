
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckMarchingCubes.h"
#include "nckMarchingCubesTable.h"
#include "nckUtils.h"
#include <string.h>
#include <stdio.h>

#define MAX_TRIANGLES 500000
#define MAX_VERTICES 1500000

_SCENE_BEGIN

MCSphereShape::MCSphereShape(){
    ssize = 1.0;
}

MCSphereShape::MCSphereShape(const Math::Vec3 & pos, float s){
    position = pos;
    ssize = s*s;
}

struct MVertex{
    float position[3];
    float normal[3];
    float uv[2];		// UV0
    float value,special;	// UV1
    
    inline void setPosition(float x, float y, float z){
        position[0] = x;position[1] = y;position[2] = z;
    }
    
    inline void setNormal(float x, float y, float z){
        normal[0] = x;normal[1] = y;normal[2] = z;
    }
    
    inline void set(float p[3],float n[3]){
        position[0] = p[0];
        position[1] = p[1];
        position[2] = p[2];
        normal[0] = n[0];
        normal[1] = n[1];
        normal[2] = n[2];
    }
};

struct MCube{
    MVertex *mVertices[8];
};

class MCPrivate{
public:
    MCube *mCubes;
    MVertex *mVertices;
    MVertex *mOriginalVertices;
    unsigned int *mGPUTriangles;
    MVertex *mGPUVertices;
};

MCRenderer::MCRenderer(Graph::Device * dev,int maxDetail, int targetDetail,float dimensions)
{
    mMaxGridDetail = maxDetail;
    mActiveDetail = targetDetail;
    mDimensions = dimensions;
    mPrivate = new MCPrivate();
    
    mPrivate->mVertices = new MVertex[(mMaxGridDetail+1)*(mMaxGridDetail+1)*(mMaxGridDetail+1)];
    mPrivate->mOriginalVertices = new MVertex[(mMaxGridDetail+1)*(mMaxGridDetail+1)*(mMaxGridDetail+1)];
    mPrivate->mCubes = new MCube[mMaxGridDetail*mMaxGridDetail*mMaxGridDetail];
    
    memset(mPrivate->mCubes,0,sizeof(MCube)*mMaxGridDetail*mMaxGridDetail*mMaxGridDetail);
    memset(mPrivate->mOriginalVertices,0,sizeof(MVertex)*(mMaxGridDetail+1)*(mMaxGridDetail+1)*(mMaxGridDetail+1));
    memset(mPrivate->mVertices,0,sizeof(MVertex)*(mMaxGridDetail+1)*(mMaxGridDetail+1)*(mMaxGridDetail+1));
    
    mPrivate->mGPUTriangles = new unsigned int[MAX_TRIANGLES*3];
    mPrivate->mGPUVertices = new MVertex[MAX_VERTICES];
    
    memset(mPrivate->mGPUVertices,0,sizeof(MVertex)*MAX_VERTICES);
    
    mDynamicMesh = dev->CreateDynamicMesh(MAX_VERTICES,MAX_TRIANGLES);
    
    for(int i = 0;i<MAX_TRIANGLES;i++)
    {
        mPrivate->mGPUTriangles[i*3] = i*3;
        mPrivate->mGPUTriangles[i*3+1] = i*3+1;
        mPrivate->mGPUTriangles[i*3+2] = i*3+2;
    }
    
    void * ibo_data = NULL;
    mDynamicMesh->LockIBO(&ibo_data);
    memcpy(ibo_data,mPrivate->mGPUTriangles,sizeof(int)*MAX_TRIANGLES*3);
    mDynamicMesh->UnlockIBO();
    
    Reset(mActiveDetail);
}

MCRenderer::~MCRenderer(){
    SafeDelete(mDynamicMesh);
    SafeArrayDelete(mPrivate->mVertices);
    SafeArrayDelete(mPrivate->mCubes);
    SafeArrayDelete(mPrivate->mOriginalVertices);
    SafeDelete(mPrivate);
}

void MCRenderer::Clear(){
    memcpy(mPrivate->mVertices,mPrivate->mOriginalVertices,sizeof(MVertex)*(mMaxGridDetail+1)*(mMaxGridDetail+1)*(mMaxGridDetail+1));
}

void MCRenderer::SetDetail(int detailLevel){
    Reset(detailLevel);
}

int MCRenderer::GetDetail(){
    return mActiveDetail;
}

int MCRenderer::GetMaxDetail(){
    return mMaxGridDetail;
}

void MCRenderer::Reset(int detailLevel){
    mActiveDetail = detailLevel;
    
    if(mActiveDetail<1)
        mActiveDetail = 1;
    if(mActiveDetail>mMaxGridDetail)
        mActiveDetail = mMaxGridDetail;
    
    int count=0;
    
    mActiveVertices=(mActiveDetail+1)*(mActiveDetail+1)*(mActiveDetail+1);
    mActiveCubes=(mActiveDetail)*(mActiveDetail)*(mActiveDetail);
    
    MCube * cubes = mPrivate->mCubes;
    MVertex * verts = mPrivate->mVertices;
    MVertex * o_verts = mPrivate->mOriginalVertices;
    
    float halfDim = mDimensions*0.5;
    
    for(int i=0; i<mActiveDetail+1; i++)
        for(int j=0; j<mActiveDetail+1; j++)
            for(int k=0; k<mActiveDetail+1; k++)
            {
                o_verts[count].setPosition((i*mDimensions)/(mActiveDetail)-halfDim,
                                           (j*mDimensions)/(mActiveDetail)-halfDim, (k*mDimensions)/(mActiveDetail)-halfDim);
                
                verts[count++].setPosition((i*mDimensions)/(mActiveDetail)-halfDim,
                                           (j*mDimensions)/(mActiveDetail)-halfDim, (k*mDimensions)/(mActiveDetail)-halfDim);
            }
    
    count = 0;
    for(int i=0; i<mActiveDetail; i++)
        for(int j=0; j<mActiveDetail; j++)
            for(int k=0; k<mActiveDetail; k++)
            {
                cubes[count].mVertices[0]=verts + (i*(mActiveDetail+1)+j)*(mActiveDetail+1)+k;
                cubes[count].mVertices[1]=verts + (i*(mActiveDetail+1)+j)*(mActiveDetail+1)+k+1;
                cubes[count].mVertices[2]=verts + (i*(mActiveDetail+1)+(j+1))*(mActiveDetail+1)+k+1;
                cubes[count].mVertices[3]=verts + (i*(mActiveDetail+1)+(j+1))*(mActiveDetail+1)+k;
                cubes[count].mVertices[4]=verts + ((i+1)*(mActiveDetail+1)+j)*(mActiveDetail+1)+k;
                cubes[count].mVertices[5]=verts + ((i+1)*(mActiveDetail+1)+j)*(mActiveDetail+1)+k+1;
                cubes[count].mVertices[6]=verts + ((i+1)*(mActiveDetail+1)+(j+1))*(mActiveDetail+1)+k+1;
                cubes[count].mVertices[7]=verts + ((i+1)*(mActiveDetail+1)+(j+1))*(mActiveDetail+1)+k;
                count++;
            }
}


void MCRenderer::Update(float threshold)
{
    MVertex mEdgeVertices[12];
    int totalFaces = 0;
    int vertCount = 0;
    
    for(int i=0; i<mActiveCubes; i++)
    {
        unsigned char cubeIndex=0;
        MCube * cube = mPrivate->mCubes+i;
        
        if(cube->mVertices[0]->value < threshold)
            cubeIndex |= 1;
        if(cube->mVertices[1]->value < threshold)
            cubeIndex |= 2;
        if(cube->mVertices[2]->value < threshold)
            cubeIndex |= 4;
        if(cube->mVertices[3]->value < threshold)
            cubeIndex |= 8;
        if(cube->mVertices[4]->value < threshold)
            cubeIndex |= 16;
        if(cube->mVertices[5]->value < threshold)
            cubeIndex |= 32;
        if(cube->mVertices[6]->value < threshold)
            cubeIndex |= 64;
        if(cube->mVertices[7]->value < threshold)
            cubeIndex |= 128;
        
        int usedEdges=sEdgeTable[cubeIndex];
        
        if(usedEdges==0 || usedEdges==255)
            continue;
        
        for(int currentEdge=0; currentEdge<12; currentEdge++)
        {
            if(usedEdges & 1<<currentEdge)
            {
                MVertex * v1=cube->mVertices[sVerticeTable[currentEdge*2]];
                MVertex * v2=cube->mVertices[sVerticeTable[currentEdge*2+1]];
                
                float delta=(threshold - v1->value)/(v2->value - v1->value);
                
                mEdgeVertices[currentEdge].setPosition(
                                                       v1->position[0] + delta*(v2->position[0] - v1->position[0]),
                                                       v1->position[1] + delta*(v2->position[1] - v1->position[1]),
                                                       v1->position[2] + delta*(v2->position[2] - v1->position[2]));
                
                mEdgeVertices[currentEdge].setNormal(
                                                     v1->normal[0] + delta*(v2->normal[0] - v1->normal[0]),
                                                     v1->normal[1] + delta*(v2->normal[1] - v1->normal[1]),
                                                     v1->normal[2] + delta*(v2->normal[2] - v1->normal[2]));
            }
        }
        
        for(int k=0; sTriangleTable[cubeIndex][k]!=-1; k+=3)
        {
            mPrivate->mGPUVertices[vertCount++].set(
                                                    mEdgeVertices[sTriangleTable[cubeIndex][k+0]].position,
                                                    mEdgeVertices[sTriangleTable[cubeIndex][k+0]].normal
                                                    );
            
            mPrivate->mGPUVertices[vertCount++].set(
                                                    mEdgeVertices[sTriangleTable[cubeIndex][k+2]].position,
                                                    mEdgeVertices[sTriangleTable[cubeIndex][k+2]].normal
                                                    );
            
            mPrivate->mGPUVertices[vertCount++].set(
                                                    mEdgeVertices[sTriangleTable[cubeIndex][k+1]].position,
                                                    mEdgeVertices[sTriangleTable[cubeIndex][k+1]].normal
                                                    );
            
            totalFaces++;
        }
    }
    
    mDynamicMesh->SetVerticesCount(vertCount);
    mDynamicMesh->SetTrianglesCount(totalFaces);
    
    void * vbo_data = NULL;
    mDynamicMesh->LockVBO(&vbo_data);
    memcpy(vbo_data,mPrivate->mGPUVertices,sizeof(MVertex)*vertCount);
    mDynamicMesh->UnlockVBO();
}

void MCRenderer::Render(){
    mDynamicMesh->Enable();
    mDynamicMesh->Render();
    mDynamicMesh->Disable();
}

int MCRenderer::GetVerticesCount(){
    return mDynamicMesh->GetVerticesCount();
}

int MCRenderer::GetTrianglesCount(){
    return mDynamicMesh->GetTrianglesCount();
}


void MCRenderer::ApplySpheres(const std::vector<MCSphereShape> & spheres)
{
    const size_t sphCount = spheres.size();
    
    for(int j=0; j<mActiveVertices; j++)
    {  
        MVertex * vertex = mPrivate->mVertices + j;
        
        for(int i = 0;i<sphCount;i++){
            float ballToPoint[3] = {vertex->position[0] - spheres[i].position.GetX(),
                vertex->position[1] - spheres[i].position.GetY(),
                vertex->position[2] - spheres[i].position.GetZ()};
            
            float squaredDistance = ballToPoint[0]*ballToPoint[0] + ballToPoint[1]*ballToPoint[1] + ballToPoint[2]*ballToPoint[2];
            
            if(squaredDistance==0.0f) squaredDistance=0.0001f;
            
            mPrivate->mVertices[j].value+=spheres[i].ssize/squaredDistance;
            
            float normalScale=spheres[i].ssize/(squaredDistance*squaredDistance);
            
            vertex->normal[0]+=ballToPoint[0]*normalScale;
            vertex->normal[1]+=ballToPoint[1]*normalScale;
            vertex->normal[2]+=ballToPoint[2]*normalScale;
        }
    }
}

_SCENE_END
