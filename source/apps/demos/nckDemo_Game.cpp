
/**
* NCtoolKit © 2007-2017 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*/

#include "nckDemo_Game.h"


Graph::Mesh * CreateTriangleMesh(Graph::Device * dev) {
	Graph::VertexProfile vp(2);
	vp.PushBack(0, 3, 1, 0);
	vp.PushBack(sizeof(float) * 3, 2, 4, 0);

	const unsigned int v_count = 3;
	const unsigned int f_count = 1;

	Core::QueueBuffer * vbdata = new Core::QueueBuffer(v_count * (3 + 2) * sizeof(float));
	Core::QueueBuffer * fbdata = new Core::QueueBuffer(f_count * 3 * sizeof(int));

	unsigned int faces_count = 0;
	unsigned int vIdx = 0;

	Math::Vec2 uv(0, 0);
	Math::Vec3 p;

	p = Math::Vec3(0, 1, 0.0f); uv = Math::Vec2(0, 1);
	vbdata->Push((void*)&p, sizeof(Math::Vec3));
	vbdata->Push((void*)&uv, sizeof(Math::Vec2));

	p = Math::Vec3(-1, -1, 0.0f); uv = Math::Vec2(0, 0);
	vbdata->Push((void*)&p, sizeof(Math::Vec3));
	vbdata->Push((void*)&uv, sizeof(Math::Vec2));

	p = Math::Vec3(1, -1, 0.0f); uv = Math::Vec2(1, 0);
	vbdata->Push((void*)&p, sizeof(Math::Vec3));
	vbdata->Push((void*)&uv, sizeof(Math::Vec2));

	
	unsigned int v1 = vIdx;
	unsigned int v2 = vIdx + 1;
	unsigned int v3 = vIdx + 2;
	unsigned int v4 = vIdx + 3;

	fbdata->Push((void*)&v1, sizeof(unsigned int));
	fbdata->Push((void*)&v2, sizeof(unsigned int));
	fbdata->Push((void*)&v3, sizeof(unsigned int));

	vIdx += 3;
	faces_count += 1;

	unsigned int gCount[2] = { faces_count ,0 };
	unsigned int * fdataGrouped[2] = { (unsigned int*)fbdata->GetData(),0 };

	Graph::Mesh * mesh = dev->CreateMesh(v_count, vp, vbdata->GetData(), 1, gCount, fdataGrouped);

	SafeDelete(vbdata);
	SafeDelete(fbdata);

	return mesh;
}

Graph::Mesh * CreateSquareMesh(Graph::Device * dev) {
	Graph::VertexProfile vp(2);
	vp.PushBack(0, 3, 1, 0);
	vp.PushBack(sizeof(float) * 3, 2, 4, 0);

	const unsigned int v_count = 4;
	const unsigned int f_count = 2;

	Core::QueueBuffer * vbdata = new Core::QueueBuffer(v_count * (3 + 2) * sizeof(float));
	Core::QueueBuffer * fbdata = new Core::QueueBuffer(f_count * 3 * sizeof(int));

	unsigned int faces_count = 0;
	unsigned int vIdx = 0;

	Math::Vec2 uv(0, 0);
	Math::Vec3 p;

	p = Math::Vec3(-1, -1, 0.0f); uv = Math::Vec2(0, 1);
	vbdata->Push((void*)&p, sizeof(Math::Vec3));
	vbdata->Push((void*)&uv, sizeof(Math::Vec2));

	p = Math::Vec3(-1, 1, 0.0f); uv = Math::Vec2(0, 0);
	vbdata->Push((void*)&p, sizeof(Math::Vec3));
	vbdata->Push((void*)&uv, sizeof(Math::Vec2));

	p = Math::Vec3(1, 1, 0.0f); uv = Math::Vec2(1, 0);
	vbdata->Push((void*)&p, sizeof(Math::Vec3));
	vbdata->Push((void*)&uv, sizeof(Math::Vec2));

	p = Math::Vec3(1, -1, 0.0f); uv = Math::Vec2(1, 1);
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

	unsigned int gCount[2] = { faces_count ,0 };
	unsigned int * fdataGrouped[2] = { (unsigned int*)fbdata->GetData(),0 };

	Graph::Mesh * mesh = dev->CreateMesh(v_count, vp, vbdata->GetData(), 1, gCount, fdataGrouped);

	SafeDelete(vbdata);
	SafeDelete(fbdata);

	return mesh;
}

