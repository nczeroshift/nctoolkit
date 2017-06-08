
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_Particles.h"
#include "nckQueueBuffer.h"

Demo_Particles::Demo_Particles(Core::Window * wnd, Graph::Device * dev) {
    this->dev = dev;
    this->wnd = wnd;
    time = 0;
    m_ParticlesProg = NULL;
    m_ParticlesMsh = NULL;
}

Demo_Particles::~Demo_Particles() {
    SafeDelete(m_ParticlesProg);
    SafeDelete(m_ParticlesMsh);
}

void Demo_Particles::Load() {
    // Enable z test
    dev->Enable(Graph::STATE_DEPTH_TEST);

    // Set correct states for alpha blending.
    dev->Enable(Graph::STATE_BLEND);
    dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

    m_ParticlesProg = dev->LoadProgram("shader://particles.cpp");
    m_ParticlesMsh = createParticles(32, 32);
}

Graph::Mesh * Demo_Particles::createParticles(int dim_x, int dim_y)
{
    Graph::VertexProfile vp(2);
    vp.PushBack(0, 3, 1, 0);
    vp.PushBack(sizeof(float) * 3, 2, 4, 0);

    const unsigned int v_count = dim_x * dim_y * 4;
    const unsigned int f_count = dim_x * dim_y * 2;

    Core::QueueBuffer * vbdata = new Core::QueueBuffer(v_count * (3 + 2) * sizeof(float));
    Core::QueueBuffer * fbdata = new Core::QueueBuffer(f_count * 3 * sizeof(int));

    unsigned int faces_count = 0;
    unsigned int vIdx = 0;

    for (int x = 0; x < dim_x; x++)
    {
        for (int y = 0; y < dim_y; y++)
        {
            Math::Vec2 uv(x / (float)dim_x, y / (float)dim_y);
            Math::Vec3 p;

            float rnd = Math::RandomValue(-1, 1);

            p = Math::Vec3(-1, -1, rnd);
            vbdata->Push((void*)&p, sizeof(Math::Vec3));
            vbdata->Push((void*)&uv, sizeof(Math::Vec2));

            p = Math::Vec3(-1, 1, rnd);
            vbdata->Push((void*)&p, sizeof(Math::Vec3));
            vbdata->Push((void*)&uv, sizeof(Math::Vec2));

            p = Math::Vec3(1, 1, rnd);
            vbdata->Push((void*)&p, sizeof(Math::Vec3));
            vbdata->Push((void*)&uv, sizeof(Math::Vec2));

            p = Math::Vec3(1, -1, rnd);
            vbdata->Push((void*)&p, sizeof(Math::Vec3));
            vbdata->Push((void*)&uv, sizeof(Math::Vec2));

            unsigned int v1 = vIdx;
            unsigned int v2 = vIdx + 1;
            unsigned int v3 = vIdx + 2;
            unsigned int v4 = vIdx + 3;

            fbdata->Push((void*)&v1, sizeof(unsigned int));
            fbdata->Push((void*)&v2, sizeof(unsigned int));
            fbdata->Push((void*)&v3, sizeof(unsigned int));

            fbdata->Push((void*)&v1, sizeof(unsigned int));
            fbdata->Push((void*)&v3, sizeof(unsigned int));
            fbdata->Push((void*)&v4, sizeof(unsigned int));

            vIdx += 4;
            faces_count += 2;
        }
    }

    unsigned int gCount[2] = { faces_count ,0 };
    unsigned int * fdataGrouped[2] = { (unsigned int*)fbdata->GetData(),0 };

    Graph::Mesh * ret = dev->CreateMesh(v_count, vp, vbdata->GetData(), 1, gCount, fdataGrouped);

    SafeDelete(vbdata);
    SafeDelete(fbdata);

    return ret;
}

void Demo_Particles::Render(float dt) {
    // Clear graphics rendering buffer.
    dev->ClearColor(0, 0, 0);
    dev->Clear();

    // Define the rendering area in window space.
    dev->Viewport(0, 0, wnd->GetWidth(), wnd->GetHeight());

    // Set projection matrix.
    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();
    dev->Perspective(wnd->GetWidth() / (float)wnd->GetHeight());

    // Set the view (camera) matrix.
    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();

    Scene::Object objCamera(dev);
    objCamera.SetPosition(Math::Vec3(0.5, -32.45318, 2.60367));
    objCamera.SetRotation(Math::EulerToQuat(Math::Vec3(80, 0, 0) * M_PI / 180.0));

    Scene::Camera cam(dev);
    cam.SetObject(&objCamera);
    cam.Enable();

    // Set the model matrix.
    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();

    dev->Disable(Graph::STATE_DEPTH_TEST);

    float p1 = 0.1;
    float p2 = 0.1;
    float p3 = 0.0;
    float p4 = 0.5;

    m_ParticlesProg->SetVariable1f("dt", time);

    m_ParticlesProg->SetVariable1f("p1", p1);
    m_ParticlesProg->SetVariable1f("p2", p2);
    m_ParticlesProg->SetVariable1f("p3", p3);
    m_ParticlesProg->SetVariable1f("p4", p4);

    m_ParticlesProg->Enable();

    m_ParticlesMsh->Enable();
    m_ParticlesMsh->Render();
    m_ParticlesMsh->Disable();

    m_ParticlesProg->Disable();

    time += dt;

    // Finish rendering and present the graphics.
    dev->PresentAll();
}

void Demo_Particles::UpdateWndEvents() {

}

std::vector<std::string> Demo_Particles::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Basic");
    ret.push_back("Particles");
    ret.push_back("Billboards");
    return ret;
}

std::string Demo_Particles::GetDescription() {
    return "Particles Rendering";
}

Demo * CreateDemo_Particles(Core::Window * wnd, Graph::Device * dev) {
    return new Demo_Particles(wnd, dev);
}
