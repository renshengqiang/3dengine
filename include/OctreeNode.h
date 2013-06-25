#ifndef _OCTREENODE_H
#define _OCTREENODE_H

#include <SceneNode.h>
#include <OctreeSceneManager.h>
#include <string>

/** Specialized SceneNode that is customized for working within an Octree. Each node
* maintains its own bounding box, rather than merging it with all the children.
*
*/
class  OctreeNode : public SceneNode
{
public:
	/** Standard constructor */
	OctreeNode( SceneManager* creator );
	/** Standard constructor */
	OctreeNode( SceneManager* creator, const std::string& name );
	/** Standard destructor */
	~OctreeNode();

	/** Returns the Octree in which this OctreeNode resides
	*/
	Octree * GetOctant()
	{
		return m_octant;
	};

	/** Sets the Octree in which this OctreeNode resides
	*/
	void SetOctant( Octree *o )
	{
		m_octant = o;
	};

	/** Determines if the center of this node is within the given box
	*/
	bool _IsIn( AxisAlignedBox &box );

protected:
	virtual OctreeNode *CreateChildImpl(const  std::string& name);

	/** Internal method for updating the bounds for this OctreeNode.
	@remarks
	This method determines the bounds solely from the attached objects, not
	any children. If the node has changed its bounds, it is removed from its
	current octree, and reinserted into the tree.
	*/
	virtual void _UpdateBounds( void );

protected:
	///who manages this node
	SceneManager *mp_creator;
	
	///Octree this node is attached to.
	Octree *m_octant;
};

#endif