class Actor {
public:
	Actor(Graph::Device * dev);
	virtual ~Actor();

	Scene::Object * GetObject();
	Scene::Object * GetModelObject();
	void SetModelObject(Scene::Object * obj);

	void Render();
	virtual void UpdatePhysics(float dt);
	void UpdateLogic(float dt);
protected:
	Graph::Device * m_Device;
	Scene::Object * m_TransformObj; // Instanced by actor
	Scene::Object * m_ModelObj; // External reference
};


Actor::Actor(Graph::Device * dev) {
	m_Device = dev;
	m_TransformObj = new Scene::Object(dev);
	m_ModelObj = NULL;
}

Actor::~Actor() {
	SafeDelete(m_TransformObj);
}

void Actor::UpdatePhysics(float dt) {

}

void Actor::UpdateLogic(float dt) {

}

void Actor::SetModelObject(Scene::Object * obj) {
	m_ModelObj = obj;
}

Scene::Object * Actor::GetObject() {
	return m_TransformObj;
}

Scene::Object * Actor::GetModelObject() {
	return m_ModelObj;
}

void Actor::Render() {
	m_Device->PushMatrix();
	m_TransformObj->Bind();
	if (m_ModelObj != NULL) {
		if (m_ModelObj->GetData()->GetType() == Scene::DATABLOCK_MODEL) {
			m_ModelObj->Bind();
			Scene::Model * model = dynamic_cast<Scene::Model*>(m_ModelObj->GetData());
			model->Render();
		}
	}
	m_Device->PopMatrix();
}

class AnimationBlock {
public:
	AnimationBlock(float duration, float delay) {m_Time = 0.0f; m_Duration = duration; m_Delay = delay;	}
	virtual ~AnimationBlock() {}
	virtual void Update(float dt) {}
	bool isDone() { return m_Time-m_Delay >= m_Duration; }
	Math::Vec3 GetPosition() { return m_Position; };
	void SetPosition(Math::Vec3 pos) { m_Position = pos; };
	Math::Vec3 GetStart() { return m_Start; }
	void SetStart(Math::Vec3 start) { m_Position = m_Start = start; }
protected:
	float m_Duration, m_Time, m_Delay;
	Math::Vec3 m_Position, m_Start;
};

class AnimBlock_ColumnPull : public AnimationBlock { public:
	AnimBlock_ColumnPull(float duration, float delay) : AnimationBlock(duration, delay) {}
	~AnimBlock_ColumnPull() {}
	void Update(float dt) {
		m_Time += dt; // Animações começam logo a atualizar para não terminarem prematuramente.
		float t = Math::Clamp(0, 1, (m_Time - m_Delay) / m_Duration);
		m_Position = Math::LinearInterpolation(m_Start + Math::Vec3(0, - sin(t * 10.0), 0), m_Start, t * t);
	}
};

class AnimBlock_ColumnPush : public AnimationBlock { public:
	AnimBlock_ColumnPush(float duration, float delay) : AnimationBlock(duration, delay) {}
	~AnimBlock_ColumnPush() {}
	void Update(float dt) {
		m_Time += dt;
		float t = Math::Clamp(0, 1, (m_Time - m_Delay) / m_Duration);
		m_Position = Math::LinearInterpolation(m_Start + Math::Vec3(0, sin(t * 10.0), 0), m_Start, t * t);
	}
};

class AnimBlock_MoveToPos : public AnimationBlock { public:
	AnimBlock_MoveToPos(float duration, float delay, Math::Vec3 target) : AnimationBlock(duration, delay) { m_Target = target; }
	~AnimBlock_MoveToPos() {}
	void Update(float dt) {
		m_Time += dt;
		float t = Math::Clamp(0, 1, (m_Time - m_Delay) / m_Duration);
		m_Position = Math::LinearInterpolation(m_Start, m_Target, t * t);

	}
	Math::Vec3 m_Target;
};

