
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* This is a sample used to study 2d physics simulation for pinball games. Although not being 
* finished it does provided a nice case study to fix bugs, add new methods and learn new 
* things.
*/

#include "nckDemo_10.h"

_PBDEMO_BEGIN
		
void PrintVec(const Math::Vec3 & v){
	Core::DebugLog(Math::FloatToString(v.GetX(),6)+","+
		Math::FloatToString(v.GetY(),6));
}

void RenderLine(Graph::Device * dev, const Math::Line & line){
	dev->Begin(Graph::PRIMITIVE_LINES);
	Math::Vec3 v1 = line.GetStart();
	Math::Vec3 v2 = line.GetEnd();
	dev->Vertex(v1.GetX(),v1.GetY(),v1.GetZ());
	dev->Vertex(v2.GetX(),v2.GetY(),v2.GetZ());
	dev->End();
}

void RenderDot(Graph::Device * dev,Math::Vec3 p,float size){
	dev->Begin(Graph::PRIMITIVE_QUADS);
	dev->Vertex(p.GetX()-size,p.GetY()+size,p.GetZ());
	dev->Vertex(p.GetX()-size,p.GetY()-size,p.GetZ());
	dev->Vertex(p.GetX()+size,p.GetY()-size,p.GetZ());
	dev->Vertex(p.GetX()+size,p.GetY()+size,p.GetZ());
	dev->End();
}

void RenderCircle(Graph::Device * dev, const Math::Vec3 & p, float radius){
	dev->PushMatrix();
	dev->Translate(p.GetX(),p.GetY(),0);
	dev->Begin(Graph::PRIMITIVE_LINES);
	int divisions = 32;
	for(float i = 0,a =0; i <= divisions;i++,a+= M_PI*2/divisions){
		if(i>0){
			dev->Vertex(cos(a)*radius,sin(a)*radius,0);
			dev->Vertex(cos(a- M_PI*2/divisions)*radius,sin(a- M_PI*2/divisions)*radius,0);
		}
	}
	dev->End();
	dev->PopMatrix();
}

World::World(){
	m_Gravity = Math::Vec3(0,0,-9.8f);
}

World::World(const Math::Vec3 & g){
	m_Gravity = g;
}

World::~World(){}

void World::setGravity(const Math::Vec3 & g){
	m_Gravity = g;
}

Math::Vec3 World::GetGravity() const{
	return m_Gravity;
}

Object::Object(World * world){
	m_Mass = 0;
	m_Radius = 1;
	m_World = world;
}

Object::~Object(){}

Math::Vec3 Object::GetLinearPosition() const{
	return m_Position;
}

void Object::SetLinearPosition(const Math::Vec3 & pos){
	m_Position = pos;
}

Math::Vec3 Object::GetLinearVelocity() const{
	return m_LinearVelocity;
}

void Object::SetLinearVelocity(const Math::Vec3 & pos){
	m_LinearVelocity = pos;
}

Math::Vec3 Object::GetLinearAcceleration() const{
	return m_LinearAcceleration;
}

void Object::SetLinearAcceleration(const Math::Vec3 & pos){
	m_LinearAcceleration = pos;
}

void Object::LinearStep(float step){
	m_LinearVelocity += (m_LinearAcceleration + m_World->GetGravity())* step;
	// Euler Integrator
	m_Position += m_LinearVelocity*step; 
}

void Object::SetRadius(float radius){
	m_Radius = radius;
}

float Object::GetRadius() const{
	return m_Radius;
}

void Object::SetMass(float mass){
	m_Mass = mass;
}

float Object::GetMass() const{
	return m_Mass;
}

Edge::Edge(bX::VertexIterator a, bX::VertexIterator b,bX::FaceIterator end){
	va = a;
	vb = b;
	fa = end;
	fb = end;
}

Edge::~Edge(){}

