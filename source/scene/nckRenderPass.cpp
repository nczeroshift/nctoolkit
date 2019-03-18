 
#include "nckCompound.h"
#include "nckMathUtils.h"
#include "nckUtils.h"
#include "nckObject.h"
#include "nckException.h"
#include "nckTransform.h"

_SCENE_BEGIN


void RenderPass_SwitchProgram(Scene::Compound_Base * scene, Graph::Program * prog) {
	std::vector<Scene::Material*> mats;
	scene->GetAllMaterials(&mats);
	for (int i = 0; i < mats.size(); i++) {
		mats[i]->SetProgram(prog);
	}
}

Math::Mat44 RenderPass_RenderFromLamp(
	Graph::Device * dev,
	Scene::Compound_Base * scene,
	Scene::Lamp * lamp)
{
	const float zNear = lamp->GetClipStart();
	const float zFar = lamp->GetClipEnd();

	dev->Disable(Graph::STATE_BLEND); // Disable alpha blend to avoid issues with depth.

	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();

	Math::Mat44 lampProj = Math::Perspective(1.0, 90, zNear, zFar);
	dev->MultMatrix((float*)&lampProj);

	dev->MatrixMode(Graph::MATRIX_VIEW);
	dev->Identity();

	Scene::Camera lampCamp(dev);
	lampCamp.SetObject(lamp->GetObject());

	Math::Mat44 lampView = lampCamp.GetMatrix();
	dev->MultMatrix((float*)&lampView);

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();

	Math::Mat44 lampProjView = lampView * lampProj;

	Math::Frustum lampFrustum;
	lampFrustum.Update(lampView, lampProj);

	//scene->Render(&lampFrustum);
	scene->Render(Scene::RenderControl().SetFrustum(&lampFrustum).SetPass(MATERIAL_PASS_SHADOW));

	return lampProjView;
}

