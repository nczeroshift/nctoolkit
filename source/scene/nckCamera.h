
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_CAMERA_H
#define NCK_CAMERA_H

#include "nckObject.h"

_SCENE_BEGIN

/// Datablock for scene camera.
class Camera: public Datablock
{
public:
    /// Constructor.
    Camera(Graph::Device *dev);
    
    /// Destructor.
    ~Camera();
    
    /// Set vizualization volume aspect relation.
    void SetAspect(float value);
    
    /// Get vizualization volume aspect relation.
    float GetAspect();
    
    /// Get camera fov.
    float GetFov();
    
    /// Set camera fov.
    void SetFov(float fFov);
    
    /// Set camera transformation manipulator.
    void SetObject(Object *obj);
    
    /// Get camera transformation manipulator.
    Object *GetObject();
    
    /// Get camera view matrix.
    Math::Mat44 GetMatrix();
    
    /// Enable camera transformation.
    /// @param mat Use GPH_PROJECTION_MATRIX to set perspective
    /// projection volume properties and GPH_VIEW_MATRIX, to set
    /// view matrix properties.
    void Enable(Graph::MatrixType mat = Graph::MATRIX_VIEW);
    
    /// Set near clip distance.
    void SetStart(float value);
    
    /// Get near clip distance.
    float GetStart();
    
    /// Set end clip distance.
    void SetEnd(float value);
    
    /// Get end clip distance.
    float GetEnd();
    
    /// Read camera properties from file.
    /// @param f Reference to file stream.
    void Read(Core::DataReader *f);
    
#ifdef NCK_BXON
    /// Read camera properties from bxon map.
    void Read(BXON::Map * entry);
#endif
    
    /// Get datablock type.
    DatablockType GetType();
    
    /// Render viewport contour.
    void RenderViewportContour();
    
private:
    
    /// Vizualization volume aspect relation.
    float m_Aspect;
    
    /// Near clip distance.
    float m_Start;
    
    /// End clip distance.
    float m_End;
    
    /// Reference to camera transformation manipulator.
    Object *m_Object;
    
    /// Camera lens(Degrees).
    float m_Fov;
};

_SCENE_END

#endif // #ifndef NCK_CAMERA_H
