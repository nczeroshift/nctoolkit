
#ifndef NCK_RENDER_PASS_H
#define NCK_RENDER_PASS_H

#include "nckCompound.h"

_SCENE_BEGIN

void RenderPass_SwitchProgram(Scene::Compound_Base * scene, Graph::Program * prog);
Math::Mat44 RenderPass_RenderFromLamp(Graph::Device * dev, Scene::Compound_Base * scene, Scene::Lamp * lamp);
void RenderPass_RenderWithLamps(Graph::Device * dev, Scene::Compound_Base * lights, Scene::Compound_Base * scene, 
	const Math::Mat44 & proj, const Math::Mat44 & view, const Math::Mat44 & world);

void RenderPass_CastShadowBuffers(
	Graph::Device * dev,
	Scene::Compound_Base * lights,
	Graph::Program * shadowPass,
	Graph::Texture * texRender,
	Graph::Texture * texPosition,
	Graph::Texture * texNormal,
	const Math::Mat44 & proj,
	const Math::Mat44 & view,
	const Math::Mat44 & world,
	const Math::Vec2 & wndSize);

_SCENE_END

#endif