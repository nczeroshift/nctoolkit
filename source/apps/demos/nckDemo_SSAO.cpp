
/**
* NCtoolKit © 2007-2018 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_SSAO.h"
#include "nckMaterialToProgram.h"

Demo_SSAO::Demo_SSAO(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
    model = NULL;
    tex_direct = NULL;
    tex_position = NULL;
    tex_normal = NULL;
    mng = NULL;
    prog = NULL;
    shape = NULL;
}

Demo_SSAO::~Demo_SSAO(){
    SafeDelete(tex_direct);
    SafeDelete(tex_position);
    SafeDelete(tex_normal);
    SafeDelete(model);
    SafeDelete(mng);
    SafeDelete(prog);
    SafeDelete(shape);
}

void Demo_SSAO::Load(){
    // Enable z test
    dev->Enable(Graph::STATE_DEPTH_TEST);

    // Set correct states for alpha blending.
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    width = wnd->GetWidth();
    height = wnd->GetHeight();

    mng = dev->CreateRTManager(width, height);
    tex_direct = dynamic_cast<Graph::Texture2D*>(dev->CreateTexture(Graph::TEXTURE_2D, width, height, 0, Graph::FORMAT_RGBA_8B, true));
    tex_position = dynamic_cast<Graph::Texture2D*>(dev->CreateTexture(Graph::TEXTURE_2D, width, height, 0, Graph::FORMAT_RGBA_32F, true));
    tex_normal = dynamic_cast<Graph::Texture2D*>(dev->CreateTexture(Graph::TEXTURE_2D, width, height, 0, Graph::FORMAT_RGBA_32F, true));

    tex_direct->SetAdressMode(Graph::ADRESS_CLAMP);
    tex_position->SetAdressMode(Graph::ADRESS_CLAMP);
    tex_normal->SetAdressMode(Graph::ADRESS_CLAMP);

    mng->Attach(0, tex_direct);
    mng->Attach(1, tex_position);
    mng->Attach(2, tex_normal);

    model = new Scene::Compound_Base(dev);
    model->Load("model://ssao.bxon");

    prog = dev->LoadProgram("shader://ssao.cpp");
    shape = new Gui::ShapeRenderer(dev);

    std::vector<Scene::Material*> mats;
    model->GetAllMaterials(&mats);

    Scene::MaterialToProgram mtp(dev);
    mtp.EnablePositionOutput(true);
    mtp.EnableNormalOutput(true);

    for (int i = 0; i < mats.size(); i++) {
        Scene::Material * mat = mats[i];
        mat->SetProgram(mtp.Generate(mat));
    }
}

void Demo_SSAO::Render(float dt){
    Scene::Camera * cam = model->GetCamera("Camera");
    dev->ClearColor(0, 0, 0);

    std::vector<Scene::Object*> lampObjs;
    model->GetObjectsWithLayer(&lampObjs, Scene::DATABLOCK_LAMP);
    Scene::LampUniforms lUniforms;
    Scene::Lamp::GenerateUniforms(lampObjs, cam->GetMatrix(), &lUniforms);
    std::vector<Scene::Material*> mats;
    model->GetAllMaterials(&mats);
    for (int i = 0; i < mats.size(); i++) {
        Scene::Material * mat = mats[i];
        lUniforms.Bind(mat->GetProgram());
    }

    mng->Enable();
    {
        dev->Viewport(0, 0, tex_direct->GetWidth(), tex_direct->GetHeight());
        dev->Clear();

        dev->MatrixMode(Graph::MATRIX_PROJECTION);
        dev->Identity();
            
        cam->SetAspect(tex_direct->GetWidth()/(float)tex_direct->GetHeight());
        cam->Enable(Graph::MATRIX_PROJECTION);
        
        dev->MatrixMode(Graph::MATRIX_VIEW);
        dev->Identity();
		dev->Rotate(sin(time) * 2, 0, 1.0, 0);
        cam->Enable(Graph::MATRIX_VIEW);
		

        dev->MatrixMode(Graph::MATRIX_MODEL);
        dev->Identity();

        model->Render();
    }
    mng->Disable();
  

	// Clear graphics rendering buffer.
	dev->Clear();

	// Define the rendering area in window space.
	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

	// Set projection matrix.
	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();

    dev->Ortho2D(wnd->GetWidth(),wnd->GetHeight());
    
    // Set the view (camera) matrix.
    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();
   
	// Set the model matrix.
	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();
   
    tex_direct->Enable(0);
    tex_position->Enable(1);
    tex_normal->Enable(2);

    prog->Enable();
 
    dev->PushMatrix();
    dev->Scale(1.0, -1.0, 0.0);
    dev->Translate(0,- height, 0);
    RenderSquare2D(0, 0, width, height);
    dev->PopMatrix();
  
    prog->Disable();

    tex_normal->Disable(0);
    tex_position->Disable(1);
    tex_direct->Disable(0);
 
    time += dt;

	// Finish rendering and present the graphics.
	dev->PresentAll();
}

void Demo_SSAO::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_SSAO::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Advanced");
    ret.push_back("Rendering");
    ret.push_back("SSAO");
    return ret;
}

std::string Demo_SSAO::GetDescription() {
    return "Screen Space Ambient Occlusion";
}

Demo * CreateDemo_SSAO(Core::Window * wnd, Graph::Device * dev){
	return new Demo_SSAO(wnd,dev);
}