bool Edge::Compare(const Edge & edge){
	if( ((*va)->m_Id == (*edge.va)->m_Id && (*vb)->m_Id == (*edge.vb)->m_Id) ||
		((*va)->m_Id == (*edge.vb)->m_Id && (*vb)->m_Id == (*edge.va)->m_Id) ){
			return true;
	}
	return false;
}

int ComputeMeshNonManifold(bX::Mesh * mesh,const Math::Mat44 & transform, std::list<Math::Line> * lines)
{
	if(mesh && lines)
	{
		std::list<Edge*> allEdges;
		std::list<std::list<Edge*> *> edgeListsPerVert;

		ListFor(bX::Face*,mesh->m_Faces,i)
		{
			for(int j = 0;j<(*i)->m_Verts.size();j++)
			{
				bX::VertexIterator v1 = (*i)->m_Verts[j];

				std::list<Edge*> *el1 = (std::list<Edge*>*)(*v1)->m_Auxiliar;
				if(el1 == NULL){
					el1 = new std::list<Edge*>();
					edgeListsPerVert.push_back(el1);
					(*v1)->m_Auxiliar = (bX::Auxiliar*)el1;
				}


				bX::VertexIterator v2;
				if(j < (*i)->m_Verts.size()-1)
					v2 = (*i)->m_Verts[j+1];
				else
					v2 = (*i)->m_Verts[0];

				std::list<Edge*> *el2 = (std::list<Edge*>*)(*v2)->m_Auxiliar;
				if(el2 == NULL){
					el2 = new std::list<Edge*>();
					edgeListsPerVert.push_back(el2);
					(*v2)->m_Auxiliar = (bX::Auxiliar*)el2;
				}


				Edge * edge = new Edge(v1,v2,mesh->m_Faces.end());
				edge->fa = i;

				bool used = false;

				std::list<Edge*>::iterator found;

				found = el1->end(); 

				ListFor(Edge*,(*el1),k){
					if((*k)->Compare(*edge)){
						found = k;
						break;
					}
				}

				if(found == el1->end()){
					used = true;
					el1->push_back(edge);
				}			
				else
					(*found)->fb = i;

				found = el2->end(); 

				ListFor(Edge*,(*el2),k){
					if((*k)->Compare(*edge)){
						found = k;
						break;
					}
				}

				if(found == el2->end()){
					used = true;
					el2->push_back(edge);
				}
				else
					(*found)->fb = i;

				if(!used)
					delete edge;
				else
					allEdges.push_back(edge);	
			}
		}

		ListFor(std::list<Edge*>*,edgeListsPerVert,i){
			delete (*i);
		}

		// Remove edges with two faces.
		ListFor(Edge*,allEdges,i){
			if((*i)->fb == mesh->m_Faces.end()){
				lines->push_back(Math::Line(bXToVec3((*(*i)->va)->m_Pos),bXToVec3((*(*i)->vb)->m_Pos)));
			}
			delete (*i);
		}

		ListFor(bX::Vertex*,mesh->m_Vertices,i){
			(*i)->m_Auxiliar = NULL;
		}
	}

	return -1;
}

int ConvertMeshToTris(bX::Mesh * mesh,const Math::Mat44 & transform, std::list<Math::Triangle> * triangles){
	if(mesh && triangles){
		ListFor(bX::Face*,mesh->m_Faces,i){
			if((*i)->m_Verts.size()==3){
				Math::Triangle t(bXToVec3((*(*i)->m_Verts[0])->m_Pos),
					bXToVec3((*(*i)->m_Verts[1])->m_Pos),
					bXToVec3((*(*i)->m_Verts[2])->m_Pos));
				triangles->push_back(t);
			}else if((*i)->m_Verts.size()==4){
				Math::Triangle t1(bXToVec3((*(*i)->m_Verts[0])->m_Pos),
					bXToVec3((*(*i)->m_Verts[1])->m_Pos),
					bXToVec3((*(*i)->m_Verts[3])->m_Pos));
				triangles->push_back(t1);

				Math::Triangle t2(bXToVec3((*(*i)->m_Verts[1])->m_Pos),
					bXToVec3((*(*i)->m_Verts[2])->m_Pos),
					bXToVec3((*(*i)->m_Verts[3])->m_Pos));
				triangles->push_back(t2);
			}
		}
	}
	return -1;
}


