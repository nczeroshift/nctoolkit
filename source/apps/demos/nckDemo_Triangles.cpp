
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_Triangles.h"

Demo_Triangles::Demo_Triangles(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
}

Demo_Triangles::~Demo_Triangles(){
	
}

void Demo_Triangles::Load(){
    // Disable z test
    dev->Enable(Graph::STATE_DEPTH_TEST);

    // Set correct states for alpha blending.
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);
}

void Demo_Triangles::Render(float dt){
	// Clear graphics rendering buffer.
	dev->Clear();

	// Define the rendering area in window space.
	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

	// Set projection matrix.
	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();

    dev->Perspective(wnd->GetWidth() / (float)wnd->GetHeight());
    
    // Set the view (camera) matrix.
    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();
   
    dev->Translate(0, 0, -10);

	// Set the model matrix.
	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();
    
    int rotAngle = 0;
    for (int x = -3; x <= 3; x++) {
        for (int y = -2; y <= 2; y++) {
            dev->PushMatrix();
            dev->Translate(x, y, 0);
            dev->Rotate(rotAngle + time, 0, 0, 1);
            dev->Scale(0.5, 0.5, 0.5);

            dev->Begin(Graph::PRIMITIVE_TRIANGLES);
            dev->Color(255, 0, 0);
            dev->Vertex(0, 1);
            dev->Color(0, 255, 0);
            dev->Vertex(-1, 0);
            dev->Color(0, 0, 255);
            dev->Vertex(1, 0);
            dev->End();
            dev->PopMatrix();

            rotAngle++;
        }
    }
 
    time += dt;

	// Finish rendering and present the graphics.
	dev->PresentAll();
}

void Demo_Triangles::UpdateWndEvents(){
	
}

std::vector<std::string> Demo_Triangles::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Basic");
    ret.push_back("Rendering");
    return ret;
}

std::string Demo_Triangles::GetDescription() {
    return "Triangles Rendering";
}

Demo * CreateDemo_Triangles(Core::Window * wnd, Graph::Device * dev){
	return new Demo_Triangles(wnd,dev);
}
