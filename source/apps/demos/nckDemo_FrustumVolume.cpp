
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_FrustumVolume.h"

Demo_FrustumVolume::Demo_FrustumVolume(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    models = NULL;
    fontTexture = NULL;
    fontMap = NULL;
    time = 0;
}

Demo_FrustumVolume::~Demo_FrustumVolume(){
    SafeDelete(models);
}

void Demo_FrustumVolume::Load(){
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    models = new Scene::Compound_Base(dev);
    models->Load("model://frustum_test.bxon");
}

void Demo_FrustumVolume::Render(float dt){
    Scene::Camera * camera1 = models->GetCamera("Camera");
	Scene::Camera * camera2 = models->GetCamera("Camera.001"); // view point camera

	float aspect = wnd->GetWidth() / (float)wnd->GetHeight();

	camera1->SetAspect(aspect);
	camera2->SetAspect(aspect);

    Math::Mat44 proj = camera2->GetMatrix(Graph::MATRIX_PROJECTION);
    Math::Mat44 view = camera2->GetMatrix(Graph::MATRIX_VIEW);

    dev->ClearColor(0.5, 0.5, 0.5);

    dev->Clear();

    dev->Viewport(0, 0, wnd->GetWidth(), wnd->GetHeight());

    // Render models with frustum culling
    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();
    dev->LoadMatrix((float*)&proj);

    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();
    dev->LoadMatrix((float*)&view);

    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();

	proj = camera1->GetMatrix(Graph::MATRIX_PROJECTION);
	view = camera1->GetMatrix(Graph::MATRIX_VIEW);

    Math::Frustum frustum;
    frustum.Update(view, proj);

    dev->FillMode(Graph::FILL_WIREFRAME);

    models->Render();


	float fov = (camera1->GetFov() * M_PI / 180) / 2.0;
	float d1 = atan(fov) * camera1->GetStart();
	float d2 = atan(fov) * camera1->GetEnd();

	Math::Vec3 fStart[4] = {
		Math::Vec3(-d1,-d1/ aspect,camera1->GetStart()),
		Math::Vec3(-d1,d1/aspect,camera1->GetStart()),
		Math::Vec3(d1,d1/aspect,camera1->GetStart()),
		Math::Vec3(d1,-d1/ aspect,camera1->GetStart()),
	};

	Math::Vec3 fEnd[4] = {
		Math::Vec3(-d2,-d2/ aspect,-camera1->GetEnd()),
		Math::Vec3(-d2,d2/aspect,-camera1->GetEnd()),
		Math::Vec3(d2,d2/aspect,-camera1->GetEnd()),
		Math::Vec3(d2,-d2/aspect,-camera1->GetEnd()),
	};

	Math::Mat44 mat = camera1->GetObject()->GetMatrix();
	for (int i = 0; i < 4; i++) {
		fStart[i] = Math::Vec3(Math::Vec4(fStart[i], 1.0) *  mat);
		fEnd[i] = Math::Vec3(Math::Vec4(fEnd[i], 1.0) * mat);
	}

	dev->Color(255, 200, 0);

	for (int i = 0; i < 4; i++) {
		Math::Vec3 a = fStart[i];
		Math::Vec3 b = fStart[(i+1) % 4];
		dev->Begin(Graph::PRIMITIVE_LINES);
		dev->Vertex(a.GetX(), a.GetY(), a.GetZ());
		dev->Vertex(b.GetX(), b.GetY(), b.GetZ());
		dev->End();
	}

	for (int i = 0; i < 4; i++) {
		Math::Vec3 a = fEnd[i];
		Math::Vec3 b = fEnd[(i + 1) % 4];
		dev->Begin(Graph::PRIMITIVE_LINES);
		dev->Vertex(a.GetX(), a.GetY(), a.GetZ());
		dev->Vertex(b.GetX(), b.GetY(), b.GetZ());
		dev->End();
	}


	for (int i = 0; i < 4; i++) {

		Math::Vec3 a = fStart[i];
		Math::Vec3 b = fEnd[i];

		dev->Begin(Graph::PRIMITIVE_LINES);
		dev->Vertex(a.GetX(), a.GetY(), a.GetZ());
		dev->Vertex(b.GetX(), b.GetY(), b.GetZ());
		dev->End();
	}


    // Finish rendering and present the graphics.
    dev->PresentAll();
}

void Demo_FrustumVolume::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_FrustumVolume::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Basic");
    ret.push_back("Rendering");
    ret.push_back("Frustum");
    return ret;
}

std::string Demo_FrustumVolume::GetDescription() {
    return "Render frustum volume";
}

Demo * CreateDemo_FrustumVolume(Core::Window * wnd, Graph::Device * dev){
	return new Demo_FrustumVolume(wnd,dev);
}