Collision::Collision(){
	m_Distance = 0;
}

Math::Line Collision::getWall() const{
	return m_Limit;
}

Math::Vec3 Collision::getIntersectionPoint() const{
	return m_Intersection;
}

Math::Vec3 Collision::getprevTOIPoint() const{
	return m_prevTOIPoint;
}

double Collision::getDistance() const{
	return m_Distance;
}

bool Collision::Compare(const Collision & col){
	if(col.getDistance() < m_Distance)
		return true;
	return false;
}

bool Collision::BelongsToLine(const Math::Line & line, const Math::Vec3 & p){
	Math::Vec3 res;
	float vec;
	return Math::NearestPoint(line,p,&res,&vec);
}

bool Collision::TestCollision(const Math::Line & limit, const Math::Line & movement, float radius,Collision * col){
	Math::Vec3 intPoint;
	Math::Vec3 prevTOIPoint;

	// Extend limit to "infinite"
	Math::Line extLimit = Math::Line(
		limit.GetStart()-limit.GetDirection()*10,
		limit.GetEnd()+limit.GetDirection()*10);

	// Extend movement to "infinite"
	//Math::Line extMovement = Math::Line(
	//	movement.GetStart(),
	//	movement.GetEnd()+movement.GetDirection()*10);

	// Edge-Edge-Circle Colision - Path intersection test with a circle shape.
	bool didColide = false;

	bool tmpColide = NearestPointToIntersection(extLimit,movement,
		radius,&intPoint,&prevTOIPoint);

	if(tmpColide){
		float dist1 = (intPoint - movement.GetStart()).Length();
		float dist2 = movement.Length();

		Math::Vec3 tmpVec;
		float tmpDist=0;
		float tmpDist2=0;

		bool toi = Math::NearestPoint(limit,prevTOIPoint,&tmpVec,&tmpDist2);

		if((dist1 < dist2 || ((dist1 - BALL_DIAMETER) < dist2 && (dist1 - BALL_DIAMETER)>0)) &&
			(Math::NearestPoint(limit,intPoint,&tmpVec,&tmpDist) ||
			toi) && tmpDist2<BALL_DIAMETER){
				didColide = true;
		}
	}

	if(didColide){
		col->m_Distance = (intPoint-movement.GetStart()).Length();
		col->m_prevTOIPoint = prevTOIPoint;
		col->m_Intersection = intPoint;
		col->m_Limit = limit;
	}

	return didColide;
}


QtLeaf::QtLeaf() : Math::Quadtree_Leaf() {}
QtLeaf::QtLeaf(Math::Quadtree_Node * parent) : Math::Quadtree_Leaf(-1,parent) {}
QtLeaf::~QtLeaf(){}

QtNode::QtNode() : Math::Quadtree_Node(){}
QtNode::QtNode(const Math::BoundBox & bb) : Math::Quadtree_Node(bb){}

void QtNode::Render(Graph::Device * dev){
	for(int i =0;i<4;i++){
		if(m_Children[i])
			((QtNode*)m_Children[i])->Render(dev);
	}

	Math::BoundBox bb = m_BoundBox;

	RenderLine(dev,Math::Line(bb.GetMax(),Math::Vec3(bb.GetMax().GetX(),bb.GetMin().GetY(),bb.GetMax().GetZ())));
	RenderLine(dev,Math::Line(Math::Vec3(bb.GetMax().GetX(),bb.GetMin().GetY(),bb.GetMax().GetZ()),bb.GetMin()));
	RenderLine(dev,Math::Line(bb.GetMin(),Math::Vec3(bb.GetMin().GetX(),bb.GetMax().GetY(),bb.GetMax().GetZ())));
	RenderLine(dev,Math::Line(Math::Vec3(bb.GetMin().GetX(),bb.GetMax().GetY(),bb.GetMax().GetZ()),bb.GetMax()));
}

