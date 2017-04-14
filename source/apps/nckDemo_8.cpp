
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* Demo 8 - Http & Screen capture stream
*/

#include "nckDemo_8.h"

Demo8::Demo8(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;

	shader = NULL;
	model = NULL;
	camera = NULL;
	server = NULL;
	fileBuffer = NULL;
	zRotation = 0;
	renderWireframe = false;

	rawBuffer = Core::Image::Create(640,480,Core::PIXEL_FORMAT_RGB_8B);

	try{
		server = Network::HttpServer::Create(8080);
	}
	catch (const Core::Exception & ex) {
		ex.PrintStackTrace();
		try{
			server = Network::HttpServer::Create(0); // Find an available port
		}
		catch (const Core::Exception & ex2) {
			ex.PrintStackTrace();
			return;
		}
	}
	
	server->SetCallback(this);
	wnd->SetTitle(wnd->GetTitle()+" (http://localhost:" + Math::IntToString(server->GetPort())+"/preview.html)");
}

Demo8::~Demo8(){
	SafeDelete(server);
	SafeArrayDelete(fileBuffer);
	SafeDelete(rawBuffer);
	SafeDelete(shader);
	SafeDelete(model);
}

void Demo8::Load(){
	dev->Enable(Graph::STATE_BLEND);
	dev->Enable(Graph::STATE_DEPTH_TEST);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA,Graph::BLEND_INV_SRC_ALPHA);

	// Load the shader program to render the caravela.
	shader = dev->LoadProgram("shader://caravela.cpp");

	// Create the compound where the caravela model will be loaded.
	model = new Scene::Compound_Base(dev);

	// Load the caravela compound model from the data/models folder.
	model->Load("model://caravela.nc");

	// Fetch camera object from compound.
	camera = (Scene::Camera*)model->GetDatablock(Scene::DATABLOCK_CAMERA,"Camera");
}

size_t Demo8::GetImage(const std::string & srcAddr, Core::ImageType type,
                        std::map<std::string,std::string> params, unsigned char ** data )
{
	fetchPicture = true;

	Core::Chronometer * chron = Core::CreateChronometer(true);
	chron->Start();

	// Since we can't know when the thread is canceling, if the picture 
	// isn't resolved in less than 1 sec, exit.
	while(fetchPicture && chron->GetElapsedTime()<1000)
		Core::Thread::Wait(1);

	if(chron->GetElapsedTime()>1000){
		SafeDelete(chron);
		return 0;
	}

	SafeDelete(chron);

	size_t size = rawBuffer->Save(Core::IMAGE_TYPE_JPEG,80,&fileBuffer);

	*data = fileBuffer;

	return size;
}

void Demo8::Render(float dt){
	// Clear graphics rendering buffer.
	dev->Clear();

	// Define the rendering area in window space.
	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

	// Set projection matrix.
	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();

	// Use camera properties as projection matrix (fov & aspect).
	camera->Enable(Graph::MATRIX_PROJECTION);

	// Set the model matrix.
	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();

	// Set camera properties as view matrix (rotation & position).
	camera->Enable(Graph::MATRIX_VIEW);

	// Rotate the caravela in the Z axis.
	dev->Rotate(zRotation,0,0,1);

	// Set the shader tex1 as the first enabled texture(use in the model material).
	shader->SetVariable1i("tex1",0);

	// Enable the shader.
	shader->Enable();

	if(renderWireframe)
		dev->FillMode(Graph::FILL_WIREFRAME);
	else
		dev->FillMode(Graph::FILL_SOLID);

	// Render the caravela.
	model->Render();

	// Disable the shader.
	shader->Disable();

	// Keep increasing the rotation.
	zRotation+=0.1;

	if(fetchPicture){
		dev->Capture(0,0,rawBuffer->GetWidth(),rawBuffer->GetHeight(),Graph::FORMAT_RGB_8B,rawBuffer->GetData());
		fetchPicture = false;
	}

	// Finish rendering and present the graphics.
	dev->PresentAll();
}

void Demo8::UpdateWndEvents(){
	static bool btnStatusZ = false;
	if(!btnStatusZ && wnd->GetKeyStatus(Core::BUTTON_Z)==Core::BUTTON_STATUS_DOWN)
		renderWireframe = !renderWireframe;
	btnStatusZ = wnd->GetKeyStatus(Core::BUTTON_Z)==Core::BUTTON_STATUS_DOWN;
}


Demo * CreateDemo_8(Core::Window * wnd, Graph::Device * dev){
	return new Demo8(wnd,dev);
}
