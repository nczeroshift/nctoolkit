
#ifndef NCK_MATERIAL_TO_PROGRAM_H
#define NCK_MATERIAL_TO_PROGRAM_H

#include "nckGraphics.h"
#include "nckMaterial.h"

_SCENE_BEGIN

class MaterialToProgram {
public:
    MaterialToProgram(Graph::Device * dev);
    ~MaterialToProgram();
    Graph::Program * Generate(Scene::Material * mat);

    void AddIncludeToVSH(const std::string & name) { m_vsh_includes.push_back(name); };
    void AddIncludeToFSH(const std::string & name) { m_fsh_includes.push_back(name); };
    void AddPreTransformVSH(const std::string & name) { m_vsh_pre_transform.push_back(name); };
    void AddPostTransformVSH(const std::string & name) { m_vsh_post_transform.push_back(name); };
    void AddPreTransformFSH(const std::string & name) { m_fsh_pre_transform.push_back(name); };
    void AddPostTransformFSH(const std::string & name) { m_fsh_post_transform.push_back(name); };
    
    // Enable source generation for armature support.
    // @param flag Enable skinning suppoort
    // @param max_bones Maximum bones definition, 0 is for auto (32 bones)
    void EnableArmature(bool flag, int max_bones = 0);

    // Enable source generation for sRGB color output.
    void EnablesRGB(bool flag);

    // Enable depth buffer generation for lights shadow buffers.
    void EnableLightDepth(bool flag);

    void EnawbleShadows(bool flag);
private:
    bool m_Shadows;
    bool m_sRGB_support;
    bool m_armature_support;
    int m_max_armature_bones;
    bool m_light_depth_generation;
    std::string generateVSH(Scene::Material * mat);
    std::string generateFSH(Scene::Material * mat);
    std::list<std::string> m_fsh_pre_transform;
    std::list<std::string> m_fsh_post_transform;
    std::list<std::string> m_vsh_pre_transform;
    std::list<std::string> m_vsh_post_transform;
    std::list<std::string> m_fsh_includes;
    std::list<std::string> m_vsh_includes;
    Graph::Device * m_Device;
};

_SCENE_END

#endif