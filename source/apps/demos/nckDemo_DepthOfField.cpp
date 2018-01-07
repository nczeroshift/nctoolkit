
/**
* NCtoolKit © 2007-2018 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_DepthOfField.h"
#include "nckMaterialToProgram.h"

Demo_DepthOfField::Demo_DepthOfField(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
    model = NULL;
    tex1 = NULL;
    tex2 = NULL;
    mng = NULL;
    prog = NULL;
    shape = NULL;
}

Demo_DepthOfField::~Demo_DepthOfField(){
    SafeDelete(tex1);
    SafeDelete(tex2);
    SafeDelete(mng);
    SafeDelete(prog);
    SafeDelete(shape);
}

void Demo_DepthOfField::Load(){
    // Enable z test
    dev->Enable(Graph::STATE_DEPTH_TEST);

    // Set correct states for alpha blending.
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    width = wnd->GetWidth();
    height = wnd->GetHeight();

    mng = dev->CreateRTManager(width, height);
    tex1 = dynamic_cast<Graph::Texture2D*>(dev->CreateTexture(Graph::TEXTURE_2D, width, height, 0, Graph::FORMAT_RGBA_8B, true));
    tex2 = dynamic_cast<Graph::Texture2D*>(dev->CreateTexture(Graph::TEXTURE_2D, width, height, 0, Graph::FORMAT_RGBA_32F, true));

    tex1->SetAdressMode(Graph::ADRESS_CLAMP);
    tex2->SetAdressMode(Graph::ADRESS_CLAMP);

    mng->Attach(0, tex1);
    mng->Attach(1, tex2);

    model = new Scene::Compound_Base(dev);
    model->Load("model://depth_of_field.bxon");

    prog = dev->LoadProgram("shader://depth_of_field.cpp");
    shape = new Gui::ShapeRenderer(dev);

    std::vector<Scene::Material*> mats;
    model->GetAllMaterials(&mats);

    Scene::MaterialToProgram mtp(dev);
    mtp.EnablePositionOutput(true);

    for (int i = 0; i < mats.size(); i++) {
        Scene::Material * mat = mats[i];
        mat->SetProgram(mtp.Generate(mat));
    }
}

void Demo_DepthOfField::Render(float dt){
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
        dev->Viewport(0, 0, tex1->GetWidth(), tex1->GetHeight());
        dev->Clear();

        dev->MatrixMode(Graph::MATRIX_PROJECTION);
        dev->Identity();
            
        cam->SetAspect(tex1->GetWidth()/(float)tex1->GetHeight());
        cam->Enable(Graph::MATRIX_PROJECTION);
        
        dev->MatrixMode(Graph::MATRIX_VIEW);
        dev->Identity();
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
   
    //tex1->Enable();
   // shape->Square(0, 0, width, height);
   // RenderSquare2D(0, 0, 256, 256);
    //tex1->Disable();
    tex1->Enable(0);
    tex2->Enable(1);

    prog->Enable();
 
    dev->PushMatrix();
    dev->Scale(1.0, -1.0, 0.0);
    dev->Translate(0,- height, 0);
    RenderSquare2D(0, 0, width, height);
    dev->PopMatrix();
  
    prog->Disable();

    tex2->Disable(1);
    tex1->Disable(0);
 
    time += dt;

	// Finish rendering and present the graphics.
	dev->PresentAll();
}

void Demo_DepthOfField::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_DepthOfField::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Advanced");
    ret.push_back("Rendering");
    ret.push_back("Depth of Field");
    return ret;
}

std::string Demo_DepthOfField::GetDescription() {
    return "Depth of Field";
}

Demo * CreateDemo_DepthOfField(Core::Window * wnd, Graph::Device * dev){
	return new Demo_DepthOfField(wnd,dev);
}
