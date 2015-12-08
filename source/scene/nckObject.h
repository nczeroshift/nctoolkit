
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_OBJECT_H
#define NCK_OBJECT_H

#include "nckGraphics.h"
#include "nckDatablock.h"
#include "nckAnimation.h"
#include "nckMat44.h"
#include "nckVec3.h"
#include "nckQuat.h"
#include "bxon.hpp"
#include <vector>
#include <list>

_SCENE_BEGIN

class Object;

/// Group of animation graphs for object motion.
/*class ObjectAction{
 public:
	/// Default constructor.
	ObjectAction();
 
	/// Destructor.
	~ObjectAction();
 
	/// Play object action animation.
	void Play(Object *obj, float time);
 
	/// Load object action data.
	void Read(Core::DataReader *f);
 
 private:
	/// Reference to position graphs.
	AnimationGraph *m_Position[3];
	
	/// Reference to rotation graphs.
	AnimationGraph *m_Rotation[4];
 
	/// Reference to scale graphs.
	AnimationGraph *m_Scale[3];
 
	/// Last valid Keyframe
	unsigned int m_LastKeyFrame;
 };*/

/// Datablock responsible for space transformation manipulation.
class Object: public Datablock
{
public:
    /// Constructors.
    Object(Graph::Device *dev);
    
    /// Destructors.
    ~Object();
    
    /// Update object matrix from position, rotation, scale vectors.
    void Update();
    
    /// Enable object transformation.
    /// @param type Target matrix to apply the transformation.
    void Bind(Graph::MatrixType type = Graph::MATRIX_MODEL);
    
    /// Load object data from file.
    /// @param f Reference to file stream.
    /// @param obj Reference to allocated objects instances.
    /// @param mod_vec Reference to a array of models datablocks.
    /// @param cam_vec Reference to a array of cameras datablocks.
    /// @throws Exception.
    void Read(Core::DataReader *f, std::vector<Object *> * obj_vec,
              std::vector<Datablock*> *mod_vec,
              std::vector<Datablock*> *cam_vec,
              std::vector<Datablock*> *lamp_vec,
              std::vector<Datablock*> *arm_vec);
    
#ifdef NCK_BXON
    /// Load object data from BXON map.
    void Parse(BXON::Map * entry, const std::map<std::string, Object *> & objMap,
               const std::map<std::string, Datablock*> & modMap,
               const std::map<std::string, Datablock*> & camMap,
               const std::map<std::string, Datablock*> & lampMap,
               const std::map<std::string, Datablock*> & armMap);
#endif
    
    /// Get object parent.
    Object *GetParent();
    
    /// Set object parent.
    void SetParent(Object *parent);
    
    /// Get object position.
    inline Math::Vec3 GetPosition() { return m_Local; }
    
    /// Get object rotation.
    inline Math::Quat GetRotation() { return m_Rotation; }
    
    /// Get object scale.
    inline Math::Vec3 GetScale() { return m_Scale; }
    
    /// Set object position.
    inline void SetPosition(const Math::Vec3 &v) { m_Local = v; }
    
    /// Get object rotation.
    inline void SetRotation(const Math::Quat &q) { m_Rotation = q; }
    
    /// Get object scale.
    inline void SetScale(const Math::Vec3 &v) { m_Scale = v; }
    
    /// Get object matrix.
    inline Math::Mat44 GetMatrix() { return m_Matrix; }
    
    /// Set object matrix.
    inline void SetMatrix(const Math::Mat44 &m) { m_Matrix = m; }
    
    /// Set object datablock.
    void SetData(Datablock *data);
    
    /// Get object datablock.
    Datablock * GetData();
    
    /// Return datablock type.
    DatablockType GetType();
    
    /// Set object action.
    //	void SetAction(ObjectAction *act);
    //
    //	/// Set reference for object action.
    //	ObjectAction *GetAction();
    
    /// Reproduce object action.
    void Play(float time);
    
private:
    /// Object data link.
    Datablock *m_Data;
    
    /// Object parent reference.
    Object	*m_Parent;
    
    /// Object location.
    Math::Vec3 m_Local;
    
    /// Object rotation.
    Math::Quat m_Rotation;
    
    /// Object scale.
    Math::Vec3 m_Scale;
    
    /// Object matrix.
    Math::Mat44	m_Matrix;
    
    std::map<std::string,AnimationTrack*> m_Tracks;
    //ObjectAction *m_Action;
};

_SCENE_END

#endif // #ifndef NCK_OBJECT_H