class ActorAnimated : public Actor {
public:
	ActorAnimated(Graph::Device * dev);
	virtual ~ActorAnimated();
	void SetType(int type) { m_Type = type; }
	int GetType() { return m_Type; }
	void UpdatePhysics(float dt);
	void PushAnimation(AnimationBlock * block) { m_Animation.push_back(block); block->SetStart(GetObject()->GetPosition()); }
	bool HasAnimation() { return !m_Animation.empty(); }
	ActorAnimated * Clone();
	void ClearAnimations();
public:
	int m_Type;
	std::vector<AnimationBlock*> m_Animation;
};

ActorAnimated * ActorAnimated::Clone() {
	ActorAnimated * ret = new ActorAnimated(m_Device);
	ret->m_ModelObj = this->m_ModelObj;
	ret->m_TransformObj->SetPosition(this->m_TransformObj->GetPosition());
	ret->m_TransformObj->SetRotation(this->m_TransformObj->GetRotation());
	ret->m_TransformObj->SetScale(this->m_TransformObj->GetScale());
	ret->m_TransformObj->Update();
	ret->m_Type = this->m_Type;
	return ret;
}

ActorAnimated::ActorAnimated(Graph::Device * dev) : Actor(dev) {
	m_Type = 0;
}

ActorAnimated::~ActorAnimated() {

}

void ActorAnimated::UpdatePhysics(float dt) {

	if (!HasAnimation()) return;

	m_Animation[0]->Update(dt);

	const Math::Vec3 finalPos = m_Animation[0]->GetPosition();
	m_TransformObj->SetPosition(finalPos);
	m_TransformObj->Update();

	if (m_Animation[0]->isDone()) {
		delete *m_Animation.begin();
		m_Animation.erase(m_Animation.begin());

		if (HasAnimation())
			m_Animation[0]->SetStart(finalPos);
	}
}

void ActorAnimated::ClearAnimations() {
	while (!m_Animation.empty()) {
		delete *m_Animation.begin();
		m_Animation.erase(m_Animation.begin());
	}
}

class ActorMatrix {
public:
	ActorMatrix(int columns, int rows) {
		m_Columns = columns;
		m_Rows = rows;
		m_Matrix = new ActorAnimated**[m_Rows];
		for (int i = 0; i < m_Rows; i++) {
			m_Matrix[i] = new ActorAnimated*[m_Columns];
			for (int j = 0; j < m_Columns; j++)
				m_Matrix[i][j] = NULL;
		}
	}
	
	~ActorMatrix() {
	
	}

	void Render() {
		for (int i = 0; i < m_Rows; i++) {
			for (int j = 0; j < m_Columns; j++)
				if (m_Matrix[i][j] != NULL)
					m_Matrix[i][j]->Render();
		}

		ListFor(ActorAnimated*, m_Unassigned,i){
			(*i)->Render();
		}
	}

	void UpdatePhysics(float dt) {
		for (int i = 0; i < m_Rows; i++) {
			for (int j = 0; j < m_Columns; j++)
				if (m_Matrix[i][j] != NULL)
					m_Matrix[i][j]->UpdatePhysics(dt);
		}

		ListWhile(ActorAnimated*, m_Unassigned, i) {

			if (!(*i)->HasAnimation()) {
				delete *i;
				m_Unassigned.erase(i++); // Avança o iterador mas apaga o atual.
			}
			else {
				(*i)->UpdatePhysics(dt);
				i++;
			}
			
		}

		
	}

	bool HasAnimation() {
		bool hasAnimation = false;
		for (int i = 0; i < m_Rows; i++) {
			for (int j = 0; j < m_Columns; j++)
				if (m_Matrix[i][j] != NULL)
					hasAnimation |= m_Matrix[i][j]->HasAnimation();
		}
		return hasAnimation;
	}

	Math::Vec3 GetPosition(int column, int row) {
		return Math::Vec3(column * 2, -row * 2, 0);
	}

