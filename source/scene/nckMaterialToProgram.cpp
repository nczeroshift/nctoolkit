
#include "nckMaterialToProgram.h"
#include "nckMathUtils.h"
#include "nckUtils.h"
#include "nckObject.h"
#include "nckException.h"

_SCENE_BEGIN

MaterialToProgram::MaterialToProgram(Graph::Device * dev) {
    m_Device = dev;
    m_armature_support = false;
    m_sRGB_support = false;
    m_light_depth_generation = false;
    m_Shadows = false;
}

MaterialToProgram::~MaterialToProgram() {

}

std::string MaterialToProgram::generateVSH(Scene::Material * mat) {
    // Vertex shader source generation

    bool hasLight = !mat->GetFlag(Scene::MATERIAL_SHADELESS);
    bool hasBumpmap = false;
    int totalTextures = 0;
    bool hasShadow = false;
    std::vector<std::string> uvReg;
    uvReg.reserve(8);

    std::map<int, Scene::TextureLayer*> layers;
    for (int l = 0; l < 8; l++) {
        Scene::TextureLayer * layer = mat->GetTextureLayer(l);
        if (layer != NULL) {
            if ((layer->GetFactorFlag() & Scene::FACTOR_NORMAL) != 0)
                hasBumpmap = true;
            if ((layer->GetFactorFlag() & Scene::FACTOR_SHADOW) != 0)
                hasShadow = true;
            // Find unique uv registers 
            if (layer->GetLayerName().size() > 0 && 
                std::find(uvReg.begin(), uvReg.end(), layer->GetLayerName()) == uvReg.end())
                    uvReg.push_back(layer->GetLayerName());
        }
    }

    // Note: Light Depth Buffer Shaders
    // The generated shader doesn't need to deal with lighting or texturing.
    if (m_light_depth_generation) {
        hasBumpmap = false;
        hasLight = false;
    }

    std::string src;
    src += "#pragma vertex_shader_glx2\n"
        "#include \"core.cpp\"\n";

    if (hasLight) {
        src += "#include \"lights.cpp\"\n";
        if (hasBumpmap)
            src += "#include \"bumpmap.cpp\"\n";
    }

    if (m_armature_support) {
        if (m_max_armature_bones != 0)
            src += "#define BONES_MAX " + Math::IntToString(m_max_armature_bones) + "\n";
        src += "#include \"armature.cpp\"\n";
    }

    ListFor(std::string, m_vsh_includes, i) {
        src += "#include \"" + (*i) + "\"\n";
    }
    
    if (m_Shadows && hasShadow) {
        src += "#include \"shadows.cpp\"\n";
    }

    // Shadow buffer generation
    if (m_light_depth_generation)
        src += "varying vec4 v_pos_pmv;\n";

    src += "void main(){\n"
        "\tvec4 P = gl_Vertex;\n"
        "\tvec3 N = gl_Normal;\n";

    if (m_armature_support) {
        src += "\tvec3 aP = P.xyz;\n";
        int sRegId = uvReg.size() + (hasBumpmap ? 1 : 0) + 1;
        // Skinning is placed after UV coordinate registers and bumpmapping Tangent vector.
        src += "\tarmature_transform(gl_MultiTexCoord"+Math::IntToString(sRegId)+
            ".xyzw,gl_MultiTexCoord" + Math::IntToString(sRegId+2) + ".xyzw,aP,N);\n";
        src += "\tP = vec4(aP,1.0);\n";
    }

    ListFor(std::string, m_vsh_pre_transform, i) {
        src += "\t" + (*i) + "\n";
    }

    src += "\tvec3 P_MV = vec3(gl_ModelViewMatrix * P);\n"
        "\tvec3 N_MV = normalize(gl_NormalMatrix * N);\n";

    src += "\tv_nor_w = normalize((gphModelMatrix * vec4(N,0.0)).xyz);\n";

    if (hasBumpmap) {
        // Bumpmap needs at least one UV texture, which means it starts on uv register 1
        src += "\tvec4 T = gl_MultiTexCoord"+Math::IntToString(MIN(uvReg.size(),1))+".xyzw;\n"
            "\tvec3 T_MV = normalize(gl_NormalMatrix * T.xyz);\n"
            "\tbumpmap_compute(P_MV,N_MV,vec4(T_MV,T.w));\n";
    }

    src += "\tcore_copy_texCoord();\n"
        "\tv_pos_mv = P_MV;\n"
        "\tv_nor_mv = N_MV;\n";

    // For light depth buffer generation.
    if (m_light_depth_generation)
        src += "\tv_pos_pmv = gl_ModelViewProjectionMatrix * P;\n";

    // Compute depth in light space.
    if (hasShadow && m_Shadows) {
        src += "\tshadow_transform_to_light(P);\n";
    }

    if (m_vsh_post_transform.size() > 0) {
        ListFor(std::string, m_vsh_post_transform, i) {
            src += "\t" + (*i) + "\n";
        }
    }
    else
    {
        src += "\tcore_transform_to_screen(P);\n";
    }  
    
    src += "}\n";

    Graph::Program * prog = NULL;

    return src;
}

