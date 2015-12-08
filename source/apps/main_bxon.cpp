
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckWindow.h"
#include "nckException.h"
#include "nckThread.h"

//#include "bXScene.h"

#include "nckUtils.h"
#include "nckDataIO.h"

#include "nckVec3.h"
#include "nckMat44.h"
#include "nckMathUtils.h"
#include "nckQuat.h"
#include "nckTriangle.h"
#include "nckBoundBox.h"
#include "nckQuadtree.h"
#include "nckLine.h"
#include "nckWidget.h"
#include "nckGraphics.h"
#include "nckFont.h"
#include "nckCompound.h"

#include "bxonDataIO.h"

class GraphicRendering : public virtual Core::Threadable
{
public:
    GraphicRendering(Core::Window * window) : Threadable(){
        wnd = window;
        dev = NULL;
    }
    
    ~GraphicRendering(){
        
    }
    
    void Run(){
        wnd->SetTitle("nctoolkit");
        
        Scene::Compound_Basic * scene = NULL;
        Graph::Program * prog = NULL;
        
        try{
            dev = Graph::CreateDevice(wnd,Graph::DEVICE_AUTO,wnd->GetWidth(),wnd->GetHeight());
            dev->ClearColor(0.5, 0.5, 0.5, 1.0);
            dev->ClearFlags(Graph::BUFFER_COLOR_BIT|Graph::BUFFER_DEPTH_BIT);
            
            Core::FileReader * fr = Core::FileReader::Open("model://test.bxon");
            BXON::ReaderContext * frCtx = new BXON::ReaderContext(fr);
            BXON::Object * obj = BXON::Object::Parse(dynamic_cast<BXON::Context*>(frCtx));
            
            BXON::Map * map = dynamic_cast<BXON::Map*>(obj);
            Core::DebugLog(BXON::Object::ToJSON(map, 0, 1));
            
            scene = new Scene::Compound_Basic(dev);
            scene->Load(map);
            
            prog = dev->LoadProgram("shader://caravela.cpp");
            
            dev->Enable(Graph::STATE_DEPTH_TEST);
            
            delete map;
            delete frCtx;
            delete fr;
        }
        catch(const Core::Exception & ex){
            ex.PrintStackTrace();
            SafeDelete(dev);
            return;
        }
        
        Scene::Camera * camera = (Scene::Camera*)scene->GetDatablock(Scene::DATABLOCK_CAMERA,"Camera");
        Scene::Armature * armature = (Scene::Armature*)scene->GetDatablock(Scene::DATABLOCK_ARMATURE, "Armature.001");
        
        float zRotation = 0.0;
        float alpha = 0.0;
        
        while(!IsTearingDown())
        {
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
            
            dev->Color(255, 255, 255);
            
            // Rotate the caravela in the Z axis.
            //dev->Rotate(zRotation,0,0,1);
            
            prog->Enable();
            
            // Render the caravela.
            //scene->Render();
            armature->Render();
            
            prog->Disable();
            
            dev->PresentAll();
            
            if(alpha > 60.0)
                alpha = 0.0;
            
            armature->Play("", alpha);
            armature->Update();
            
            //camera->GetObject()->Play(alpha);
            
            alpha+= 2.0/60.0;
            
            zRotation += 0.1;
            
            wnd->SetTitle(Math::IntToString(alpha));
        }
        
        SafeDelete(scene);
        SafeDelete(dev);
    }
    
    void UpdateWndInput(){
        if(wnd->GetKeyStatus(Core::BUTTON_ESCAPE) != Core::BUTTON_STATUS_UP)
            Teardown();
    }
    
private:
    Graph::Device * dev;
    Core::Window  * wnd;
};

void Core::Application_Main(const std::vector<std::string> & CmdLine)
{
    // Create a window.
    Core::Window * wnd = Core::CreateWindow("nctoolkit", 640, 480, false);
    
    // Create the thread where the graphics rendering will run.
    GraphicRendering * grThread = new GraphicRendering(wnd);
    
    // Start graphics thread.
    grThread->Start();
    
    // Get window events.
    while(wnd->Peek(true))
    {
        // If on selector frontend, escape will tell
        // the thread to enter shutdown.
        if(!grThread->IsRunning())
            break;
        
        grThread->UpdateWndInput();
    }
    
    // Tell the thread to end and wait.
    grThread->TearDownAndJoin();
    
    // Release stuff from memory.
    SafeDelete(grThread);
    SafeDelete(wnd);
}

