
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_Shadows.h"
#include "nckMaterialToProgram.h"

#include <map>
#include "bxonDataIO.h"

Demo_Shadows::Demo_Shadows(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
    bufferSize = 1024;
    program = NULL;
    scene = NULL;
    time = 0;
    depth = NULL;
    rtManager = NULL;
    rtTexture = NULL;
    display = NULL;
}

Demo_Shadows::~Demo_Shadows(){
    SafeDelete(display);
    SafeDelete(scene);
    SafeDelete(program);
    SafeDelete(depth);
    SafeDelete(rtManager);
    SafeDelete(rtTexture);
}

void Demo_Shadows::Load(){
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    // 1. Create shadow buffer.
    rtTexture = dynamic_cast<Graph::Texture2D*>(
        dev->CreateTexture(Graph::TEXTURE_2D, bufferSize, bufferSize, 0, Graph::FORMAT_RGBA_32F, true)
        );

    rtTexture->SetFilter(Graph::FILTER_MINIFICATION, Graph::FILTER_NEAREST);
    rtTexture->SetFilter(Graph::FILTER_MAGNIFICATION, Graph::FILTER_NEAREST);
    rtTexture->SetAdressMode(Graph::ADRESS_CLAMP);

    // 2. Create render target and assign texture.
    rtManager = dev->CreateRTManager(bufferSize, bufferSize);
    rtManager->Attach(0, rtTexture);


    // Load scene geometry and other stuff.
    scene = new Scene::Compound_Base(dev);
    scene->Load("model://buffer_shadows_scene.bxon");    


    // 3. Prepare generic material that accepts shadows.
    
    // Wrap shadow buffer texture into a scene texture.
    Scene::Texture * shadowTex = new Scene::Texture(dev);
    shadowTex->SetTexture(rtTexture);
    
    // Assign texture to a layer with shadow properties.
    Scene::TextureLayer * shadowTL = new Scene::TextureLayer(dev);
    shadowTL->SetTexture(shadowTex);
    shadowTL->SetShadowFactor(1.0);
    shadowTL->SetFactorFlag(Scene::FACTOR_SHADOW);

    // Define a simple material with support to shadows.
    Scene::Material diffuseSimple(dev);
    diffuseSimple.SetTextureLayer(0, shadowTL);
    diffuseSimple.SetUseShadows(true);
    
    // 4. Generate necessary materials.

    // Diffuse material with shadows support.
    Scene::MaterialToProgram diffuseMTP(dev);
    diffuseMTP.EnableShadows(true);
    program = diffuseMTP.Generate(&diffuseSimple);
    
    // Depth generation for shadow buffer
    Scene::MaterialToProgram shadowMTP(dev);
    shadowMTP.EnableLightDepth(true);
    Scene::Material matDepth(dev);
    depth = shadowMTP.Generate(&matDepth);

    camera = (Scene::Camera*)scene->GetDatablock(Scene::DATABLOCK_CAMERA, "Camera");
    lamp = (Scene::Lamp*)scene->GetDatablock(Scene::DATABLOCK_LAMP, "Lamp");

    display = dev->LoadProgram("shader://shadow_depth_display.cpp");
}

