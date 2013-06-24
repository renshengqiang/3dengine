#ifndef _OCTREENODE_H
#define _OCTREENODE_H

#include <SceneNode.h>
#include "OctreeSceneManager.h"
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

    /** Overridden from Node to remove any reference to octants */
    Node * removeChild( unsigned short index );
    
    /** Overridden from Node to remove any reference to octants */
    Node * removeChild( const std::string & name );

    /** Overridden from Node to remove any reference to octants */
    Node * removeChild( Node* child);

	/** Overridden from Node to remove any reference to octants */
	void removeAllChildren(void);

    /** Returns the Octree in which this OctreeNode resides
    */
    Octree * getOctant()
    {
        return mOctant;
    };

    /** Sets the Octree in which this OctreeNode resides
    */
    void setOctant( Octree *o )
    {
        mOctant = o;
    };

    /** Determines if the center of this node is within the given box
    */
    bool _isIn( AxisAlignedBox &box );

    /** Adds all the attached scenenodes to the render queue
    */
/*
    virtual void _addToRenderQueue( Camera* cam, RenderQueue * q, bool onlyShadowCasters, 
		VisibleObjectsBoundsInfo* visibleBounds);
*/
    /** Returns the local bounding box of this OctreeNode.
    @remarks
    This is used to render the bounding box, rather then the global.
    */
    AxisAlignedBox & _getLocalAABB()
    {
        return mLocalAABB;
    };

protected:

    /** Internal method for updating the bounds for this OctreeNode.
    @remarks
    This method determines the bounds solely from the attached objects, not
    any children. If the node has changed its bounds, it is removed from its
    current octree, and reinserted into the tree.
    */
    void _updateBounds( void );

    void _removeNodeAndChildren( );

    SceneManager *mp_creator;
    ///local bounding box
    AxisAlignedBox mLocalAABB;

    ///Octree this node is attached to.
    Octree *mOctant;

    ///preallocated corners for rendering
    float mCorners[ 24 ];
    ///shared colors for rendering
    static unsigned long mColors[ 8 ];
    ///shared indexes for rendering
    static unsigned short mIndexes[ 24 ];
};

#endif