	void SetActor(int column, int row, ActorAnimated * actor) {
		if(column >= 0 && column < m_Columns && row >= 0 && row < m_Rows)
			m_Matrix[row][column] = actor;
	}

	ActorAnimated * GetActor(int column, int row) {
		if (column >= 0 && column < m_Columns && row >= 0 && row < m_Rows)
			return m_Matrix[row][column];
		return NULL;
	}

	void AnimateDown() {
		for (int i = m_Rows - 1; i >= 0; i--) {
			for (int j = 0; j < m_Columns; j++) {
				if (i == 0) {
					m_Matrix[i][j] = NULL;
				}
				else
				m_Matrix[i][j] = m_Matrix[i - 1][j];
			}
		}

		for (int i = 1; i < m_Rows; i++) {
			for (int j = 0; j < m_Columns; j++)
				if (m_Matrix[i][j] != NULL)
					m_Matrix[i][j]->PushAnimation(new AnimBlock_MoveToPos(0.3,0,this->GetPosition(j,i)));
		}
		
	}

	void AnimateColumnPush(int column,float delay) {
		if (!(column >= 0 && column < m_Columns)) return;
		for (int i = 0; i < m_Rows; i++) {
			if (m_Matrix[i][column] == NULL)  continue;
			m_Matrix[i][column]->PushAnimation(new AnimBlock_ColumnPush(0.3,delay));	
		}
	}

	void AnimateColumnPull(int column, float delay) {
		if (!(column >= 0 && column < m_Columns)) return;
		for (int i = 0; i < m_Rows; i++) { 
			if (m_Matrix[i][column] == NULL) continue;
			m_Matrix[i][column]->PushAnimation(new AnimBlock_ColumnPull(0.3, delay));
		}
	}

	std::vector<ActorAnimated*> PushToUnassigned(std::vector<ActorAnimated*> & items) {
		std::vector<ActorAnimated*> ret;
		for (size_t i = 0; i < items.size(); i++) {
			ActorAnimated * tmp = items[i]->Clone();
			m_Unassigned.push_back(tmp);
			ret.push_back(tmp);
		}
		return ret;
	}


private:
	std::list<ActorAnimated*> m_Unassigned;
	ActorAnimated *** m_Matrix;
	int m_Columns;
	int m_Rows;
};



const int max_rows = 12;
const int max_columns = 7;
const int viewport_width = 640;
const int viewport_height = 960;
ActorMatrix * gameMatrix = NULL;
Actor * cursorCharacter = NULL;
int cursorColumn = 0;
bool cursorAction = 0;
bool cursorActionRelease = false;
bool cursorActionCapture = false;
Core::Chronometer * addRowTimer = NULL;
std::vector<ActorAnimated*> cursorStack;
std::vector<Scene::Object*> possibleObjects;

ActorAnimated * InstanceActor(Graph::Device * dev, int i, int j) {
	// Add new row of balls..
	ActorAnimated * actorModel = new ActorAnimated(dev);

	int index = Math::RandomValue(0, 0.9999) * possibleObjects.size();

	actorModel->SetType(index);
	actorModel->SetModelObject(possibleObjects[index]);

	actorModel->GetObject()->SetPosition(gameMatrix->GetPosition(i, j));
	actorModel->GetObject()->Update();

	return actorModel;
}