void Demo_Shadows::Render(float dt){
    CaptureLightBuffer();

    std::vector<Scene::Object*> lampObjs;
    scene->GetObjectsWithLayer(&lampObjs, Scene::DATABLOCK_LAMP);

    dev->ClearColor(0, 0, 0, 0);
    dev->Clear();
    dev->Viewport(0, 0, wnd->GetWidth(), wnd->GetHeight());

    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();

    camera->SetAspect(wnd->GetWidth() / (float)wnd->GetHeight());
    camera->Enable(Graph::MATRIX_PROJECTION);

    // View Matrix Mode
    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();

    // Set camera properties as view matrix (rotation & position).
    Math::Mat44 modelView = camera->GetMatrix();
    modelView *= Math::RotateZ(time );
    dev->LoadMatrix((float*)&modelView);

    // Model Matrix Mode            
    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();
 
    dev->Enable(Graph::STATE_DEPTH_TEST);

    Scene::LampUniforms lu;
    Scene::Lamp::GenerateUniforms(lampObjs, modelView, &lu);
    lu.Bind(program);

    float bias = 0.0005;

    program->Enable();

    // Set lamp projection model view matrix
    program->SetMatrix("shadow_pmv", (float*)&lampProjViewMat); 
    
    // Set shadow sampling properties.
    program->SetVariable4f("shadow_params", bufferSize, bufferSize, bias, 0);

    // Set shadow texture sampler
    program->SetVariable1i("gphTexture0", 0);

    rtTexture->Enable();
    scene->Render();
    rtTexture->Disable();
    
    program->Disable();
    
    for (int i = 0; i < lampObjs.size(); i++) {
        Scene::Object * obj = lampObjs[i];
        Math::Vec3 lampPos = obj->GetPosition();

        dev->Begin(Graph::PRIMITIVE_LINES);
        dev->Color(255, 255, 0);
        dev->Vertex(lampPos.GetX(), lampPos.GetY(), lampPos.GetZ()-0.5);
        dev->Vertex(lampPos.GetX(), lampPos.GetY(), lampPos.GetZ()+0.5);
        dev->Vertex(lampPos.GetX(), lampPos.GetY()-0.5, lampPos.GetZ());
        dev->Vertex(lampPos.GetX(), lampPos.GetY()+0.5, lampPos.GetZ());
        dev->Vertex(lampPos.GetX()-0.5, lampPos.GetY(), lampPos.GetZ());
        dev->Vertex(lampPos.GetX()+0.5, lampPos.GetY(), lampPos.GetZ());
        dev->End();
    }

    dev->Disable(Graph::STATE_DEPTH_TEST);
    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();
    dev->Ortho2D(wnd->GetWidth(), wnd->GetHeight());

    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();

    display->Enable();
    rtTexture->Enable();
    RenderSquare2D(0, 0, 128, 128, Math::Color4ub());
    rtTexture->Disable();
    display->Disable();

    dev->PresentAll();

    time += 0.01;
}


void Demo_Shadows::RenderFromLight(float width, float height) {
    float zNear = 0.2;
    float zFar = 150.0;

    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Disable(Graph::STATE_CULL_FACE);
    dev->Disable(Graph::STATE_BLEND); // Avoid alpha issues.

    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();
    Math::Mat44 matLampProj = Math::Perspective(width / height, 90, zNear, zFar);
    dev->MultMatrix((float*)&matLampProj);


    // Set the model matrix.
    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();
    Scene::Camera lampCamp(dev);
    lampCamp.SetObject(lamp->GetObject());
    lampViewMat = lampCamp.GetMatrix();
    dev->MultMatrix((float*)&lampViewMat);

    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();

    lampProjViewMat = lampViewMat * matLampProj;

    depth->Enable();
    scene->Render();
    depth->Disable();
    dev->Enable(Graph::STATE_BLEND);
}

void Demo_Shadows::CaptureLightBuffer() {
    rtManager->Enable();
    dev->ClearColor(255, 255, 255, 255);
    dev->Clear();
    dev->Viewport(0, 0, rtTexture->GetWidth(), rtTexture->GetHeight());
    RenderFromLight(rtTexture->GetWidth(), rtTexture->GetHeight());
    rtManager->Disable();
}


void Demo_Shadows::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_Shadows::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Advanced");
    ret.push_back("Rendering");
    ret.push_back("Shadows");
    return ret;
}

std::string Demo_Shadows::GetDescription() {
    return "Buffer Shadows";
}

Demo * CreateDemo_Shadows(Core::Window * wnd, Graph::Device * dev){
	return new Demo_Shadows(wnd,dev);
}
