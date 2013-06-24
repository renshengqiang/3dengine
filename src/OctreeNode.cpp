#include <OctreeNode.h>
#include <OctreeSceneManager.h>

OctreeNode::OctreeNode( SceneManager* creator ) : SceneNode(""), mp_creator(creator)
{
    mOctant = 0;
}

OctreeNode::OctreeNode( SceneManager* creator, const std::string& name ) : SceneNode(name ), mp_creator(creator)
{
    mOctant = 0;
}

OctreeNode::~OctreeNode()
{}
void OctreeNode::_removeNodeAndChildren( )
{
    static_cast< OctreeSceneManager * > ( mp_creator ) ->_removeOctreeNode( this ); 
    //remove all the children nodes as well from the octree.
    
    for(ChildNodeIterator it = m_childVec.begin(); it != m_childVec.end(); ++it)
    {
        static_cast<OctreeNode *>(*it) ->_removeNodeAndChildren();
    }
}
Node * OctreeNode::removeChild( unsigned short index )
{
/*
    OctreeNode *on = static_cast<OctreeNode* >( SceneNode::removeChild( index ) );
    on -> _removeNodeAndChildren(); 
    return on; 
*/
	return NULL;
}
Node * OctreeNode::removeChild( Node* child )
{
/*
    OctreeNode *on = static_cast<OctreeNode* >( SceneNode::removeChild( child ) );
    on -> _removeNodeAndChildren(); 
    return on; 
*/
	return child;
}
void OctreeNode::removeAllChildren()
{
	for(ChildNodeIterator it = m_childVec.begin(); it !=m_childVec.end(); ++it)
	{
		OctreeNode *on = static_cast<OctreeNode*>(*it);
		on->SetParent(0);
		on->_removeNodeAndChildren();
	}
	m_childVec.clear();
}
    
Node * OctreeNode::removeChild( const std::string & name )
{
/*
    OctreeNode *on = static_cast< OctreeNode * >( SceneNode::removeChild(  name ) );
    on -> _removeNodeAndChildren( ); 
    return on; 
*/
	return NULL;
}

//same as SceneNode, only it doesn't care about children...
void OctreeNode::_updateBounds( void )
{
	m_worldAABB.setNull();
	mLocalAABB.setNull();

	// Update bounds from own attached objects
	if(mp_attachedEntity)
	{
		m_worldAABB = mp_attachedEntity->GetBoundingBox();
		m_worldAABB.transformAffine(_GetFullTransform());
	}

	//update the OctreeSceneManager that things might have moved.
	// if it hasn't been added to the octree, add it, and if has moved
	// enough to leave it's current node, we'll update it.
	if(!m_worldAABB.isNull())
	{
		static_cast< OctreeSceneManager* > (mp_creator) ->_updateOctreeNode(this);
	}
}

/** Since we are loose, only check the center.
*/
bool OctreeNode::_isIn( AxisAlignedBox &box )
{
	// Always fail if not in the scene graph or box is null
	if (box.isNull()) return false;

	// Always succeed if AABB is infinite
	if (box.isInfinite())
		return true;

	Vector3f center = m_worldAABB.getMaximum().midPoint(m_worldAABB.getMinimum());
	Vector3f bmin = box.getMinimum();
	Vector3f bmax = box.getMaximum();

	bool centre = (bmax > center && bmin < center);
	if(!centre) return false;

  	// Even if covering the centre line, need to make sure this BB is not large
	// enough to require being moved up into parent. When added, bboxes would
	// end up in parent due to cascade but when updating need to deal with
	// bbox growing too large for this child
	Vector3f octreeSize = bmax - bmin;
	Vector3f nodeSize = m_worldAABB.getMaximum() - m_worldAABB.getMinimum();
	return nodeSize < octreeSize;
}

/** Addes the attached objects of this OctreeScene node into the queue. */
/*
void OctreeNode::_addToRenderQueue( Camera* cam, RenderQueue *queue, 
	bool onlyShadowCasters, VisibleObjectsBoundsInfo* visibleBounds )
{
    ObjectMap::iterator mit = mObjectsByName.begin();

    while ( mit != mObjectsByName.end() )
    {
        MovableObject * mo = mit->second;
		
		queue->processVisibleObject(mo, cam, onlyShadowCasters, visibleBounds);

        ++mit;
    }

}
*/