std::string MaterialToProgram::generateFSH(Scene::Material * mat) {

    if (m_light_depth_generation) {
        // Note:
        // This will write the distance to the camera in all the 4 channels, 
        // use a float texture with single change to save resources.
        std::string src = "#pragma fragment_shader_glx2\n"
            //"#include \"core.cpp\"\n"
            "varying vec4 v_pos_pmv;\n"
            "void main(void){\n"
            "\tgl_FragColor = vec4(vec3(v_pos_pmv.z / v_pos_pmv.w),1.0);\n"
            "}\n";
        return src;
    }

    bool hasBumpmap = false;
    bool hasTexture = false;
    bool hasLight = !mat->GetFlag(Scene::MATERIAL_SHADELESS);
    bool hasShadow = false;
    int totalTextures = 0;
    std::vector<std::string> uvReg;
    uvReg.reserve(8);

    std::map<int, Scene::TextureLayer*> layers;
    for (int l = 0; l < 8; l++) {
        Scene::TextureLayer * layer = mat->GetTextureLayer(l);
        if (layer != NULL) {
            hasTexture = true;
            if ((layer->GetFactorFlag() & Scene::FACTOR_NORMAL) != 0)
                hasBumpmap = true;
            if ((layer->GetFactorFlag() & Scene::FACTOR_SHADOW) != 0)
                hasShadow = true;

            layers.insert(std::pair<int, Scene::TextureLayer*>(l, layer));
            totalTextures++;

            // Find unique uv registers 
            if (layer->GetLayerName().size() > 0 &&
                std::find(uvReg.begin(), uvReg.end(), layer->GetLayerName()) == uvReg.end())
                uvReg.push_back(layer->GetLayerName());
        }
    }

    std::string src = "";

    // Fragment shader source generation

    src += "#pragma fragment_shader_glx2\n";
    src += "#include \"core.cpp\"\n";
    if (hasLight) {
        src += "#include \"lights.cpp\"\n";
        if (hasBumpmap)
            src += "#include \"bumpmap.cpp\"\n";
    }
    
    if (hasShadow && m_Shadows) {
        src += "#include \"shadows.cpp\"\n";
    }

    ListFor(std::string, m_fsh_includes, i) {
        src += "#include \"" + (*i) + "\"\n";
    }

    MapFor(int, Scene::TextureLayer*, layers, i) {
        Graph::Texture * tex = i->second->GetTexture()->GetTexture();
        if (tex->GetType() == Graph::TEXTURE_2D) {
            src += "uniform sampler2D gphTexture" + Math::IntToString(i->first) + ";\n";
        }
        else if (tex->GetType() == Graph::TEXTURE_3D) {
            src += "uniform sampler3D gphTexture" + Math::IntToString(i->first) + ";\n";
        }
        else if (tex->GetType() == Graph::TEXTURE_CUBEMAP) {
            src += "uniform samplerCube gphTexture" + Math::IntToString(i->first) + ";\n";
        }
    }

    src += "void main(void){\n"
        "\tvec3 diff = vec3(0.0);\n"
        "\tvec3 spec = vec3(0.0);\n"
        "\tfloat alpha = gphAlpha;\n"
        "\tvec3 N = normalize(v_nor_mv);\n"
        "\tvec3 colDiff = gphDiffuseColor.rgb;\n"
        "\tvec3 colSpec = gphSpecularColor.rgb;\n";

    // Use shadows from textures
    if (hasShadow && m_Shadows) {
        src += "\tfloat shadowMask = 1.0;\n";
    }

    ListFor(std::string, m_fsh_pre_transform, i) {
        src += "\t" + (*i) + "\n";
    }

    if (hasTexture) {
        MapFor(int, Scene::TextureLayer*, layers, i) {
            Scene::TextureLayer * tl = i->second;
            Graph::Texture * tex = tl->GetTexture()->GetTexture();
            std::string tString = "";
            
            // by default, it's the first register, unless the layer name 
            // is formated like this: "foobar_2" or "barfoo_1" which 
            // means "uv reg 2" and "uv reg 1" respectively
            std::string uvRegIdStr = "0"; 
          
            std::string layerName = tl->GetLayerName();
            if (layerName.size() > 0 && layerName.find_last_of("_") != std::string::npos) {
                std::string layerRegId = layerName.substr(layerName.find_last_of("_")+1);
                int id = atoi(layerRegId.c_str());
                if (id >= 0 && id <= 7) {
                    uvRegIdStr = layerRegId;
                }
            }

            if (tl->GetMappingObject() != NULL) {
                // Por enquanto sem uniforms para ligar a transform do Objecto.
                Scene::Object * obj = tl->GetMappingObject();
                if ((obj->GetScale() - Math::Vec3(1.0, 1.0, 1.0)).Length() > 0.01) {
                    if (tex->GetType() == Graph::TEXTURE_2D)
                        tString += "* vec2(" + Math::FloatToString(obj->GetScale().GetX(), 2) + "," + Math::FloatToString(obj->GetScale().GetY(), 2) + ")";
                    else
                        tString += "* vec3(" + Math::FloatToString(obj->GetScale().GetX(), 2) + "," + Math::FloatToString(obj->GetScale().GetY(), 2) + "," + Math::FloatToString(obj->GetScale().GetZ(), 2) + ")";
                }
                if ((obj->GetPosition() - Math::Vec3(0.0, 0.0, 0.0)).Length() > 0.001) {
                    if (tex->GetType() == Graph::TEXTURE_2D)
                        tString += "+ vec2(" + Math::FloatToString(obj->GetPosition().GetX(), 2) + "," + Math::FloatToString(obj->GetPosition().GetY(), 2) + ")";
                    else
                        tString += "+ vec3(" + Math::FloatToString(obj->GetPosition().GetX(), 2) + "," + Math::FloatToString(obj->GetPosition().GetY(), 2) + "," + Math::FloatToString(obj->GetPosition().GetZ(), 2) + ")";
                }
            }

            // Use textures for shadows.
            if (m_Shadows && (tl->GetFactorFlag() & FACTOR_SHADOW) != 0) 
            {
                src += "\tshadowMask *= shadow_cast(gphTexture" + Math::IntToString(i->first) + ");\n";
                continue;
            }
            
            // Use textures to diffuse, spec and bump.
            src += "\t{\n";
                        
            if (tex->GetType() == Graph::TEXTURE_2D) {
                src += "\t\tvec4 tex = texture2D(gphTexture" + Math::IntToString(i->first) + ",gl_TexCoord["+ uvRegIdStr +"].xy" + tString + ");\n";
            }
            else if (tex->GetType() == Graph::TEXTURE_3D) {
                src += "\t\tvec4 tex = texture3D(gphTexture" + Math::IntToString(i->first) + ",gl_TexCoord["+ uvRegIdStr +"].xyz" + tString + ");\n";
            }
            else if (tex->GetType() == Graph::TEXTURE_CUBEMAP) {
                src += "\t\tvec4 tex = textureCube(gphTexture" + Math::IntToString(i->first) + ",gl_TexCoord["+ uvRegIdStr +"].xyz" + tString + ");\n";
            }

            if ((tl->GetFactorFlag() & Scene::FACTOR_DIFFUSE_COLOR) != 0) {
                if (tl->GetBlendMode() == TEX_BLEND_MODE_MULTIPLY)
                    src += "\t\tcolDiff = colDiff * mix(vec3(1.0),tex.xyz,"+ Math::FloatToString(tl->GetFactorDiffuseColor(), 2) + ");\n";
                else
                    src += "\t\tcolDiff = mix(colDiff,tex.xyz," + Math::FloatToString(tl->GetFactorDiffuseColor(), 2) + ");\n";
            }

            if ((tl->GetFactorFlag() & Scene::FACTOR_SPECULAR_COLOR) != 0) {
                src += "\t\tcolSpec = mix(colSpec,tex.xyz," + Math::FloatToString(tl->GetFactorSpecularColor(), 2) + ");\n";
            }

            if ((tl->GetFactorFlag() & Scene::FACTOR_ALPHA) != 0) {
                src += "\t\talpha = mix(alpha,tex.a," + Math::FloatToString(tl->GetFactorAlpha(), 2) + ");\n";
            }

            if ((tl->GetFactorFlag() & Scene::FACTOR_NORMAL) != 0) {
                src += "\t\tN = bumpmap_mix_sample(N,tex.xyz," + Math::FloatToString(tl->GetFactorNormal(), 2) + ");\n";
            }

            src += "\t}\n";
        }
    }

    if (hasLight) {
        src += "\tlights_compute(v_pos_mv,N,diff,spec);\n";
    }

    ListFor(std::string, m_fsh_post_transform, i) {
        src += "\t" + (*i) + "\n";
    }

    if (hasLight)
    {
        if (m_Shadows && hasShadow)
            src += "\tgl_FragColor = vec4(shadowMask * (diff * colDiff + spec * colSpec), alpha);\n";
        else
            src += "\tgl_FragColor = vec4(diff * colDiff + spec * colSpec, alpha);\n";
    }
    else 
        src += "\tgl_FragColor = vec4(colDiff, alpha);\n";

    src += "}\n";

    return src;
}

void MaterialToProgram::EnableArmature(bool flag, int max_bones) {
    m_armature_support = flag;
    m_max_armature_bones = max_bones;
}

void MaterialToProgram::EnablesRGB(bool flag) { 
    m_sRGB_support = flag;
}

void MaterialToProgram::EnableLightDepth(bool flag) {
    m_light_depth_generation = flag;
}

void MaterialToProgram::EnableShadows(bool flag) {
    m_Shadows = flag;
}

Graph::Program * MaterialToProgram::Generate(Scene::Material * mat) {
    std::string src = generateVSH(mat);
    src += generateFSH(mat);
    Graph::Program * prog  = NULL;

    try {
        prog = m_Device->LoadProgramFromText(src);
    }
    catch (const Core::Exception & e) {
        Core::Exception retEx("Unable to generate shader from material \""+mat->GetName()+"\"", __FUNCTION__, __FILE__, __LINE__, e);
        retEx.SetMetadata(src);
        throw retEx;
    }

    return prog;
}

_SCENE_END