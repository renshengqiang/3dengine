#ifndef _OCTREE_RAY_SCENE_QUERY_H
#define _OCTREE_RAY_SCENE_QUERY_H

#include <RaySceneQuery.h>
/** Octree implementation of RaySceneQuery. */
class OctreeRaySceneQuery : public RaySceneQuery
{
public:
	OctreeRaySceneQuery(SceneManager* creator);
	~OctreeRaySceneQuery();

	/** See RayScenQuery. */
	void execute(RaySceneQueryListener* listener);
};
#endif