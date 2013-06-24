#include <OctreeSceneManager.h>
#include <OctreeNode.h>
#include <OctreeCamera.h>

OctreeSceneManager::OctreeSceneManager() 
{
    AxisAlignedBox b( -10000, -10000, -10000, 10000, 10000, 10000 );
    int depth = 8; 
    mOctree = 0;
    init( b, depth );
}

OctreeSceneManager::OctreeSceneManager(AxisAlignedBox &box, int max_depth ) 
{
    mOctree = 0;
    init( box, max_depth );
}

void OctreeSceneManager::init( AxisAlignedBox &box, int depth )
{

    if ( mOctree != 0 )
        delete mOctree;

    mOctree = new Octree( 0 );

    mMaxDepth = depth;
    mBox = box;

    mOctree ->mBox = box;

    Vector3f min = box.getMinimum();

    Vector3f max = box.getMaximum();

    mOctree ->mHalfSize = ( max - min ) / 2;

    mNumObjects = 0;

    Vector3f v( 1.5, 1.5, 1.5 );

    mScaleFactor.setScale( v );

    //mSceneRoot isn't put into the octree since it has no volume.
}

OctreeSceneManager::~OctreeSceneManager()
{

    if ( mOctree )
	{
	        delete mOctree;
		mOctree = 0;
	}
}
//-----------------------------------------------------------------------
Camera* OctreeSceneManager::CreateCamera(Vector3f pos, Vector3f target, Vector3f up)
{
	if(mp_cameraInUse!=NULL)
		delete mp_cameraInUse;
	mp_cameraInUse = new OctreeCamera(pos, target, up);
	return mp_cameraInUse;
}
void OctreeSceneManager::destroySceneNode( const std::string &name )
{
/*
    OctreeNode * on = static_cast < OctreeNode* > ( getSceneNode( name ) );

    if ( on != 0 )
        _removeOctreeNode( on );

    SceneManager::destroySceneNode( name );
   */
}

void OctreeSceneManager::_updateOctreeNode( OctreeNode * onode )
{
    const AxisAlignedBox& box = onode ->GetWorldBoundingBox();

    if ( box.isNull() )
        return ;

	// Skip if octree has been destroyed (shutdown conditions)
	if (!mOctree)
		return;

    if ( onode ->getOctant() == 0 )
    {
        //if outside the octree, force into the root node.
        if ( ! onode ->_isIn( mOctree ->mBox ) )
            mOctree->_addNode( onode );
        else
            _addOctreeNode( onode, mOctree );
        return ;
    }

    if ( ! onode ->_isIn( onode ->getOctant() -> mBox ) )
    {
        _removeOctreeNode( onode );

        //if outside the octree, force into the root node.
        if ( ! onode ->_isIn( mOctree->mBox ) )
            mOctree->_addNode( onode );
        else
            _addOctreeNode( onode, mOctree );
    }
}

/** Only removes the node from the octree.  It leaves the octree, even if it's empty.
*/
void OctreeSceneManager::_removeOctreeNode( OctreeNode * n )
{
	// Skip if octree has been destroyed (shutdown conditions)
	if (!mOctree)
		return;

    Octree * oct = n ->getOctant();

    if ( oct )
    {
        oct ->_removeNode( n );
    }

    n->setOctant(0);
}

