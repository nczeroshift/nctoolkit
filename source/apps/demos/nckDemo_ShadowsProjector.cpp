
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_ShadowsProjector.h"
#include "nckMaterialToProgram.h"
#include "nckRenderPass.h"

#include <map>
#include "bxonDataIO.h"

Demo_ShadowsProjector::Demo_ShadowsProjector(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
    bufferSize = 1024;
    program = NULL;
    scene = NULL;
    time = 0;
    depth = NULL;
    rtDirect = NULL;
	rtShadow = NULL;
	texColor = NULL;
	texPosition = NULL;
	texNormal = NULL;
    display = NULL;
}

Demo_ShadowsProjector::~Demo_ShadowsProjector(){
    SafeDelete(display);
    SafeDelete(scene);
    SafeDelete(program);
    SafeDelete(depth);
    SafeDelete(rtDirect);
    SafeDelete(rtShadow);
}

void Demo_ShadowsProjector::Load(){
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    // 1. Create shadow buffer.
    texShadow = dynamic_cast<Graph::Texture2D*>(
        dev->CreateTexture(Graph::TEXTURE_2D, bufferSize, bufferSize, 0, Graph::FORMAT_R_32F, true)
    );

	texPosition = dynamic_cast<Graph::Texture2D*>(
		dev->CreateTexture(Graph::TEXTURE_2D, wnd->GetWidth(), wnd->GetHeight(), 0, Graph::FORMAT_RGBA_32F, true)
	);	

	texNormal = dynamic_cast<Graph::Texture2D*>(
		dev->CreateTexture(Graph::TEXTURE_2D, wnd->GetWidth(), wnd->GetHeight(), 0, Graph::FORMAT_RGBA_32F, true)
	);

	texColor = dynamic_cast<Graph::Texture2D*>(
		dev->CreateTexture(Graph::TEXTURE_2D, wnd->GetWidth(), wnd->GetHeight(), 0, Graph::FORMAT_RGBA_8B, true)
	);

	texShadow->SetFilter(Graph::FILTER_MINIFICATION, Graph::FILTER_LINEAR);
	texShadow->SetFilter(Graph::FILTER_MAGNIFICATION, Graph::FILTER_LINEAR);
	texShadow->SetAdressMode(Graph::ADRESS_CLAMP);

    // 2. Create render target and assign texture.
    rtShadow = dev->CreateRTManager(bufferSize, bufferSize);
	rtShadow->Attach(0, texShadow);

	rtDirect = dev->CreateRTManager(wnd->GetWidth(), wnd->GetHeight());
	rtDirect->Attach(0, texColor);
	rtDirect->Attach(1, texPosition);
	rtDirect->Attach(2, texNormal);

    // Load scene geometry and other stuff.
    scene = new Scene::Compound_Base(dev);
    scene->Load("model://shadow_spot.bxon");    
	
    // Wrap shadow buffer texture into a scene texture.
    Scene::Texture * shadowTex = new Scene::Texture(dev);
    shadowTex->SetTexture(texShadow);

    // Depth generation for shadow buffer
    Scene::MaterialToProgram shadowMTP(dev);
    shadowMTP.EnableLightDepth(true);
    Scene::Material matDepth(dev);
    depth = shadowMTP.Generate(&matDepth);

	// Define a simple material with support to shadows.
	Scene::Material diffuseSimple(dev);

	// Diffuse material with shadows support.
	Scene::MaterialToProgram diffuseMTP(dev);
	diffuseMTP.EnablePositionOutput(true);
	diffuseMTP.EnableNormalOutput(true);
	program = diffuseMTP.Generate(&diffuseSimple);	

    camera = (Scene::Camera*)scene->GetDatablock(Scene::DATABLOCK_CAMERA, "Camera");
    lamp = (Scene::Lamp*)scene->GetDatablock(Scene::DATABLOCK_LAMP, "Lamp");
	lamp->SetTexture(texShadow);
	
	std::vector<Scene::Material*> materials;
	scene->GetAllMaterials(&materials);
	for (int i = 0; i < materials.size(); i++){
		materials[i]->SetProgram(program);
		materials[i]->SetProgram(depth,Scene::MATERIAL_PASS_SHADOW);
	}


	sampler = dev->LoadProgram("shader://shadow_project.cpp");
}


void Demo_ShadowsProjector::Render(float dt){

	const float width = wnd->GetWidth();
	const float height = wnd->GetHeight();
	const float aspect = width / height;

	camera->SetAspect(aspect);

	const Math::Mat44 camProj = camera->GetMatrix(Graph::MATRIX_PROJECTION);
	const Math::Mat44 camView = camera->GetMatrix(Graph::MATRIX_VIEW);
	const Math::Mat44 world = Math::RotateZ(time);

	dev->ClearColor(0, 0, 0);

	dev->Enable(Graph::STATE_CULL_FACE);
	dev->CullMode(Graph::CULL_BACK);

	// Shadow buffer pass
	if (rtShadow->Enable()) 
	{
		dev->Enable(Graph::STATE_DEPTH_TEST);

		dev->Clear();
		dev->Viewport(0, 0, texShadow->GetWidth(), texShadow->GetHeight());

		RenderPass_RenderFromLamp(dev, scene, lamp);

		rtShadow->Disable();
	}
	
	// Direct rendering and position buffers pass
	if (rtDirect->Enable())
	{
		//dev->Enable(Graph::STATE_CULL_FACE);
		//dev->CullMode(Graph::CULL_BACK);

		dev->Clear();
		dev->Viewport(0, 0, width, height);

		dev->Enable(Graph::STATE_DEPTH_TEST);
		dev->Enable(Graph::STATE_BLEND);

		RenderPass_RenderWithLamps(dev, scene, scene, camProj, camView, world);

		rtDirect->Disable();
	}
	
	
	dev->Clear();
	dev->Viewport(0, 0, width, height);
	
	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();
	dev->Ortho2D(width, height);

	dev->MatrixMode(Graph::MATRIX_VIEW);
	dev->Identity();
         
	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();

	dev->Disable(Graph::STATE_DEPTH_TEST);
	dev->Enable(Graph::STATE_BLEND);
	dev->Disable(Graph::STATE_CULL_FACE);

	RenderPass_CastShadowBuffers(dev, scene, sampler, texColor, texPosition, texNormal, camProj, camView, world, Math::Vec2(width, height));

	RenderSquare2D(4, 4, 8, 8, Math::Color4ub(255,255,255,255*abs(cos(time*100))));
	
	dev->PresentAll();

	time += 0.001;
}

void Demo_ShadowsProjector::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_ShadowsProjector::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Advanced");
    ret.push_back("Rendering");
    ret.push_back("Shadows");
    return ret;
}

std::string Demo_ShadowsProjector::GetDescription() {
    return "Buffer Shadows";
}

Demo * CreateDemo_ShadowsProjector(Core::Window * wnd, Graph::Device * dev){
	return new Demo_ShadowsProjector(wnd,dev);
}