void RenderPass_RenderWithLamps(
	Graph::Device * dev,
	Scene::Compound_Base * lights,
	Scene::Compound_Base * scene,
	const Math::Mat44 & proj,
	const Math::Mat44 & view,
	const Math::Mat44 & world)
{
	Math::Mat44 modelView = world * view;

	std::vector<Scene::Object*> lampObjs;
	lights->GetObjectsWithLayer(&lampObjs, Scene::DATABLOCK_LAMP);

	std::vector<Scene::Material*> allMaterials;
	scene->GetAllMaterials(&allMaterials);

	Scene::LampUniforms lu;

	Scene::Lamp::GenerateUniforms(lampObjs, modelView, &lu);
	for (int i = 0; i < allMaterials.size(); i++) {
		Scene::Material * mat = allMaterials[i];
		Graph::Program * prog = mat->GetProgram();
		if (prog)
			lu.Bind(prog);
	}

	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();
	dev->LoadMatrix((float*)&proj);

	dev->MatrixMode(Graph::MATRIX_VIEW);
	dev->Identity();
	dev->LoadMatrix((float*)&view);

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();
	dev->LoadMatrix((float*)&world);

	Math::Frustum frustum;
	frustum.Update(view, proj);

	scene->Render(Scene::RenderControl().SetFrustum(&frustum));
}

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
	const Math::Vec2 & wndSize)
{
	Math::Mat44 modelView = world * view;
	Math::Mat44 cameraInvMV;
	Math::Invert(modelView, &cameraInvMV);
	
	std::vector<Scene::Object*> lampObjs;
	lights->GetObjectsWithLayer(&lampObjs, Scene::DATABLOCK_LAMP);

	for (int i = 0; i < lampObjs.size(); i++)
	{
		Scene::Lamp * lamp = dynamic_cast<Scene::Lamp*>(lampObjs[i]->GetData());
		Graph::Texture * shadowTex = lamp->GetTexture();

		if (lamp->GetLampType() == Scene::LAMP_TYPE_SPOT && lamp->CastShadow())
		{
			if (!shadowTex)
				continue;

			Scene::Camera lampCamp(dev);
			lampCamp.SetObject(lamp->GetObject());
			lampCamp.SetAspect(1);
			lampCamp.SetFov(90);
			lampCamp.SetEnd(lamp->GetClipEnd());
			lampCamp.SetStart(lamp->GetClipStart());

			Math::Mat44 lampViewMat = lampCamp.GetMatrix(Graph::MATRIX_VIEW);
			Math::Mat44 lampProjMat = lampCamp.GetMatrix(Graph::MATRIX_PROJECTION);

			float fov = (lampCamp.GetFov() * M_PI/180)/2.0;
			float d1 = atan(fov) * lamp->GetClipStart();
			float d2 = atan(fov) * lamp->GetClipEnd();

			Math::Vec3 fStart[4] = {
				Math::Vec3(-d1,-d1,lamp->GetClipStart()),
				Math::Vec3(-d1,d1,lamp->GetClipStart()),
				Math::Vec3(d1,d1,lamp->GetClipStart()),
				Math::Vec3(d1,-d1,lamp->GetClipStart()),
			};

			Math::Vec3 fEnd[4] = {
				Math::Vec3(-d2,-d2,-lamp->GetClipEnd()),
				Math::Vec3(-d2,d2,-lamp->GetClipEnd()),
				Math::Vec3(d2,d2,-lamp->GetClipEnd()),
				Math::Vec3(d2,-d2,-lamp->GetClipEnd()),
			};

			for (int i = 0; i < 4; i++) {
				fStart[i] = Math::Vec3(Math::Vec4(fStart[i], 1.0) * lampViewMat);
				fEnd[i] = Math::Vec3(Math::Vec4(fEnd[i], 1.0) * lampViewMat);
				fStart[i] = Math::Vec3(Math::Project(proj, modelView, fStart[i], wndSize),0);
				fEnd[i] = Math::Vec3(Math::Project(proj, modelView, fEnd[i], wndSize), 0);
			}
			
			Math::Mat44 lampProjViewMat = lampViewMat * lampProjMat;

			Math::Mat44 finalSpotMatrix = cameraInvMV * lampProjViewMat;

			Math::Vec4 lampDir = (Math::Vec4(0, 0, 1, 0) * lamp->GetObject()->GetMatrix()) * modelView;

			float bias = 0.001;
			shadowPass->SetMatrix("shadow_pmv", (float*)&finalSpotMatrix);
			shadowPass->SetVariable4f("shadow_params", shadowTex->GetWidth(), shadowTex->GetHeight(), bias, 0);
			shadowPass->SetVariable4f("lamp_dir", lampDir.GetX(), lampDir.GetY(), lampDir.GetZ(), lampDir.GetW());

			texRender->Enable(0);
			texPosition->Enable(1);
			texNormal->Enable(2);
			shadowTex->Enable(3);

			shadowPass->Enable();
			{
				float x = 0, y = 0, w = wndSize.GetX(), h = wndSize.GetY();

				dev->Color(255, 255, 255, 255);
				dev->Begin(Graph::PRIMITIVE_QUADS);
				dev->TexCoord(0, 1);
				dev->Vertex(x, y);
				dev->TexCoord(0, 0);
				dev->Vertex(x, y + h);
				dev->TexCoord(1, 0);
				dev->Vertex(x + w, y + h);
				dev->TexCoord(1, 1);
				dev->Vertex(x + w, y);
				dev->End();
			}
			shadowPass->Disable();

			shadowTex->Disable(3);
			texNormal->Disable(2);
			texPosition->Disable(1);
			texRender->Disable(0);

			for (int i = 0; i < 4; i++) {
			
				Math::Vec3 a = fStart[i];
				Math::Vec3 b = fEnd[i];
				dev->Color(255,255,0);
				dev->Begin(Graph::PRIMITIVE_LINES);
				dev->Vertex(a.GetX(), a.GetY(), a.GetZ());
				dev->Vertex(b.GetX(), b.GetY(), b.GetZ());
				dev->End();
			}
		}
	}
}


_SCENE_END