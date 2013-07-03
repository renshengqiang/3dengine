#ifndef _OCTREESCENEMANAGER_H
#define _OCTREESCENEMANAGER_H

#include <SceneManager.h>
#include <Octree.h>

class OctreeNode;
class OctreeCamera;

/** Specialized SceneManager that divides the geometry into an octree in order to facilitate spatial queries.
@remarks
*/
class  OctreeSceneManager : public SceneManager
{
public:
	// Standard Constructor.  Initializes the octree to -10000,-10000,-10000 to 10000,10000,10000 with a depth of 8. 
	OctreeSceneManager();
	// Standard Constructor 
	OctreeSceneManager(AxisAlignedBox &box, int max_depth );
	// Standard destructor
	~OctreeSceneManager();

	virtual RaySceneQuery* CreateRayQuery(const Ray& ray);
	//Initializes the manager to the given box and depth.
	void Init( AxisAlignedBox &box, int d );

	 // Creates a specialized OctreeCamera    
	virtual Camera* CreateCamera(Vector3f pos= Vector3f(0,0,0), Vector3f target=Vector3f(0,0,-1), Vector3f up=Vector3f(0,1,0));

	// Does nothing more 
	virtual void _UpdateSceneGraph( Camera * cam );
	
	// Recurses through the octree determining which nodes are visible. 
	virtual void _FindVisibleObjects(Camera * cam, SceneManager::RenderQueue& renderQueue);

	/** Walks through the octree, adding any visible objects to the render queue.
	@remarks
	If any octant in the octree if completely within the view frustum,
	all subchildren are automatically added with no visibility tests.
	*/
	void WalkOctree( OctreeCamera*, RenderQueue&, Octree *,  bool);

	//Checks the given OctreeNode, and determines if it needs to be moved to a different octant.
	void _UpdateOctreeNode( OctreeNode * );
	
	//Removes the given octree node 
	void _RemoveOctreeNode( OctreeNode * );
	
	//Adds the Octree Node, starting at the given octree, and recursing at max to the specified depth.
	void _AddOctreeNode( OctreeNode *, Octree *octree, int depth = 0 );

	/** Recurses the octree, adding any nodes intersecting with the ray into the given list.
	It ignores the exclude scene node.
	*/
	void FindNodesIn( const Ray &ray, std::vector<SceneNode*>& list, SceneNode *exclude=0 );

protected:
	/// The root octree
	Octree *mp_octree;

	/// Max depth for the tree
	int m_maxDepth;
};
#endif
