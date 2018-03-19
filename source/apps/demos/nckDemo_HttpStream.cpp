
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_HttpStream.h"

Demo_HttpStream::Demo_HttpStream(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    
    shader = NULL;
    model = NULL;
    camera = NULL;
    server = NULL;
    fileBuffer = NULL;
    rawBuffer = NULL;

    zRotation = 0;
    renderWireframe = false;
}

Demo_HttpStream::~Demo_HttpStream(){
    SafeDelete(server);
    SafeArrayDelete(fileBuffer);
    SafeDelete(rawBuffer);
    SafeDelete(shader);
    SafeDelete(model);
}

void Demo_HttpStream::Load(){
    dev->Enable(Graph::STATE_DEPTH_TEST);
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    shader = dev->LoadProgram("shader://caravela_mat.cpp");
    model = new Scene::Compound_Base(dev);
    model->Load("model://caravela_scene.bxon");
    camera = model->GetCamera("Camera");

    rawBuffer = Core::Image::Create(640, 480, Core::PIXEL_FORMAT_RGB_8B);

    try {
        server = Network::HttpServer::Create(8080);
    }
    catch (const Core::Exception & ex) {
        ex.PrintStackTrace();
        try {
            server = Network::HttpServer::Create(0); // Find an available port
        }
        catch (const Core::Exception & ex2) {
            ex.PrintStackTrace();
            return;
        }
    }

    server->SetCallback(this);
    wnd->SetTitle(wnd->GetTitle() + " (http://localhost:" + Math::IntToString(server->GetPort()) + "/preview.html)");
}

bool Demo_HttpStream::HandleRequest(Network::HttpRequest * request, Network::HttpResponse * response) {
    if (request->GetMethod() == "GET") {
        fetchPicture = true;

        Core::Chronometer * chron = Core::CreateChronometer(true);
        chron->Start();

        // Since we can't know when the thread is canceling, if the picture 
        // isn't resolved in less than 1 sec, exit.
        while (fetchPicture && chron->GetElapsedTime()<1000)
            Core::Thread::Wait(1);

        if (chron->GetElapsedTime()>1e6) {
            SafeDelete(chron);
            return 0;
        }

        SafeDelete(chron);

        size_t size = rawBuffer->Save(Core::IMAGE_TYPE_JPEG, 80, &fileBuffer);

        response->SetType(Network::MIME_JPEG_IMAGE);
        response->GetBuffer()->Push(fileBuffer, size);
        response->SetStatusCode(200);
    }
    return true;
}

void Demo_HttpStream::Render(float dt) {
    dev->ClearColor(0.5, 0.5, 0.5);
    dev->Clear();
    dev->Viewport(0, 0, wnd->GetWidth(), wnd->GetHeight());

    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();

    camera->SetAspect(wnd->GetWidth() / (float)wnd->GetHeight());
    camera->Enable(Graph::MATRIX_PROJECTION);

    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();

    camera->Enable(Graph::MATRIX_VIEW);
    dev->Rotate(zRotation, 0, 0, 1);

    shader->SetVariable1i("tex1", 0);
    shader->Enable();

    if (renderWireframe)
        dev->FillMode(Graph::FILL_WIREFRAME);
    else
        dev->FillMode(Graph::FILL_SOLID);

    model->Render();
    shader->Disable();
    zRotation += 0.1;

    if (fetchPicture) {
        dev->Capture(0, 0, rawBuffer->GetWidth(), rawBuffer->GetHeight(), Graph::FORMAT_RGB_8B, rawBuffer->GetData());
        fetchPicture = false;
    }

    dev->PresentAll();
}

void Demo_HttpStream::UpdateWndEvents() {
    static bool btnStatusZ = false;
    if (!btnStatusZ && wnd->GetKeyStatus(Core::BUTTON_Z) == Core::BUTTON_STATUS_DOWN)
        renderWireframe = !renderWireframe;
    btnStatusZ = wnd->GetKeyStatus(Core::BUTTON_Z) == Core::BUTTON_STATUS_DOWN;
}

std::vector<std::string> Demo_HttpStream::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Advanced");
    ret.push_back("Http");
    ret.push_back("Server");
    ret.push_back("Stream");
    return ret;
}

std::string Demo_HttpStream::GetDescription() {
    return "Http Server with Images Streaming";
}

Demo * CreateDemo_HttpStream(Core::Window * wnd, Graph::Device * dev){
	return new Demo_HttpStream(wnd,dev);
}
