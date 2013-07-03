#include <OctreeSceneManager.h>
#include <OctreeNode.h>
#include <OctreeCamera.h>
#include <OctreeRaySceneQuery.h>
#include <algorithm>

enum Intersection
{
	OUTSIDE=0,
	INSIDE=1,
	INTERSECT=2
};
Intersection intersect( const Ray &one, const AxisAlignedBox &two )
{
	// Null box?
	if (two.isNull()) return OUTSIDE;
	// Infinite box?
	if (two.isInfinite()) return INTERSECT;

	bool inside = true;
	const Vector3f& twoMin = two.getMinimum();
	const Vector3f& twoMax = two.getMaximum();
	Vector3f origin = one.getOrigin();
	Vector3f dir = one.getDirection();

	Vector3f maxT(-1, -1, -1);

	int i = 0;
	for(i=0; i<3; i++ )
	{
		if( origin[i] < twoMin[i] )
		{
			inside = false;
			if( dir[i] > 0 )
			{
				maxT[i] = (twoMin[i] - origin[i])/ dir[i];
			}
		}
		else if( origin[i] > twoMax[i] )
		{
			inside = false;
			if( dir[i] < 0 )
			{
				maxT[i] = (twoMax[i] - origin[i]) / dir[i];
			}
		}
	}

	if( inside )
	{
		return INTERSECT;
	}
	int whichPlane = 0;
	if( maxT[1] > maxT[whichPlane])
	whichPlane = 1;
	if( maxT[2] > maxT[whichPlane])
	whichPlane = 2;

	if( ((int)maxT[whichPlane]) & 0x80000000 )
	{
		return OUTSIDE;
	}
	for(i=0; i<3; i++ )
	{
		if( i!= whichPlane )
		{
			float f = origin[i] + maxT[whichPlane] * dir[i];
			if ( f < (twoMin[i] - 0.00001f) ||
				f > (twoMax[i] +0.00001f ) )
			{
				return OUTSIDE;
			}
		}
	}
	return INTERSECT;
}
//-----------------------------------------------------------------------------
void _findNodes( const Ray &t, std::vector<SceneNode*>& list, SceneNode *exclude, bool full, Octree *octant )
{

	if ( !full )
	{
		AxisAlignedBox obox;
		octant->_GetCullBounds( &obox );

		Intersection isect = intersect( t, obox );

		if ( isect == OUTSIDE )
			return ;

		full = ( isect == INSIDE );
	}

	Octree::NodeList::iterator it = octant->mNodes.begin();

	while ( it != octant->mNodes.end())
	{
		OctreeNode * on = ( *it );

		if ( on != exclude )
		{
			if ( full )
			{
				list.push_back( on );
			}
			else
			{
				Intersection nsect = intersect( t, on->GetWorldBoundingBox() );

				if ( nsect != OUTSIDE )
				{
					list.push_back( on );
				}
			}
		}
		++it;
	}

	Octree* child;

	if ( (child=octant->m_children[ 0 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant->m_children[ 1 ][ 0 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant->m_children[ 0 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant->m_children[ 1 ][ 1 ][ 0 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant->m_children[ 0 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant->m_children[ 1 ][ 0 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant->m_children[ 0 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );

	if ( (child=octant->m_children[ 1 ][ 1 ][ 1 ]) != 0 )
		_findNodes( t, list, exclude, full, child );
}
//-----------------------------------------------------------------------------
OctreeSceneManager::OctreeSceneManager() 
{
	AxisAlignedBox b( -10000, -10000, -10000, 10000, 10000, 10000 );
	int depth = 8; 
	mp_octree = 0;
	Init( b, depth );
	delete mp_rootNode;
	mp_rootNode = new OctreeNode(this);
}
//-----------------------------------------------------------------------------
OctreeSceneManager::OctreeSceneManager(AxisAlignedBox &box, int max_depth ) 
{
	mp_octree = 0;
	Init( box, max_depth );
}
//-----------------------------------------------------------------------------
RaySceneQuery* OctreeSceneManager::CreateRayQuery(const Ray& ray)
{
	RaySceneQuery* q = new OctreeRaySceneQuery(this);
	q->setRay(ray);
	return q;
}
//-----------------------------------------------------------------------------
void OctreeSceneManager::Init( AxisAlignedBox &box, int depth )
{

	if ( mp_octree != 0 )
	    delete mp_octree;

	mp_octree = new Octree( 0 );

	m_maxDepth = depth;

	mp_octree ->m_box = box;

	Vector3f min = box.getMinimum();

	Vector3f max = box.getMaximum();

	mp_octree ->m_halfSize = ( max - min ) / 2;
}
//-----------------------------------------------------------------------------
OctreeSceneManager::~OctreeSceneManager()
{

    if ( mp_octree )
	{
	        delete mp_octree;
		mp_octree = 0;
	}
}
//-----------------------------------------------------------------------------
Camera* OctreeSceneManager::CreateCamera(Vector3f pos, Vector3f target, Vector3f up)
{
	if(mp_cameraInUse!=NULL)
		delete mp_cameraInUse;
	mp_cameraInUse = new OctreeCamera(pos, target, up);
	return mp_cameraInUse;
}
//-----------------------------------------------------------------------------
/*
对八叉树进行更新操作
节点进行了松散处理，松散处理见_AddOctreeNode
*/
void OctreeSceneManager::_UpdateOctreeNode( OctreeNode * onode )
{
	const AxisAlignedBox& box = onode ->GetWorldBoundingBox();

	if ( box.isNull() )
		return ;

	// Skip if octree has been destroyed (shutdown conditions)
	if (!mp_octree)
		return;

	if ( onode->GetOctant() == 0 )
	{
		//if outside the octree, force into the root node.
		if ( ! onode->_IsIn( mp_octree->m_box ) )
		    mp_octree->_AddNode( onode );
		else
		    _AddOctreeNode( onode, mp_octree );
		return ;
	}

	if ( ! onode->_IsIn( onode ->GetOctant()->m_box ) )
	{
		_RemoveOctreeNode( onode );

		//if outside the octree, force into the root node.
		if ( ! onode->_IsIn( mp_octree->m_box ) )
			mp_octree->_AddNode( onode );
		else
			_AddOctreeNode( onode, mp_octree );
	}
}
//-----------------------------------------------------------------------------
/** Only removes the node from the octree.  It leaves the octree, even if it's empty.
*/
void OctreeSceneManager::_RemoveOctreeNode( OctreeNode * n )
{
	// Skip if octree has been destroyed (shutdown conditions)
	if (!mp_octree)
		return;

	Octree * oct = n ->GetOctant();

	if ( oct )
	{
		oct ->_RemoveNode( n );
	}

	n->SetOctant(0);
}
//-----------------------------------------------------------------------------
void OctreeSceneManager::_AddOctreeNode( OctreeNode * n, Octree *octant, int depth )
{

	// Skip if octree has been destroyed (shutdown conditions)
	if (!mp_octree)
		return;

	const AxisAlignedBox& bx = n ->GetWorldBoundingBox();


	//if the octree is twice as big as the scene node,
	//we will add it to a child.
	if ( ( depth < m_maxDepth ) && octant ->_IsTwiceSize( bx ) )
	{
		int x, y, z;
		octant ->_GetChildIndexes( bx, &x, &y, &z );

		if ( octant ->m_children[ x ][ y ][ z ] == 0 )
		{
			octant ->m_children[ x ][ y ][ z ] = new Octree( octant );
			const Vector3f& octantMin = octant ->m_box.getMinimum();
			const Vector3f& octantMax = octant ->m_box.getMaximum();
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

			octant ->m_children[ x ][ y ][ z ] ->m_box.setExtents( min, max );
			octant ->m_children[ x ][ y ][ z ] ->m_halfSize = ( max - min ) / 2;
		}

		_AddOctreeNode( n, octant ->m_children[ x ][ y ][ z ], ++depth );
	}
	else
	{
	    octant ->_AddNode( n );
	}
}
//-----------------------------------------------------------------------------
void OctreeSceneManager::FindNodesIn( const Ray &ray, std::vector<SceneNode*>& list, SceneNode *exclude)
{
	_findNodes(ray, list, exclude, false, mp_octree);
}
//-----------------------------------------------------------------------------
void OctreeSceneManager::_UpdateSceneGraph( Camera * cam )
{
	SceneManager::_UpdateSceneGraph( cam );
}
//-----------------------------------------------------------------------------
void OctreeSceneManager::_FindVisibleObjects(Camera * cam, SceneManager::RenderQueue& renderQueue)
{
	renderQueue.clear();

	//walk the octree, adding all visible Octreenodes nodes to the render queue.
	WalkOctree( static_cast < OctreeCamera * > ( cam ),  renderQueue, mp_octree, false);
	
	//printf("renderQueue size: %d\n", renderQueue.size());
}
//-----------------------------------------------------------------------------
void OctreeSceneManager::WalkOctree( OctreeCamera *camera, SceneManager::RenderQueue& renderQueue,Octree *octant , bool foundvisible)
{
	//return immediately if nothing is in the node.
	if ( octant ->NumNodes() == 0 )
		return ;
	OctreeCamera::Visibility v = OctreeCamera::NONE;

	if ( foundvisible )
	{
		v = OctreeCamera::FULL;
	}
	else if ( octant == mp_octree )
	{
		v = OctreeCamera::PARTIAL;
	}
	else
	{
		AxisAlignedBox box;
		octant ->_GetCullBounds( &box );
		v = camera->GetVisibility( box );
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
			item.pNode = sn;
			item.transMatrix = sn->_GetFullTransform();
		        renderQueue.push_back(item);
		    }

		    ++it;
		}

		Octree* child;
		bool childfoundvisible = (v == OctreeCamera::FULL);
		if ( (child = octant->m_children[ 0 ][ 0 ][ 0 ]) != 0 )
		    WalkOctree( camera, renderQueue, child, childfoundvisible);

		if ( (child = octant->m_children[ 1 ][ 0 ][ 0 ]) != 0 )
		WalkOctree( camera, renderQueue, child, childfoundvisible);

		if ( (child = octant->m_children[ 0 ][ 1 ][ 0 ]) != 0 )
		    WalkOctree( camera, renderQueue, child, childfoundvisible);

		if ( (child = octant->m_children[ 1 ][ 1 ][ 0 ]) != 0 )
		    WalkOctree( camera, renderQueue, child, childfoundvisible);

		if ( (child = octant->m_children[ 0 ][ 0 ][ 1 ]) != 0 )
		    WalkOctree( camera, renderQueue, child, childfoundvisible);

		if ( (child = octant->m_children[ 1 ][ 0 ][ 1 ]) != 0 )
		    WalkOctree( camera, renderQueue, child, childfoundvisible);

		if ( (child = octant->m_children[ 0 ][ 1 ][ 1 ]) != 0 )
		    WalkOctree( camera, renderQueue, child, childfoundvisible);

		if ( (child = octant->m_children[ 1 ][ 1 ][ 1 ]) != 0 )
		    WalkOctree( camera, renderQueue, child, childfoundvisible);
	}
}
