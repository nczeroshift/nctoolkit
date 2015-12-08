
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_ANIMATION_H
#define NCK_ANIMATION_H

#include "nckSceneConfig.h"
#include "nckMat44.h"
#include "nckDataIO.h"
#include <vector>
#include <map>
#include <stdio.h>

#ifdef NCK_BXON
#include "bxon.hpp"
#endif

_SCENE_BEGIN

/// Animation graphs types.
enum AnimationType
{
    ANIMATION_GRAPH_LOC     = 1,
    ANIMATION_GRAPH_ROT     = 4,
    ANIMATION_GRAPH_SCALE   = 8,
    ANIMATION_GRAPH_COLOR   = 16,
    
    ANIMATION_GRAPH_RESERVED  = 32
};

/// Animation channel types.
enum AnimationChannel{
    ANIMATION_CHANNEL_LOC_X = 1,
    ANIMATION_CHANNEL_LOC_Y = 2,
    ANIMATION_CHANNEL_LOC_Z = 3,
    
    ANIMATION_CHANNEL_ROT_X = 4,
    ANIMATION_CHANNEL_ROT_Y = 5,
    ANIMATION_CHANNEL_ROT_Z = 6,
    ANIMATION_CHANNEL_ROT_W = 7,
    
    ANIMATION_CHANNEL_SCALE_X = 8,
    ANIMATION_CHANNEL_SCALE_Y = 9,
    ANIMATION_CHANNEL_SCALE_Z = 10,
    
    ANIMATION_CHANNEL_COLOR_R = 16,
    ANIMATION_CHANNEL_COLOR_G = 17,
    ANIMATION_CHANNEL_COLOR_B = 18,
    ANIMATION_CHANNEL_COLOR_A = 19,
    
    ANIMATION_CHANNEL_RESERVED  = 32
};

/// Bezier animation node.
class AnimationNode
{
public:
    /// Constructor.
    AnimationNode();
    
    /// Previous anchor time.
    float m_P_Time;
    
    /// Previous anchor value.
    float m_P_Value;
    
    /// Center anchor time.
    float m_C_Time;
    
    /// Center anchor value
    float m_C_Value;
    
    /// Next anchor time
    float m_N_Time;
    
    /// Next anchor value
    float m_N_Value;
    
    /// Read node data from file.
    /// @param f Reference to current file stream.
    void Read(Core::DataReader *f);
    
#ifdef NCK_BXON
    void Read(BXON::Array * array, int i);
#endif
};

/// Animation graph.
class AnimationGraph
{
public:
    /// Constructor
    AnimationGraph();
    
    /// Destructor
    ~AnimationGraph();
    
    /// Get animation graph value for a given instant.
    float GetValue(float time);
    
    /// Read animation graph data from file.
    /// @param f Reference to file stream.
    /// @throws Exception if the blending matrix isn't invertible.
    void Read(Core::DataReader * f);
    
    /// Set animation type.
    void SetChannel(AnimationChannel channel);
    
    /// Set animation type.
    AnimationChannel GetChannel() const;
    
#ifdef NCK_BXON
    void Read(BXON::Array * entry);
#endif
    
private:
    
    void ComputeBlendMatrix();
    
    /// Animation graph type.
    AnimationChannel m_Channel;
    
    /// Graph nodes.
    std::vector<AnimationNode> m_Nodes;
    
    /// Bezier blend matrix.
    std::vector<Math::Mat44> m_BlendMatrix;
};

class AnimationStrip{
public:
    AnimationStrip();
    ~AnimationStrip();
    
    float GetStart();
    float GetEnd();
    int   GetRepeat();
    
    Math::Vec3 GetPosition(float t);
    Math::Vec3 GetScale(float t);
    Math::Vec3 GetEuler(float t);
    Math::Quat GetQuaternion(float t);
    
    bool HasPosition();
    bool HasScale();
    bool HasEuler();
    bool HasQuaternion();
    
#ifdef NCK_BXON
    void Read(BXON::Map * entry);
#endif
    
private:
    float m_Start;
    float m_End;
    int m_Repeat;
    uint32_t m_ChannelsMask;
    std::vector<AnimationGraph*> m_Graphs;
};

class AnimationTrack{
public:
    AnimationTrack();
    ~AnimationTrack();
    
#ifdef NCK_BXON
    void Read(BXON::Map * entry);
#endif
    
    Math::Vec3 GetPosition(float t);
    Math::Vec3 GetScale(float t);
    Math::Vec3 GetEuler(float t);
    Math::Quat GetQuaternion(float t);
    
    bool HasPosition(float t);
    bool HasScale(float t);
    bool HasEuler(float t);
    bool HasQuaternion(float t);
    
    std::string GetName();
private:
    std::string m_Name;
    std::vector<AnimationStrip*> m_Strips;
};

_SCENE_END

#endif // #ifndef NCK_ANIMATION_H