void QtNode::Intersect(const Math::BoundBox & bb, std::list<Math::Line> * intersectedLines)
{
	for(int n = 0;n < 4;n++){
		if(m_Children[n]){
			QtNode * node = (QtNode*)m_Children[n];
			bool inside = Math::Intersect(bb,node->m_BoundBox);
			if(inside)
				node->Intersect(bb,intersectedLines);
		}
	}

	if(GetLeaf()){
		QtLeaf * l = (QtLeaf*)GetLeaf();
		ListFor(Math::Line,l->lines,i){
			if(intersectedLines){
				Math::BoundBox tmp;
				tmp.Insert(i->GetStart());
				tmp.Insert(i->GetEnd());
				if(Math::Intersect(bb,tmp))		
					intersectedLines->push_back(*i);
			}
		}
	}

}

Math::Quadtree_Leaf * QtNode::CreateLeaf(){
	return new QtLeaf(this);
}

void QtNode::Insert(const Math::Line & line){
	Math::BoundBox bb;
	bb.Insert(line.GetStart());
	bb.Insert(line.GetEnd());

	Math::Quadtree_Leaf * leaf = NULL;
	QtNode::Build(bb,16,&leaf,nodeAllocator);

	if(leaf)
		((QtLeaf*)leaf)->lines.push_back(line);
}

Math::Quadtree_Node * QtNode::nodeAllocator(const Math::BoundBox & bb){
	return new QtNode(bb);
}

Simulation::Simulation(){
	world = new PbDemo::World(Math::Vec3(0,-sin(MACHINE_INCLINATION*M_PI/180.0)*EARTH_GRAVITY,0));
	ball = new PbDemo::Object(world);
	ball->SetRadius(BALL_DIAMETER/2.0);
	ball->SetLinearPosition(Math::Vec3(0,0,0));
	ball->SetLinearVelocity(Math::Vec3(Math::RandomValue(-1,1),Math::RandomValue(-1,1),0)*2);
}

Simulation::~Simulation(){
	SafeDelete(ball);
	SafeDelete(world);
	SafeDelete(node);
}

void Simulation::Load(){
	bX::File * pinballFile = bX::OpenFileStream(Core::ResolveFilename("model://demo_pinball_test.bx"));
	if(pinballFile){
		bX::Scene * scene = new bX::Scene();
		bool success = scene->Read(pinballFile);
		if(success){
			bX::Mesh * mesh = scene->GetMesh("machine");
			if(mesh){
				Math::Mat44 mat;
				PbDemo::ComputeMeshNonManifold(mesh,mat,&walls);

				Math::BoundBox bb;
				ListFor(Math::Line, walls,i){
					bb.Insert(i->GetStart());
					bb.Insert(i->GetEnd());
				}

				node = new PbDemo::QtNode(bb);
				ListFor(Math::Line, walls,i){
					node->Insert(*i);
				}
			}
		}
		SafeDelete(scene);
	}
	SafeDelete(pinballFile);
}

