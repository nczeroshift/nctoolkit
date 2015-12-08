
/**
 * NCtoolKit © 2007-2015 Luís F.Loureiro, under zlib software license.
 * https://github.com/nczeroshift/nctoolkit
 */

#ifndef NCK_QUADTREE_H
#define NCK_QUADTREE_H

#include "nckBoundBox.h"
#include <list>
#include <vector>

_MATH_BEGIN

class Quadtree_Node;
	
/**
* Quadtree leaf abstract class. Implement the abstract methods on a
* subclass to complete the custom quadtree generation.
*/
class Quadtree_Leaf{
public:
	/**
	* Default constructor.
	*/ 
	Quadtree_Leaf();

	/**
	* Quadtree leaf constructor.
	* @param id Leaf sequencial id number.
	* @param parent Reference to parent node. 
	*/
	Quadtree_Leaf(int id, Quadtree_Node * parent);

	/**
	* Destructor.
	*/
	virtual ~Quadtree_Leaf();

	/**
	* Setter for parent node reference.
	*/
	void SetParent(Quadtree_Node * parent);
	
	/**
	* Getter for the parent node reference.
	*/
	Quadtree_Node * GetParent();

	/**
	* Set neighbour leaf reference, assuming it touches the parent leaf 
	* node boundbox, it may touch up to 8 leafs.
	* @param i Neighbour index, a number between 0 and 7.
	* @param leaf Neighbour Quadtree leaf reference.
	*/
	void SetNeighbour(int i, Quadtree_Leaf * leaf);

	/**
	* Get neighbour leaf reference.
	* @param i Neighbour index, a number between 0 and 7.
	*/
	Quadtree_Leaf * GetNeighbours(int i);
protected:
	int m_Id;
	Quadtree_Leaf * m_Neighbours[8];
	Quadtree_Node * m_Parent;
};


/**
* Quadtree node abstract class. Implement the abstract methods on a
* subclass to complete the custom quadtree generation.
*/
class Quadtree_Node{
public:
	/**
	* Default constructor.
	*/
	Quadtree_Node();
	
	/**
	* Constructor with boundbox.
	* @param bb Boundbox.
	*/
	Quadtree_Node(const BoundBox & bb);
	
	virtual ~Quadtree_Node();

	/**
	* Get node boundbox.
	*/
	inline Math::BoundBox GetBoundbox() const {
		return m_BoundBox;
	}

	/**
	* Construct quadtree by "inserting" a boundbox through the datastructre.
	* @param bb Object boundbox to be inserted.
	* @param maxDepth Maximum depth when constructing the quadtree.
	* @param foundLeaf Reference to the quadtree leaf.
	*/
	void Build(const BoundBox & bb, int maxDepth, Quadtree_Leaf ** foundLeaf, 
		Quadtree_Node * nodeAllocator(const BoundBox & bb));

	/**
	* Return reference to leaf.
	*/
	Quadtree_Leaf * GetLeaf();

protected:

	/**
	* Abstract method to be implemented when creating the leafs.
	*/
	virtual Quadtree_Leaf * CreateLeaf() = 0;

	/**
	* Compute boundbox for children id.
	* @param id Number between 0 and 3 for each child.
	*/
	BoundBox ComputeBoundBox(int id);

	/**
	* Node boundbox.
	*/
	BoundBox m_BoundBox;
	
	/**
	* Reference to each node children nodes.
	*/
	Quadtree_Node * m_Children[4];
	
	/**
	* Reference to node leaf.
	*/
	Quadtree_Leaf * m_Leaf;
};

_MATH_END

#endif // #ifndef NCK_QUADTREE_H
