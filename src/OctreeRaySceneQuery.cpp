#include "OctreeRaySceneQuery.h"
#include "OctreeSceneManager.h"
#include <vector>

//---------------------------------------------------------------------
OctreeRaySceneQuery::OctreeRaySceneQuery(SceneManager* creator) : RaySceneQuery(creator)
{
}
//---------------------------------------------------------------------
OctreeRaySceneQuery::~OctreeRaySceneQuery()
{}
//---------------------------------------------------------------------
void OctreeRaySceneQuery::execute(RaySceneQueryListener* listener)
{
	std::vector<SceneNode*> list;

	static_cast<OctreeSceneManager *>(mpSceneManager)->FindNodesIn(mRay, list, 0);
   
	for(std::vector<SceneNode*>::iterator iter = list.begin(); iter != list.end(); ++ iter)
	{
		std::pair<bool, float> result = mRay.intersects((*iter)->GetWorldBoundingBox());
		if(result.first)
		{
			listener->queryResult(*iter, result.second);
		}
	}
}
