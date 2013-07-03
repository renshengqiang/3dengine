#include "RaySceneQuery.h"
#include "SceneManager.h"
#include <algorithm>
#include <utility>
#include <queue>
//-----------------------------------------------------------------------
RaySceneQuery::RaySceneQuery(SceneManager* mgr)
{
	mpSceneManager = mgr;
	mSortByDistance = false;
	mMaxResults = 0;
}
//-----------------------------------------------------------------------
RaySceneQuery::~RaySceneQuery()
{
}
//-----------------------------------------------------------------------
void RaySceneQuery::setRay(const Ray& ray)
{
	mRay = ray;
}
//-----------------------------------------------------------------------
const Ray& RaySceneQuery::getRay(void) const
{
	return mRay;
}
//-----------------------------------------------------------------------
void RaySceneQuery::setSortByDistance(bool sort, unsigned short maxresults)
{
	mSortByDistance = sort;
	mMaxResults = maxresults;
}
//-----------------------------------------------------------------------
bool RaySceneQuery::getSortByDistance(void) const
{
	return mSortByDistance;
}
//-----------------------------------------------------------------------
unsigned short RaySceneQuery::getMaxResults(void) const
{
	return mMaxResults;
}
//-----------------------------------------------------------------------
RaySceneQueryResult& RaySceneQuery::execute(void)
{
	// Clear without freeing the vector buffer
	mResult.clear();

	// Call callback version with self as listener
	this->execute(this);

	if (mSortByDistance)
	{
	    if (mMaxResults != 0 && mMaxResults < mResult.size())
	    {
	        // Partially sort the N smallest elements, discard others
	        std::partial_sort(mResult.begin(), mResult.begin()+mMaxResults, mResult.end());
	        mResult.resize(mMaxResults);
	    }
	    else
	    {
	        // Sort entire result array
	        std::sort(mResult.begin(), mResult.end());
	    }
	}

	return mResult;
}
//-----------------------------------------------------------------------
void RaySceneQuery::execute(RaySceneQueryListener *listener)
{
	// Note that because we have no scene partitioning, we actually
	// perform a complete scene search even if restricted results are
	// requested; smarter scene manager queries can utilise the paritioning 
	// of the scene in order to reduce the number of intersection tests 
	// required to fulfil the query
	std::queue<SceneNode*> IterQueue;
	int no_intersection = 0;

	IterQueue.push(mpSceneManager->GetRootNode());
	while(!IterQueue.empty())
	{
		SceneNode *pNode = IterQueue.front();
		IterQueue.pop();

		// Do ray/box test
		std::pair<bool, float> result =
			mRay.intersects(pNode->GetWorldBoundingBox());

		if (result.first)
		{
			if (!listener->queryResult(pNode, result.second)) return;
		}
		else
			no_intersection++;
		
		int n  = pNode->NumChildren();
		for(int i=0; i<n; ++i)
			IterQueue.push((SceneNode*)(pNode->GetChild(i)));		
	}
}
//-----------------------------------------------------------------------
RaySceneQueryResult& RaySceneQuery::getLastResults(void)
{
    return mResult;
}
//-----------------------------------------------------------------------
void RaySceneQuery::clearResults(void)
{
    // C++ idiom to free vector buffer: swap with empty vector
    RaySceneQueryResult().swap(mResult);
}
//-----------------------------------------------------------------------
bool RaySceneQuery::queryResult(SceneNode* pNode, float distance)
{
    // Add to internal list
    RaySceneQueryResultEntry dets;
    dets.distance = distance;
    dets.movable = pNode;
    //dets.worldFragment = NULL;
    mResult.push_back(dets);
    // Continue
    return true;
}