bool Simulation::Simulate(float dt, int depth)
{
	if(ball->GetLinearVelocity().Length()>5)
		ball->SetLinearVelocity(Math::Normalize(ball->GetLinearVelocity())*5.0);

	Math::Vec3 oldPosition = ball->GetLinearPosition();
	Math::Vec3 oldVelocity = ball->GetLinearVelocity();


	ball->LinearStep(dt);

	if(depth == 8) // Stop at 8 recursive calls
		return true;

	Math::Line desl(oldPosition,ball->GetLinearPosition());

	Math::BoundBox deslBB;

	deslBB.Insert(desl.GetStart());
	deslBB.Insert(desl.GetEnd());

	Math::Vec3 bbNorm = Math::Vec3(0.707,0.707,0);
	deslBB.Insert(deslBB.GetMax()+bbNorm*ball->GetRadius());
	deslBB.Insert(deslBB.GetMin()-bbNorm*ball->GetRadius());

		
	// Broadphase Collision Detection
	std::list<Math::Line> intersectedLines;
	node->Intersect(deslBB,&intersectedLines);

	bool didColide = false;
	PbDemo::Collision colision;

	// Narrowphase Collision detection
	{
		double subStep = 0;
		const int maxIter = 10;
		float a = 0.0;
		float b = 1.0;

		const Math::Vec3 nor = desl.GetEnd()-desl.GetStart();
		PbDemo::Collision newCollision;

		// Start the sweep
		for(int i = 0 ; i < maxIter ; i++)
		{
			Math::Vec3 tmp = desl.GetStart() + nor * (a+b)*0.5;

			// First step will detect any collision in the ball path 
			if(i == 0) 
				tmp = desl.GetEnd();

			Math::Line newDesl = Math::Line(desl.GetStart(),tmp);

			ListFor(Math::Line,intersectedLines,k){
				PbDemo::Collision tmp;
				if( PbDemo::Collision::TestCollision(*k,newDesl,ball->GetRadius(),&tmp)){
					if(!didColide){
						newCollision = tmp;
					}else if(newCollision.Compare(tmp)){
						newCollision = tmp;
					}
					didColide = true;
				}
			}

			if(didColide){
				colision = newCollision;
				prevIntersectPoint = colision.getIntersectionPoint();
				prevPoint = oldPosition;
				prevTOIPoint = colision.getprevTOIPoint();
				subStep = ((double)(colision.getprevTOIPoint()-oldPosition).Length()) / (double)ball->GetLinearVelocity().Length();
				b = (a+b)*0.5;
			}else{
				a = (a+b)*0.5;
			}
		}

		if(didColide){
			//Core::DebugLog("pos=(");PrintVec(oldPosition);Core::DebugLog(")\n");
			//Core::DebugLog("vel=(");PrintVec(oldVelocity);Core::DebugLog(")\n");

			ball->SetLinearPosition(oldPosition);
			ball->SetLinearVelocity(oldVelocity);

			Math::Vec3 wNor = Math::Normalize(colision.getWall().GetStart()-colision.getWall().GetEnd());
			Math::Vec3 nor = Math::Cross(wNor,Math::Vec3(0,0,1));

			ball->LinearStep(subStep);

			Math::Vec3 newSpeed = - Math::Reflect(nor,ball->GetLinearVelocity());
			ball->SetLinearVelocity(newSpeed);
			ball->SetLinearPosition(colision.getprevTOIPoint());

			prevExitVector = ball->GetLinearVelocity();

			if(subStep<dt && subStep>0)
				return Simulate(dt-subStep,++depth);
			else
				Core::DebugLog("Ops!\n");
		}

	}

	return false;
}

void Simulation::Reset(){
	ball->SetLinearPosition(Math::Vec3(0,0,0));
}

void Simulation::Render(Graph::Device * dev){
	dev->Color(255,0,0,50);
	node->Render(dev);

	dev->Color(255,255,255);
	ListFor(Math::Line,walls,i){
		PbDemo::RenderLine(dev,(*i));
	}
	PbDemo::RenderCircle(dev,ball->GetLinearPosition(),ball->GetRadius());
}

_PBDEMO_END
	
Demo10::Demo10(Core::Window * wnd, Graph::Device * dev){
	this->dev = dev;
	this->wnd = wnd;
	fntTex = NULL;
	fntMap = NULL;
	sim = NULL;
}

Demo10::~Demo10(){
	SafeDelete(sim);
	SafeDelete(fntTex);
	SafeDelete(fntMap);
}