void OctreeSceneManager::_addOctreeNode( OctreeNode * n, Octree *octant, int depth )
{

	// Skip if octree has been destroyed (shutdown conditions)
	if (!mOctree)
		return;

	const AxisAlignedBox& bx = n ->GetWorldBoundingBox();


    //if the octree is twice as big as the scene node,
    //we will add it to a child.
    if ( ( depth < mMaxDepth ) && octant ->_isTwiceSize( bx ) )
    {
        int x, y, z;
        octant ->_getChildIndexes( bx, &x, &y, &z );

        if ( octant ->mChildren[ x ][ y ][ z ] == 0 )
        {
            octant ->mChildren[ x ][ y ][ z ] = new Octree( octant );
            const Vector3f& octantMin = octant ->mBox.getMinimum();
            const Vector3f& octantMax = octant ->mBox.getMaximum();
            Vector3f min, max;
	    //将原来的八叉树节点分成八块
            if ( x == 0 )
            {
                min.x = octantMin.x;
                max.x = ( octantMin.x + octantMax.x ) / 2;
            }

            else
            {
                min.x = ( octantMin.x + octantMax.x ) / 2;
                max.x = octantMax.x;
            }

            if ( y == 0 )
            {
                min.y = octantMin.y;
                max.y = ( octantMin.y + octantMax.y ) / 2;
            }

            else
            {
                min.y = ( octantMin.y + octantMax.y ) / 2;
                max.y = octantMax.y;
            }

            if ( z == 0 )
            {
                min.z = octantMin.z;
                max.z = ( octantMin.z + octantMax.z ) / 2;
            }

            else
            {
                min.z = ( octantMin.z + octantMax.z ) / 2;
                max.z = octantMax.z;
            }

            octant ->mChildren[ x ][ y ][ z ] ->mBox.setExtents( min, max );
            octant ->mChildren[ x ][ y ][ z ] ->mHalfSize = ( max - min ) / 2;
        }

        _addOctreeNode( n, octant ->mChildren[ x ][ y ][ z ], ++depth );

    }

    else
    {
        octant ->_addNode( n );
    }
}


SceneNode * OctreeSceneManager::createSceneNodeImpl( void )
{
    return new OctreeNode( this );
}

SceneNode * OctreeSceneManager::createSceneNodeImpl( const std::string &name )
{
    return new OctreeNode( this, name );
}

void OctreeSceneManager::_updateSceneGraph( Camera * cam )
{
    SceneManager::_updateSceneGraph( cam );
}

void OctreeSceneManager::_findVisibleObjects(Camera * cam, SceneManager::RenderQueue& renderQueue)
{
    renderQueue.clear();
    mVisible.clear();

    mNumObjects = 0;

    //walk the octree, adding all visible Octreenodes nodes to the render queue.
    walkOctree( static_cast < OctreeCamera * > ( cam ),  renderQueue, mOctree, false);
}

void OctreeSceneManager::walkOctree( OctreeCamera *camera, SceneManager::RenderQueue& renderQueue,Octree *octant , bool foundvisible)
{
    //return immediately if nothing is in the node.
    if ( octant ->numNodes() == 0 )
        return ;

    OctreeCamera::Visibility v = OctreeCamera::NONE;

    if ( foundvisible )
    {
        v = OctreeCamera::FULL;
    }
    else if ( octant == mOctree )
    {
        v = OctreeCamera::PARTIAL;
    }
    else
    {
        AxisAlignedBox box;
        octant ->_getCullBounds( &box );
        v = camera->getVisibility( box );
    }


    // if the octant is visible, or if it's the root node...
    if ( v != OctreeCamera::NONE )
    {

        //Add stuff to be rendered;
        Octree::NodeList::iterator it = octant ->mNodes.begin();
	RenderItem item;
        bool vis = true;

        while ( it != octant ->mNodes.end() )
        {
            OctreeNode * sn = *it;

            // if this octree is partially visible, manually cull all
            // scene nodes attached directly to this level.

            if ( v == OctreeCamera::PARTIAL )
                vis = camera ->IsVisible( sn->GetWorldBoundingBox(), NULL);

            if (vis)
            {
                mNumObjects++;
		
		item.pNode = sn;
		item.transMatrix = sn->_GetFullTransform();
                renderQueue.push_back(item);

                mVisible.push_back( sn );
            }

            ++it;
        }

        Octree* child;
        bool childfoundvisible = (v == OctreeCamera::FULL);
        if ( (child = octant->mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
            walkOctree( camera, renderQueue, child, childfoundvisible);

        if ( (child = octant->mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
	    walkOctree( camera, renderQueue, child, childfoundvisible);

        if ( (child = octant->mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
            walkOctree( camera, renderQueue, child, childfoundvisible);

        if ( (child = octant->mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
            walkOctree( camera, renderQueue, child, childfoundvisible);

        if ( (child = octant->mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
            walkOctree( camera, renderQueue, child, childfoundvisible);

        if ( (child = octant->mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
            walkOctree( camera, renderQueue, child, childfoundvisible);

        if ( (child = octant->mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
            walkOctree( camera, renderQueue, child, childfoundvisible);

        if ( (child = octant->mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
            walkOctree( camera, renderQueue, child, childfoundvisible);
    }
}
