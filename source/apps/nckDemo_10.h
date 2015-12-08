
/**
* NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
* https://github.com/nczeroshift/nctoolkit
*
* This is a sample used to study 2d physics simulation for pinball games. Although not being 
* finished it does provided a nice case study to fix bugs, add new methods and learn new 
* things.
*/

#ifndef _PINBALL_PHSYICS_DEMO_H_
#define _PINBALL_PHSYICS_DEMO_H_

#include "nckDemo.h"

#define _PBDEMO_BEGIN namespace PbDemo{
#define _PBDEMO_END }

// Pinball simulation constants
#define EARTH_GRAVITY		9.8f		// 9.4 m/s^2
#define BALL_WEIGHT			0.08f		// 80g
#define BALL_DIAMETER		0.022f		// 0.0254f - 2.54 cm
#define MACHINE_WIDTH		0.5588f		// 55.88 cm
#define MACHINE_HEIGHT		1.2954f		// 1.2954 m
#define MACHINE_INCLINATION	12.6f		// 12.6� = asin((47-35)/55)*180/pi

#define bXToVec3(v)	Math::Vec3(v.x,v.y,v.z)

_PBDEMO_BEGIN
	
// Simple line rendering
void RenderLine(Graph::Device * dev, const Math::Line & line);
// Simple squared dot rendering
void RenderDot(Graph::Device * dev,Math::Vec3 p,float size = 0.01);
// Simple circle rendering
void RenderCircle(Graph::Device * dev, const Math::Vec3 & p, float radius);

/**
* Physics world properties.
*/
class World{
public:
	World();
	World(const Math::Vec3 & g);
	~World();

	void setGravity(const Math::Vec3 & g);
	Math::Vec3 GetGravity() const;

protected:
	Math::Vec3 m_Gravity;
};

/**
* Physics object state class.
*/
class Object{
public:
	Object(World * world);
	~Object();

	Math::Vec3 GetLinearPosition() const;
	void SetLinearPosition(const Math::Vec3 & pos);

	Math::Vec3 GetLinearVelocity() const;
	void SetLinearVelocity(const Math::Vec3 & pos);

	Math::Vec3 GetLinearAcceleration() const;
	void SetLinearAcceleration(const Math::Vec3 & pos);

	void LinearStep(float step);

	void SetRadius(float radius);
	float GetRadius() const;

	void SetMass(float mass);
	float GetMass() const;

protected:
	Math::Vec3 m_AngularAcceleration;
	Math::Vec3 m_AngularVelocity;
	Math::Quat m_Rotation;

	Math::Vec3 m_LinearAcceleration;
	Math::Vec3 m_LinearVelocity;
	Math::Vec3 m_Position;

	float m_Mass;
	float m_Radius;

	Math::Mat44 m_Matrix;
	
	World * m_World;
};

/**
* Helper class used to list edges.
*/
class Edge{
public:
	Edge(bX::VertexIterator a, bX::VertexIterator b,bX::FaceIterator end);
	~Edge();

	bool Compare(const Edge & edge);

	bX::VertexIterator va,vb;
	bX::FaceIterator fa,fb;
};

// Compute non manifold edges(edges with only one assigned face) from bXporter mesh.
int ComputeMeshNonManifold(bX::Mesh * mesh,const Math::Mat44 & transform, std::list<Math::Line> * lines); 

// Compute triangulated mesh from bXporter mesh.
int ConvertMeshToTris(bX::Mesh * mesh,const Math::Mat44 & transform, std::list<Math::Triangle> * triangles);

/**
* Collision detection/prediction helper class.
*/
class Collision{
public:
	Collision();
	Math::Line getWall() const;
	Math::Vec3 getIntersectionPoint() const;
	Math::Vec3 getprevTOIPoint() const;
	double getDistance() const;
	bool Compare(const Collision & col);
	static bool BelongsToLine(const Math::Line & line, const Math::Vec3 & p);
	static bool TestCollision(const Math::Line & limit, const Math::Line & movement, float radius,Collision * col);

private:	
	// Collision boundry
	Math::Line m_Limit;

	// Collision boundry and path intersection point
	Math::Vec3 m_Intersection;

	// Time of Impact Point
	Math::Vec3 m_prevTOIPoint; 

	// Distance from path start point
	double m_Distance;
};

/**
* Custom quadtree leaf.
*/
class QtLeaf: public Math::Quadtree_Leaf{
public:
	QtLeaf();
	QtLeaf(Math::Quadtree_Node * parent);
	~QtLeaf();

	std::list<Math::Line> lines;
};

/**
* Custom quadtree node.
*/
class QtNode: public Math::Quadtree_Node{
public:
	QtNode();
	QtNode(const Math::BoundBox & bb);

	void Render(Graph::Device * dev);
	void Intersect(const Math::BoundBox & bb, std::list<Math::Line> * intersectedLines);
	Math::Quadtree_Leaf * CreateLeaf();
	void Insert(const Math::Line & line);
	static Quadtree_Node * nodeAllocator(const Math::BoundBox & bb);
};

/**
* Pinball main simulation class.
*/
class Simulation{
public:
	Simulation();
	~Simulation();

	void Load();
	bool Simulate(float dt = 1.0/60.0f, int depth = 0);
	void Reset();
	void Render(Graph::Device * dev);

	std::list<Math::Line> walls;
	World * world;
	QtNode * node;
	Object * ball;

	Math::Vec3 prevIntersectPoint;
	Math::Vec3 prevPoint;
	Math::Vec3 prevExitVector;
	Math::Vec3 prevTOIPoint;
};

_PBDEMO_END

/**
* Demo 10 class.
*/
class Demo10 : public Demo{
public:
	Demo10(Core::Window * wnd, Graph::Device * dev);
	~Demo10();

	void Load();
	void Render(float dt);
	void UpdateWndEvents();

	PbDemo::Simulation * sim;
	Graph::Texture * fntTex;
	Gui::FontMap * fntMap;
};

Demo * CreateDemo_10(Core::Window * wnd, Graph::Device * dev);

#endif
