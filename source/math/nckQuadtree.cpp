
/**
 * NCtoolKit © 2007-2015 Lu’s F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#include "nckQuadtree.h"

_MATH_BEGIN
 
Quadtree_Leaf::Quadtree_Leaf(){
	m_Parent = NULL;
	for(int i = 0;i<6;i++)
		m_Neighbours[i] = NULL;
}

Quadtree_Leaf::Quadtree_Leaf(int id, Quadtree_Node * parent){
	m_Parent = parent;
	for(int i = 0;i<7;i++)
		m_Neighbours[i] = NULL;
	m_Id = id;
}

Quadtree_Leaf::~Quadtree_Leaf(){
	m_Parent = NULL;
	for(int i = 0;i < 7;i++)
		m_Neighbours[i] = NULL;
}

void Quadtree_Leaf::SetParent(Quadtree_Node * parent){
	m_Parent = parent;
}

Quadtree_Node * Quadtree_Leaf::GetParent(){
	return m_Parent;
}

void Quadtree_Leaf::SetNeighbour(int i, Quadtree_Leaf * leaf){
	if(i>=0 && i < 7)
		m_Neighbours[i] = leaf;
}

Quadtree_Leaf * Quadtree_Leaf::GetNeighbours(int i){
	if(i>=0 && i < 7)
		return m_Neighbours[i];
	return NULL;
}

Quadtree_Node::Quadtree_Node(){
	for(int i = 0;i<4;i++)
		m_Children[i] = NULL;
	m_Leaf = NULL;
}
	
Quadtree_Node::Quadtree_Node(const BoundBox & bb){
	m_BoundBox = bb;
	for(int i = 0;i<4;i++)
		m_Children[i] = NULL;
	m_Leaf = NULL;
}
	
Quadtree_Node::~Quadtree_Node(){
	for(int i = 0;i<4;i++){
		if(m_Children[i]){
			delete m_Children[i];
			m_Children[i] = NULL;
		}
	}
	if(m_Leaf){
		delete m_Leaf;
		m_Leaf = NULL;
	}
}

void Quadtree_Node::Build(const BoundBox & bb, int maxDepth, Quadtree_Leaf ** foundLeaf,		
	Quadtree_Node * nodeAllocator(const BoundBox & bb))
{
	int currentDepth = (maxDepth & 0xFFFF0000) >> 16;
	int tmpMaxDepth = maxDepth & 0x0000FFFF;

	if(currentDepth + 1 < tmpMaxDepth){
		for(int i = 0;i<4;i++)
		{
			BoundBox childBB;
			
			if(m_Children[i]){
				childBB = m_Children[i]->m_BoundBox;
			}else{
				childBB = ComputeBoundBox(i);
			}

			if(Math::Inside(childBB,bb.GetMax()) && Math::Inside(childBB,bb.GetMin())){
				if(!m_Children[i])
					m_Children[i] = nodeAllocator(childBB);

				m_Children[i]->Build(bb,(maxDepth & 0x0000FFFF) |((currentDepth+1) << 16),
					foundLeaf,
					nodeAllocator);

				if(foundLeaf && *foundLeaf)
					return;
			}
		}
	}
	
	if(!m_Leaf){
		m_Leaf = CreateLeaf();
	}

	if(foundLeaf)
		*foundLeaf = m_Leaf;
}

BoundBox Quadtree_Node::ComputeBoundBox(int id){
	Math::Vec3 pA,pB;
	Math::Vec3 nA,nB;

	pA = m_BoundBox.GetMax();
	pB = m_BoundBox.GetMin();

	if(id == 0){
		nA = pA;
		nB = Math::Vec3( (pA.GetX()+pB.GetX())*0.5f,(pA.GetY()+pB.GetY())*0.5f,pB.GetZ() );
	}else if(id == 1){
		nA = Math::Vec3( (pA.GetX()+pB.GetX())*0.5f,(pA.GetY()+pB.GetY())*0.5f,pA.GetZ() );
		nB = pB;
	}else if(id == 2){
		nA = Math::Vec3( (pA.GetX()+pB.GetX())*0.5f,pA.GetY(),pA.GetZ() );
		nB = Math::Vec3( pB.GetX(),(pA.GetY()+pB.GetY())*0.5f,pB.GetZ() );
	}else if(id == 3){
		nA = Math::Vec3( pA.GetX() ,(pA.GetY() + pB.GetY())*0.5f,pA.GetZ() );
		nB = Math::Vec3( (pA.GetX() + pB.GetX())*0.5f ,pB.GetY(),pB.GetZ() );
	}

	return Math::BoundBox(nA,nB);
}

Quadtree_Leaf * Quadtree_Node::GetLeaf(){
	return m_Leaf;
}

_MATH_END
