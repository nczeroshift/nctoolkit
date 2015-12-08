
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 5 - Marching Cubes
*/

#include "nckDemo_5.h"

Demo5::Demo5(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;

	chron = Core::CreateChronometer();

	fontMap = NULL;
	fontTexture = NULL;
	shader = NULL;
	processor = NULL;

	dimensions = 6;
	currentLod = 20;  
	maxLod = 40;
	zRotation = 0;

	spheres.push_back(Scene::MCSphereShape());
	spheres.push_back(Scene::MCSphereShape());
	spheres.push_back(Scene::MCSphereShape());
	spheres.push_back(Scene::MCSphereShape());
}

Demo5::~Demo5(){
	SafeDelete(fontMap);
	SafeDelete(fontTexture);
	SafeDelete(shader);
	SafeDelete(processor);
	SafeDelete(chron);
}

void Demo5::Load(){
	dev->Enable(Graph::STATE_BLEND);
	dev->Disable(Graph::STATE_DEPTH_TEST);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA,Graph::BLEND_INV_SRC_ALPHA);

	shader = dev->LoadProgram("shader://marching_cubes.cpp");
	processor = new Scene::MCRenderer(dev,maxLod,currentLod,dimensions);

	fontTexture = dev->LoadTexture("texture://tex2d_font_ubuntu.png");

	fontMap = new Gui::FontMap(dev);
	fontMap->Load("script://font_ubuntu.txt");
}

void Demo5::Render(float dt){
	dev->Clear();
	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

	dev->Enable(Graph::STATE_DEPTH_TEST);
			
	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();
	dev->Perspective(wnd->GetWidth()/(float)wnd->GetHeight());
			
	dev->MatrixMode(Graph::MATRIX_VIEW);
	dev->Identity();

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();

	dev->Rotate(63,-1,0,0);
	dev->Rotate(0.62,0,-1,0);
	dev->Rotate(46,0,0,-1);
	dev->Translate(-7.48,6.5,-5.3);
		
	dev->Rotate(zRotation,0,0,1);

	processor->Clear();

	if(processor->GetDetail()!=currentLod)
		processor->SetDetail(currentLod);

	spheres[0] = Scene::MCSphereShape(Math::Vec3(sin(zRotation+3.14),cos(zRotation)*2,-cos(zRotation)),0.5);
	spheres[1] = Scene::MCSphereShape(Math::Vec3(cos(zRotation),sin(zRotation)*2,cos(zRotation)),0.5);
	spheres[2] = Scene::MCSphereShape(Math::Vec3(cos(zRotation)*2.1,sin(zRotation)*1.2,cos(zRotation+2.24)*1.4),0.5);
	spheres[3] = Scene::MCSphereShape(Math::Vec3(0,0,0),1);
		
	processor->ApplySpheres(spheres);

	processor->Update();	

	//dev->FillMode(Graph::FILL_WIREFRAME);
	shader->Enable();
	processor->Render();
	shader->Disable();
	
	//RenderCube(dev,dimensions);			
				
	dev->Disable(Graph::STATE_DEPTH_TEST);

	dev->Enable(Graph::STATE_BLEND);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA,Graph::BLEND_INV_SRC_ALPHA);

	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();
	dev->Ortho2D(wnd->GetWidth(),wnd->GetHeight());
			
	dev->MatrixMode(Graph::MATRIX_VIEW);
	dev->Identity();

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();

	chron->Stop();

	float fps = chron->GetElapsedTime()/1000.0;
	if(fps!=0.0)
		fps = 1.0/fps;
	else 
		fps = 1.0;

	chron->Start();

	dev->Color(0,0,0);

	fontTexture->Enable();
	fontMap->Draw(10,20,16,std::string("Triangles: ")+Math::IntToString(processor->GetTrianglesCount()),false);
	fontMap->Draw(10,44,16,std::string("Vertices: ")+Math::IntToString(processor->GetVerticesCount()),false);
	fontMap->Draw(10,68,16,std::string("Lod: ")+Math::IntToString(processor->GetDetail()) + " Max Lod: " + Math::IntToString(processor->GetMaxDetail()));				
	fontMap->Draw(10,92,16,std::string("FPS: ")+Math::FloatToString(fps,2));							
	fontTexture->Disable();

	dev->PresentAll();

	zRotation += 0.01;
}

void Demo5::UpdateWndEvents(){
	static bool statPlusBtn = false;
	static bool statMinusBtn = false;

	if(wnd->GetKeyStatus(Core::BUTTON_UP) == Core::BUTTON_STATUS_DOWN){
		if(!statPlusBtn)
			currentLod = processor->GetDetail()+1;
		statPlusBtn = true;
	}else
		statPlusBtn = false;

	if(wnd->GetKeyStatus(Core::BUTTON_DOWN) == Core::BUTTON_STATUS_DOWN){
		if(!statMinusBtn)
			currentLod = processor->GetDetail()-1;
		statMinusBtn = true;
	}else
		statMinusBtn = false;
}


Demo * CreateDemo_5(Core::Window * wnd, Graph::Device * dev){
	return new Demo5(wnd,dev);
}
