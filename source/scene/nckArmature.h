
/**
 * NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_ARMATURE_H
#define NCK_ARMATURE_H

#include "nckAnimation.h"
#include "nckDatablock.h"
#include "nckGraphics.h"
#include "nckVec3.h"
#include "nckQuat.h"
#include "bxon.hpp"

#include <vector>

_SCENE_BEGIN

/// Character skelecton bone.
class Bone
{
public:
    /// Constructor.
    Bone();
    
    /// Destructor.
    ~Bone();
    
    /// Bone name.
    std::string m_Name;
    
    /// Bone head position.
    Math::Vec3 m_Head;
    
    /// Bone tail position.
    Math::Vec3 m_Tail;
    
    /// Reference to parent bone.
    Bone *m_Parent;
    
    /// Bone active location.
    Math::Vec3 m_Location;
    
    /// Bone active rotation.
    Math::Quat m_Rotation;
    
    /// Bone active scale.
    Math::Vec3 m_Scale;
    
    /// Bone matrix.
    Math::Mat44 m_Matrix;
    
    /// Bone initial position in armature space.
    Math::Vec3 m_RestPos;
    
    // Blender bone matrix.
    Math::Mat44 m_BMatrix;
    
    // Bone inverse bone matrix.
    Math::Mat44 m_InvBMatrix;
    
    /**
     * Load bone data from file stream.
     * @param f Reference to file handler.
     * @param b_array Reference to an array of bones.
     */
    void Read(Core::DataReader *f, Bone ** b_array);
    
#ifdef NCK_BXON
    void Read(BXON::Map * entry, const std::vector<Bone *> & b_array);
#endif
};

/**
 * Collection of animation strips for each bone.
 */
class ArmatureStrip{
public:
    ArmatureStrip();
    ~ArmatureStrip();
    
    Math::Vec3 GetPosition(const std::string & name, float t);
    Math::Vec3 GetScale(const std::string & name, float t);
    Math::Vec3 GetEuler(const std::string & name, float t);
    Math::Quat GetQuaternion(const std::string & name, float t);
    
    bool HasPosition(const std::string & name);
    bool HasScale(const std::string & name);
    bool HasEuler(const std::string & name);
    bool HasQuaternion(const std::string & name);
    
#ifdef NCK_BXON
    void Read(BXON::Map * entry);
#endif
    
    float GetStart();
    float GetEnd();
    
private:
    float m_Start;
    float m_End;
    AnimationStrip * GetStrip(const std::string & name);
    std::map<std::string, AnimationStrip*> m_BoneStrips;
};

/**
 * Collection of armature animation strips.
 */
class ArmatureTrack{
public:
    ArmatureTrack();
    ~ArmatureTrack();
    
#ifdef NCK_BXON
    void Read(BXON::Map * entry);
#endif
    
    void UpdateBone(Bone * b, float time);
    
    std::string GetName();
    
private:
    std::string m_Name;
    std::vector<ArmatureStrip*> m_Strips;

};

/// Character skelecton class.
class Armature: public Datablock
{
public:
    /// Constructor.
    Armature(Graph::Device *dev);
    
    /// Destructor.
    ~Armature();
    
    /// Get datablock type.
    DatablockType GetType();
    
    /// Read armature data from file.
    /// @param f Reference to file stream.
    void Read(Core::DataReader *f);
    
#ifdef NCK_BXON
    void Read(BXON::Map * entry);
#endif
    
    /// Reproduce action.
    /// @param name Action name.
    /// @param time
    /// @param mixture
    /// @param clear
    ///
    bool Play(std::string name,float time,float mixture = 1.0f, bool clear = true);
    
    /// Update bones matrix.
    void Update();
    
    /// Get bone reference from name.
    Bone * GetBone(const std::string &name);
    
    /// Get copy of bones vector
    std::vector<Bone*> GetBones();
    
    /// Render skelecton structure.
    void Render();
    
private:
    
    /// Armature bones.
    std::vector<Bone*> m_Bones;
    
    /// Armature actions.
    //std::vector<ArmatureAction*> m_Actions;
    
    std::map<std::string, ArmatureTrack*> m_Tracks;
};

_SCENE_END

#endif // #ifndef NCK_ARMATURE_H