void Demo10::Load(){
	dev->Enable(Graph::STATE_BLEND);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA,Graph::BLEND_INV_SRC_ALPHA);

	fntMap = new Gui::FontMap(dev);
	fntMap->Load("script://font_ubuntu.txt");
	fntMap->SetPositionAccuracy(Gui::FONT_POSITION_ACCURACY_INTEGER);

	fntTex = dev->LoadTexture("texture://tex2d_font_ubuntu.png");
	fntTex->SetFilter(Graph::FILTER_MIPMAPPING,Graph::FILTER_NEAREST);
	fntTex->SetFilter(Graph::FILTER_MAGNIFICATION,Graph::FILTER_NEAREST);
	fntTex->SetFilter(Graph::FILTER_MINIFICATION,Graph::FILTER_NEAREST);

	sim = new PbDemo::Simulation();
	sim->Load();
}

void Demo10::Render(float dt){
	dev->Clear();

	dev->Viewport(0,0,wnd->GetWidth(),wnd->GetHeight());

	float wScale = wnd->GetWidth()/(float)wnd->GetHeight();

	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();

	dev->Perspective(wScale,36.0f,0.2f,100.0f);

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();

	dev->Translate(0,0,-2);

	dev->Disable(Graph::STATE_DEPTH_TEST);

	sim->Simulate();

	dev->Color(0,0,0);
	PbDemo::RenderLine(dev,Math::Line(sim->prevTOIPoint,sim->ball->GetLinearPosition()));

	dev->Color(0,255,0);
	PbDemo::RenderLine(dev,Math::Line(sim->ball->GetLinearPosition(),
	sim->ball->GetLinearPosition()+sim->ball->GetLinearVelocity()/30.0));

	dev->Color(255,0,0);
	PbDemo::RenderDot(dev,sim->prevIntersectPoint);

	dev->Color(255,255,0);
	PbDemo::RenderDot(dev,sim->prevTOIPoint,0.005);

	dev->Color(255,255,0);
	PbDemo::RenderLine(dev,Math::Line(sim->prevTOIPoint+sim->prevExitVector/60.0f,sim->prevTOIPoint));

	dev->Color(0,0,255);
	PbDemo::RenderDot(dev,sim->prevPoint,0.005);

	sim->Render(dev);

	dev->MatrixMode(Graph::MATRIX_PROJECTION);
	dev->Identity();
	dev->Ortho2D(wnd->GetWidth(),wnd->GetHeight());

	dev->MatrixMode(Graph::MATRIX_MODEL);
	dev->Identity();

	dev->Color(0,0,0,255);

	fntTex->Enable();
	fntMap->Draw(10,10,16,"Press space to reset ball position");
	fntMap->Draw(10,24,16,"Press C to set ball state before last collision");
	fntTex->Disable();

	// Finish rendering and present the graphics.
	dev->PresentAll();
}

void Demo10::UpdateWndEvents(){
	static bool btnStatusSpace = false;
	static bool btnStatusC = false;

	if(!btnStatusSpace && wnd->GetKeyStatus(Core::BUTTON_SPACE) == Core::BUTTON_STATUS_DOWN)
		sim->Reset();		
	btnStatusSpace = wnd->GetKeyStatus(Core::BUTTON_SPACE) == Core::BUTTON_STATUS_DOWN;

	if(!btnStatusC && wnd->GetKeyStatus(Core::BUTTON_C) == Core::BUTTON_STATUS_DOWN){
		sim->ball->SetLinearPosition(sim->prevTOIPoint);
		sim->ball->SetLinearVelocity(sim->prevExitVector);
	}
	btnStatusC = wnd->GetKeyStatus(Core::BUTTON_C) == Core::BUTTON_STATUS_DOWN;
}

Demo * CreateDemo_10(Core::Window * wnd, Graph::Device * dev){
	return new Demo10(wnd,dev);
}
