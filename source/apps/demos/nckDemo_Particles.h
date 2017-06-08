
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#ifndef _NCK_DEMO_PARTICLES_H_
#define _NCK_DEMO_PARTICLES_H_

#include "../nckDemo.h"

class Demo_Particles : public Demo{
public:
    Demo_Particles(Core::Window * wnd, Graph::Device * dev);
	~Demo_Particles();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();
    std::vector<std::string> GetKeywords();
    std::string GetDescription();

private:
    Graph::Program * m_ParticlesProg;
    Graph::Mesh * m_ParticlesMsh;

    Graph::Mesh * createParticles(int dim_x, int dim_y);
	float time;
};

Demo * CreateDemo_Particles(Core::Window * wnd, Graph::Device * dev);

#endif