void FinishColumnDrop(ActorMatrix * game, int column, int row, std::vector<ActorAnimated*> count) {
	ActorAnimated * ref = game->GetActor(column, row);

	int hitCount = 0;
	for (int i = row - 1; i > 0; i--) {
		ActorAnimated * other = game->GetActor(column, i);
		if (other->GetType() != ref->GetType()) {
			break;
		}
		else
			hitCount++;
	}

	// lets remove!
	if (hitCount + count.size() >= 3) {
		//std::vector<ActorAnimated*> tmp;
		int last_row = -1;
		for (int i = max_rows-1; i > 0; i--) {
			ActorAnimated * other = game->GetActor(column, i);
			if (other == NULL)
				continue;
			if (other->GetType() != ref->GetType()) 
				break;			
			else {
				
				std::vector<ActorAnimated *> tmp;
				tmp.push_back(other);
				tmp = game->PushToUnassigned(tmp);

				tmp[0]->PushAnimation(new AnimBlock_ColumnPush(0.3, 1.0));
				// Adicionar a coluna um column push
				/*other->GetObject()->SetPosition(gameMatrix->GetPosition(column, max_rows));
				other->GetObject()->Update();

				std::vector<ActorAnimated *> tmp;
				tmp.push_back(other);
				tmp = game->PushToUnassigned(tmp);
				
				tmp[0]->ClearAnimations();
				tmp[0]->PushAnimation(new AnimBlock_MoveToPos(1.0, 0, gameMatrix->GetPosition(column, i-1)));
				*/
				//tmp.push_back(other);

				game->SetActor(column, i, NULL);
				if (last_row < 0)
					last_row = i;
			}
		}

		int cc = 0;
		for (int i = last_row; i < max_rows && (i - last_row) < count.size(); i++) {
			ActorAnimated * other = count[i - last_row];
			other->GetObject()->SetPosition(gameMatrix->GetPosition(column, max_rows+ cc));
			other->GetObject()->Update();

			std::vector<ActorAnimated *> tmp;
			tmp.push_back(other);
			tmp = game->PushToUnassigned(tmp);

			tmp[0]->ClearAnimations();
			tmp[0]->PushAnimation(new AnimBlock_MoveToPos(1.0, 0, gameMatrix->GetPosition(column, i - 1)));
			cc++;
			//tmp.push_back(other);
		}

		//game->PushToUnassigned(tmp);
	}
}

Demo_Game::Demo_Game(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
    time = 0;
}

Demo_Game::~Demo_Game(){
	
}

#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof(*arr)) 

void Demo_Game::Load()
{
	addRowTimer = Core::CreateChronometer();
	addRowTimer->Start();

	std::string ball_textures[] = {
		"texture://tex2d_ball_blue_f.png",
		"texture://tex2d_ball_gold_f.png",
		"texture://tex2d_ball_red_f.png",
		"texture://tex2d_ball_green_f.png"
	};

	std::vector<Scene::Material*> materials;
	std::vector<Scene::Model*> models;


	for (size_t i = 0; i < ARRAY_COUNT(ball_textures); i++)
	{
		Graph::Texture * tex = dev->LoadTexture(ball_textures[i]);

		Scene::Material * sMaterial = new Scene::Material(dev);
		sMaterial->SetDiffuse(Math::Color4f(1, 1, 1, 1));

		Scene::Texture * sTexture = new Scene::Texture(dev);

		sTexture->SetTexture(tex);
		sMaterial->SetTextureLayer(0, new Scene::TextureLayer(sTexture, NULL, dev));

		materials.push_back(sMaterial);
	}

	dev->Enable(Graph::STATE_DEPTH_TEST);

	Graph::Mesh * ball_mesh = CreateSquareMesh(dev);
	Graph::Mesh * cursor_mesh = CreateTriangleMesh(dev);

	{
		Scene::Material * mat = new Scene::Material(dev); mat->SetDiffuse(Math::Color4f(1.0, 0.0, 0.0));
		std::vector<Scene::Material*> matVec;matVec.push_back(mat);
		Scene::Model * model = new Scene::Model(dev, cursor_mesh, matVec, Math::BoundBox(Math::Vec3(1, 1, 0), Math::Vec3(-1, -1, 0)));
		Scene::Object * obj = new Scene::Object(dev);
		obj->SetData(model);
		Actor * actor = new Actor(dev);
		actor->SetModelObject(obj);
		actor->GetObject()->SetScale(Math::Vec3(0.5, 0.5, 0.5));
		cursorCharacter = actor;
	}

	for (size_t i = 0; i < materials.size(); i++) {
		std::vector<Scene::Material*> mat_vec;
		mat_vec.push_back(materials[i]);

		Scene::Model * sModel = new Scene::Model(dev, ball_mesh, mat_vec, Math::BoundBox(Math::Vec3(1, 1, 0), Math::Vec3(-1, -1, 0)));
		models.push_back(sModel);

		Scene::Object * sObject = new Scene::Object(dev);
		sObject->SetData(sModel);

		possibleObjects.push_back(sObject);
	}

	gameMatrix = new ActorMatrix(max_columns, max_rows);

	for (int j = 0; j < max_rows/2; j++) {
		for (int i = 0; i < max_columns; i++) {
			gameMatrix->SetActor(i, j, InstanceActor(dev,i,j));
		}
	}

}

