
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_AutoMaterial.h"

std::vector<Graph::Program*> programs;

Demo_AutoMaterial::Demo_AutoMaterial(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
    scene = NULL;
    program = NULL;
}

Demo_AutoMaterial::~Demo_AutoMaterial(){
    SafeDelete(program);
    SafeDelete(scene);
}

void Demo_AutoMaterial::Load(){
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    scene = new Scene::Compound_Base(dev);
   // scene->Load("model://multi_material_scene.bxon");
    scene->Load("model://house_inner.bxon");
    program = dev->LoadProgram("shader://multi_material_mat.cpp");

    std::vector<Scene::Texture*> textures;
    if (scene->GetAllTextures(&textures) > 0)
    {
        for (int i = 0; i < textures.size(); i++) {
            textures[i]->GetTexture()->SetAnisotropyFilter(8);
        }
    }

    std::vector<Scene::Material*> materials;
    if (scene->GetAllMaterials(&materials) > 0) 
    {
        programs.reserve(materials.size());

        for (int i = 0; i < materials.size(); i++) 
        {
            Scene::Material * mat = materials[i];
            
            bool hasBumpmap = false;
            bool hasTexture = false;
            bool hasLight = !mat->GetFlag(Scene::MATERIAL_SHADELESS);
            int totalTextures = 0;

            std::map<int, Scene::TextureLayer*> layers;
            for (int l = 0; l < 8; l++) {
                Scene::TextureLayer * layer = mat->GetTextureLayer(l);
                if (layer != NULL) {
                    hasTexture = true;
                    if ((layer->GetFactorFlag() & Scene::FACTOR_NORMAL) != 0)
                        hasBumpmap = true;
                    layers.insert(std::pair<int, Scene::TextureLayer*>(l,layer));
                    totalTextures++;
                }
            }

            std::string src = "";
            
            // Fragment shader
            src += "#pragma fragment_shader_glx2\n";
            src += "#include \"core.cpp\"\n";
            if (hasLight) {
                src += "#include \"lights.cpp\"\n";
                if(hasBumpmap)
                    src += "#include \"bumpmap.cpp\"\n";
            }
            
            MapFor(int, Scene::TextureLayer*, layers, i) {
                Graph::Texture * tex = i->second->GetTexture()->GetTexture();
                if (tex->GetType() == Graph::TEXTURE_2D) {
                    src += "uniform sampler2D gphTexture"+Math::IntToString(i->first)+";\n";
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
                "\tfloat alpha = 1.0;\n"
                "\tvec3 N = normalize(v_nor_mv);\n";
                
            if (hasLight) {
                src += "\tlights_compute(v_pos_mv,N,diff,spec);\n";
            }

            if (hasTexture) {
                MapFor(int, Scene::TextureLayer*, layers, i) {
                    Scene::TextureLayer * tl = i->second;
                    Graph::Texture * tex = tl->GetTexture()->GetTexture();
                    std::string tString = "";

                    if (tl->GetMappingObject() != NULL) {
                        // Por enquanto sem uniforms para ligar a transform do Objecto.
                        Scene::Object * obj = tl->GetMappingObject();
                        if ((obj->GetScale() - Math::Vec3(1.0, 1.0, 1.0)).Length() > 0.01) {
                            if (tex->GetType() == Graph::TEXTURE_2D)
                                tString += "* vec2(" + Math::FloatToString(obj->GetScale().GetX(),2) + "," + Math::FloatToString(obj->GetScale().GetY(),2) + ")";
                            else
                                tString += "* vec3(" + Math::FloatToString(obj->GetScale().GetX(),2) + "," + Math::FloatToString(obj->GetScale().GetY(),2) + "," + Math::FloatToString(obj->GetScale().GetZ(),2) + ")";
                        }
                        if ((obj->GetPosition() - Math::Vec3(0.0, 0.0, 0.0)).Length() > 0.001) {
                            if (tex->GetType() == Graph::TEXTURE_2D)
                                tString += "+ vec2(" + Math::FloatToString(obj->GetPosition().GetX(),2) + "," + Math::FloatToString(obj->GetPosition().GetY(),2) + ")";
                            else
                                tString += "+ vec3(" + Math::FloatToString(obj->GetPosition().GetX(),2) + "," + Math::FloatToString(obj->GetPosition().GetY(),2) + "," + Math::FloatToString(obj->GetPosition().GetZ(),2) + ")";
                        }
                   }

                    src += "\t{\n";

                    if (tex->GetType() == Graph::TEXTURE_2D) {
                        src += "\t\tvec4 tex = texture2D(gphTexture" + Math::IntToString(i->first) + ",gl_TexCoord[0].xy" + tString + ");\n";
                    }
                    else if (tex->GetType() == Graph::TEXTURE_3D) {
                        src += "\t\tvec4 tex = texture3D(gphTexture" + Math::IntToString(i->first) + ",gl_TexCoord[0].xyz" + tString + ");\n";
                    }
                    else if (tex->GetType() == Graph::TEXTURE_CUBEMAP) {
                        src += "\t\tvec4 tex = textureCube(gphTexture" + Math::IntToString(i->first) + ",gl_TexCoord[0].xyz" + tString + ");\n";
                    }
                    
                    if ((tl->GetFactorFlag() & Scene::FACTOR_DIFFUSE_COLOR) != 0){
                        src += "\t\tdiff *= mix(diff,tex.xyz," + Math::FloatToString(tl->GetFactorDiffuseColor(),2) + ");\n";
                    }

                    if ((tl->GetFactorFlag() & Scene::FACTOR_NORMAL) != 0) {
                        src += "\t\tdiff *= mix(diff,tex.xyz," + Math::FloatToString(tl->GetFactorNormal(),2) + ");\n";
                    }

                    src += "\t}\n";

                }
            }
            else{
                //src += "\tres = vec4(1.0);\n";
            }

            src += "\tgl_FragColor = vec4(diff+spec+ambiente(N), alpha * gphAlpha);\n"
            "}\n";


            src += "#pragma vertex_shader_glx2\n"
            "#include \"core.cpp\"\n"
            "void main(){\n"
            "\tvec4 P = gl_Vertex;\n"
            "\tvec3 N = gl_Normal;\n"
            "\tvec3 P_MV = vec3(gl_ModelViewMatrix * P);\n"
            "\tvec3 N_MV = normalize(gl_NormalMatrix * N);\n"
            "\tcore_copy_texCoord();\n"
            "\tv_pos_mv = P_MV;\n"
            "\tv_nor_mv = N_MV;\n"
            "\tcore_transform_to_screen(P);\n"
            "}\n";

            Core::DebugLog(src);

            Graph::Program * prog = NULL;
            try {
                prog = dev->LoadProgramFromText(src);
                mat->SetProgram(prog);
                programs.push_back(prog);
            }
            catch (const Core::Exception & e) {
                THROW_EXCEPTION_STACK("Unable to generate shader",e);
            }

           
        }
    }
}

void Demo_AutoMaterial::Render(float dt){
    Scene::Camera * cam = scene->GetCamera("Camera");
    cam->SetAspect(wnd->GetWidth() / (float)wnd->GetHeight());

    time += dt;

    dev->ClearColor(0, 0, 0, 0);
    dev->Clear();
    dev->Viewport(0, 0, wnd->GetWidth(), wnd->GetHeight());

    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();
    cam->Enable(Graph::MATRIX_PROJECTION);

    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();
    Math::Mat44 viewMatrix = cam->GetMatrix();
    viewMatrix = Math::RotateZ(sin(time*0.1)*0.1) * viewMatrix;

    dev->MultMatrix((float*)&viewMatrix);

    //cam->Enable(Graph::MATRIX_VIEW);

    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();

   // Math::Mat44 viewMatrix = cam->GetMatrix();

    dev->Enable(Graph::STATE_DEPTH_TEST);

    std::vector<Scene::Object*> objects;
    scene->GetObjectsWithLayer(&objects, Scene::DATABLOCK_LAMP);
    
    Math::Vec4 lamp_pos[8]; // position, energy
    Math::Vec4 lamp_dir[8]; // normal, type (0 - point, 1 - spot, 2 - sun, 3 -area) 
    Math::Vec4 lamp_color[8]; // rgb, distance
    Math::Vec4 lamp_params[8]; // ????

    for (size_t i = 0; i < objects.size() && i < 8; i++) {
        Scene::Object * obj = objects[i];
        Scene::Lamp * lamp = dynamic_cast<Scene::Lamp*>(obj->GetData());
        
        Math::Vec4 pMV = Math::Vec4(obj->GetPosition(), 1.0) * viewMatrix;
        Math::Vec4 dMV = Math::Vec4(0, 0, 0, 0); 
        
        if(lamp->GetType() == Scene::LAMP_TYPE_SPOT || lamp->GetType() == Scene::LAMP_TYPE_SUN)
            dMV = (Math::Vec4(0, 0, 1, 0) * obj->GetMatrix()) * viewMatrix;

        lamp_pos[i] = Math::Vec4(Math::Vec3(pMV), lamp->GetEnergy());
        lamp_dir[i] = Math::Vec4(Math::Vec3(dMV), (float)lamp->GetLampType());
        lamp_color[i] = Math::Vec4(lamp->GetColor().GetR(), lamp->GetColor().GetG(), lamp->GetColor().GetB(), lamp->GetDistance());
        lamp_params[i] = Math::Vec4();
    }

    if (objects.size() < 8)
        lamp_pos[objects.size()] = Math::Vec4(0, 0, 0, -1); // early terminator.

    for (int i = 0; i < programs.size(); i++) {
        programs[i]->SetArray4f("lamp_pos_mv", 8, (float*)lamp_pos);
        programs[i]->SetArray4f("lamp_dir_mv", 8, (float*)lamp_dir);
        programs[i]->SetArray4f("lamp_color", 8, (float*)lamp_color);
        programs[i]->SetArray4f("lamp_params", 8, (float*)lamp_params);
    }

    Scene::Object *obj = scene->GetObject("Ventoinha");
    obj->SetRotation(Math::EulerToQuat(Math::Vec3(0, 0, time*2)));
    obj->Update();

    scene->Render();
    //program->Disable();

    dev->PresentAll();
}

void Demo_AutoMaterial::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_AutoMaterial::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Basic");
    ret.push_back("Material");
    return ret;
}

std::string Demo_AutoMaterial::GetDescription() {
    return "Materials and Multi-Material";
}

Demo * CreateDemo_AutoMaterial(Core::Window * wnd, Graph::Device * dev){
	return new Demo_AutoMaterial(wnd,dev);
}
