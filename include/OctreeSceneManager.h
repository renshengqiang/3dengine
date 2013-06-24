#ifndef _OCTREESCENEMANAGER_H
#define _OCTREESCENEMANAGER_H

#include <SceneManager.h>
#include <Octree.h>
#include <list>
#include <algorithm>
#include <string>

class OctreeNode;
class OctreeCamera;

/** Specialized SceneManager that divides the geometry into an octree in order to facilitate spatial queries.
@remarks
*/
class  OctreeSceneManager : public SceneManager
{
public:
    /** Standard Constructor.  Initializes the octree to -10000,-10000,-10000 to 10000,10000,10000 with a depth of 8. */
    OctreeSceneManager();
    /** Standard Constructor */
    OctreeSceneManager(AxisAlignedBox &box, int max_depth );
    /** Standard destructor */
    ~OctreeSceneManager();

    /** Initializes the manager to the given box and depth.
    */
    void init( AxisAlignedBox &box, int d );

    /** Creates a specialized OctreeNode */
    virtual SceneNode * createSceneNodeImpl ( void );
    /** Creates a specialized OctreeNode */
    virtual SceneNode * createSceneNodeImpl ( const std::string &name );

    /** Creates a specialized OctreeCamera */    
    virtual Camera* CreateCamera(Vector3f pos= Vector3f(0,0,0), Vector3f target=Vector3f(0,0,-1), Vector3f up=Vector3f(0,1,0));
    
    /** Deletes a scene node */
    virtual void destroySceneNode( const std::string &name );

    /** Does nothing more */
    virtual void _updateSceneGraph( Camera * cam );
    /** Recurses through the octree determining which nodes are visible. */
    virtual void _findVisibleObjects(Camera * cam, SceneManager::RenderQueue& renderQueue);

    /** Walks through the octree, adding any visible objects to the render queue.
    @remarks
    If any octant in the octree if completely within the view frustum,
    all subchildren are automatically added with no visibility tests.
    */
    void walkOctree( OctreeCamera*, RenderQueue&, Octree *,  bool);

    /** Checks the given OctreeNode, and determines if it needs to be moved
    * to a different octant.
    */
    void _updateOctreeNode( OctreeNode * );
    /** Removes the given octree node */
    void _removeOctreeNode( OctreeNode * );
    /** Adds the Octree Node, starting at the given octree, and recursing at max to the specified depth.
    */
    void _addOctreeNode( OctreeNode *, Octree *octree, int depth = 0 );

    /** Recurses the octree, adding any nodes intersecting with the box into the given list.
    It ignores the exclude scene node.
    */
    //void findNodesIn( const AxisAlignedBox &box, list< SceneNode * >::type &list, SceneNode *exclude = 0 );

    void setLooseOctree( bool b )
    {
        mLoose = b;
    };


    /** Resizes the octree to the given size */
    //void resize( const AxisAlignedBox &box );


    /** Overridden from SceneManager */
   // void clearScene(void);
protected:
	
    Octree::NodeList mVisible;

    /// The root octree
    Octree *mOctree;

    /// Number of rendered objs
    int mNumObjects;

    /// Max depth for the tree
    int mMaxDepth;

    /// Size of the octree
    AxisAlignedBox mBox;

    bool mLoose;

    Matrix4f mScaleFactor;

};
#endif