void Demo_Game::Render(float dt){
	dev->ClearColor(0.2, 0.2, 0.2);

	// Clear graphics rendering buffer.
	dev->Clear();

	float screen_aspect = viewport_width / (float)viewport_height;

	int screen_height = wnd->GetHeight();
	int screen_width = screen_height * screen_aspect;
	int screen_x = wnd->GetWidth()/2 - screen_width / 2;

	// Define the rendering area in window space.
	dev->Viewport(screen_x,0,screen_width, screen_height);

	// BG
	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();
	dev->Ortho2D(screen_width, screen_height);
	dev->MatrixMode(Graph::MATRIX_VIEW);
	dev->Identity();
	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();

	dev->Disable(Graph::STATE_DEPTH_TEST);
	dev->Disable(Graph::STATE_ZBUFFER_WRITE);

	dev->Begin(Graph::PRIMITIVE_QUADS);
	dev->Color(100, 100, 100);
	dev->Vertex(0, 0, 0);
	dev->Vertex(0, screen_height, 0);
	dev->Vertex(screen_width, screen_height, 0);
	dev->Vertex(screen_width, 0, 0);
	dev->End();




	// Set projection matrix.
	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();

	Scene::Camera cam(dev);
	cam.SetAspect(screen_aspect);
	cam.SetFov(49.1);
	Scene::Object cam_obj(dev);
	cam_obj.SetPosition(Math::Vec3(6, -11, 28));
	cam.SetObject(&cam_obj);

	cam.Enable(Graph::MATRIX_PROJECTION);

    
    // Set the view (camera) matrix.
    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();
   
	cam.Enable(Graph::MATRIX_VIEW);

	// Set the model matrix.
	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();
    
	dev->Enable(Graph::STATE_BLEND);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

	dev->Color(255, 255, 255);


	gameMatrix->Render();
		

	dev->PushMatrix();
	{
		Math::Vec3 p = gameMatrix->GetPosition(cursorColumn, max_rows-1);
		cursorCharacter->GetObject()->SetPosition(p);
		cursorCharacter->GetObject()->Update();
		cursorCharacter->Render();
	}
	dev->PopMatrix();
	

    time += dt;

	// Finish rendering and present the graphics.
	dev->PresentAll();

	gameMatrix->UpdatePhysics(dt);

	// Game Logic
	if (!gameMatrix->HasAnimation()) {
		//floatingActors.clear();
		if (addRowTimer->GetElapsedTime() > 10e6) {
			gameMatrix->AnimateDown();

			for (int i = 0; i < max_columns; i++) {
				ActorAnimated * newActor = InstanceActor(dev, i, -1);
				newActor->PushAnimation(new AnimBlock_MoveToPos(0.3, 0, gameMatrix->GetPosition(i, 0)));
				gameMatrix->SetActor(i, 0, newActor);
			}
			addRowTimer->Stop();
			addRowTimer->Start();
		}
		else if (cursorActionCapture) {
			cursorActionCapture = false;
			const int active_col = cursorColumn;
			ActorAnimated * ref = dynamic_cast<ActorAnimated*>((!cursorStack.empty()) ? cursorStack[0] : NULL);
			std::vector<ActorAnimated*> tempActors;
			for (int i = max_rows-1; i >= 0; i--) {
				ActorAnimated * tmp = dynamic_cast<ActorAnimated*>(gameMatrix->GetActor(active_col, i));
				if (tmp == NULL)
					continue;
				if (ref == NULL || ref->GetType() == tmp->GetType()) {
					ref = tmp;
					tempActors.push_back(tmp);
					gameMatrix->SetActor(active_col, i, NULL);
				}
				else
					break;
			}				

			std::vector<ActorAnimated*> tmp = gameMatrix->PushToUnassigned(tempActors);
			for (size_t i = 0; i < tmp.size(); i++) {
				tmp[i]->ClearAnimations();
				tmp[i]->PushAnimation(new AnimBlock_MoveToPos(0.3, 0.0, gameMatrix->GetPosition(active_col, max_rows)));
			}


			cursorStack.insert(cursorStack.end(), tempActors.begin(), tempActors.end());
			gameMatrix->AnimateColumnPull(active_col, 0.0);
		}
		else if (cursorActionRelease) {
			cursorActionRelease = false;
			const int active_col = cursorColumn;
			if (!cursorStack.empty()) {
				for (int i = max_rows - 1; i >= 0; i--) {
					ActorAnimated * tmp = dynamic_cast<ActorAnimated*>(gameMatrix->GetActor(active_col, i));
					
					if (i > 0) {
						if (tmp == NULL)
							continue;
						i++;
					}
					else {
						if(tmp!=NULL)
							i++;
					}
					gameMatrix->AnimateColumnPush(active_col, 0.3);
					
					for (int j = 0; j < cursorStack.size() && j + i < max_rows; j++) {
						int currentRow = j + i;
						ActorAnimated * actor = dynamic_cast<ActorAnimated*>(cursorStack[j]);
						actor->ClearAnimations();

						actor->GetObject()->SetPosition(gameMatrix->GetPosition(active_col, max_rows));
						actor->GetObject()->Update();

						actor->PushAnimation(new AnimBlock_MoveToPos(0.3,0.0, gameMatrix->GetPosition(active_col, currentRow)));
						actor->PushAnimation(new AnimBlock_ColumnPush(0.3, 0.0));

						gameMatrix->SetActor(active_col, currentRow, actor);
					}
					
					FinishColumnDrop(gameMatrix, active_col, i, cursorStack);

					cursorStack.clear();
					break;
				}
			}
		}

		
	}


}

void Demo_Game::UpdateWndEvents(){
	static bool wasLeftUp = false;
	static bool wasRightUp = false;
	static bool wasSpaceUp = false;
	static bool wasZUp = false;
	static bool wasXUp = false;

	if (wnd->GetKeyStatus(Core::KeyboardButton::BUTTON_LEFT) == Core::BUTTON_STATUS_DOWN) {
		if (wasLeftUp) cursorColumn = cursorColumn > 0 ? cursorColumn - 1 : 0;
		wasLeftUp = false;
	}
	else 
		wasLeftUp = true;
	

	if (wnd->GetKeyStatus(Core::KeyboardButton::BUTTON_RIGHT) == Core::BUTTON_STATUS_DOWN) {
		if (wasRightUp) cursorColumn = cursorColumn < max_columns-1? cursorColumn + 1: max_columns-1;
		wasRightUp = false;
	}
	else 
		wasRightUp = true;

	if (wnd->GetKeyStatus(Core::KeyboardButton::BUTTON_SPACE) == Core::BUTTON_STATUS_DOWN) {
		if (wasSpaceUp) cursorAction = true;
		wasSpaceUp = false;
	}
	else {
		wasSpaceUp = true;
	}

	if (wnd->GetKeyStatus(Core::KeyboardButton::BUTTON_Z) == Core::BUTTON_STATUS_DOWN) {
		if (wasZUp) cursorActionCapture = true;
		wasZUp = false;
	}
	else {
		wasZUp = true;
	}

	if (wnd->GetKeyStatus(Core::KeyboardButton::BUTTON_X) == Core::BUTTON_STATUS_DOWN) {
		if (wasXUp) cursorActionRelease = true;
		wasXUp = false;
	}
	else {
		wasXUp = true;
	}
	
}

std::vector<std::string> Demo_Game::GetKeywords() {
    std::vector<std::string> ret;
    ret.push_back("Basic");
    ret.push_back("Rendering");
    return ret;
}

std::string Demo_Game::GetDescription() {
    return "Triangles Rendering";
}

Demo * CreateDemo_Game(Core::Window * wnd, Graph::Device * dev){
	return new Demo_Game(wnd,dev);
}
