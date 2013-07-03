#ifndef _RAY_SCENE_QUERY_H
#define _RAY_SCENE_QUERY_H

#include <SceneNode.h>
#include <Ray.h>
#include <vector>
/** Alternative listener class for dealing with RaySceneQuery.
@remarks
	Because the RaySceneQuery returns results in an extra bit of information, namely
	distance, the listener interface must be customised from the standard SceneQueryListener.
*/
class  RaySceneQueryListener 
{
public:
	virtual ~RaySceneQueryListener() { }
	/** Called when a movable objects intersects the ray.
	@remarks
		As with SceneQueryListener, the implementor of this method should return 'true'
		if further results are required, or 'false' to abandon any further results from
		the current query.
	*/
	virtual bool queryResult(SceneNode* obj, float distance) = 0;

	/** Called when a world fragment is intersected by the ray. 
	@remarks
		As with SceneQueryListener, the implementor of this method should return 'true'
		if further results are required, or 'false' to abandon any further results from
		the current query.
	*/
	//virtual bool queryResult(SceneQuery::WorldFragment* fragment, Real distance) = 0;

};

/** This struct allows a single comparison of result data no matter what the type */
struct RaySceneQueryResultEntry
{
	/// Distance along the ray
	float distance;
	/// The movable, or NULL if this is not a movable result
	SceneNode* movable;
	/// The world fragment, or NULL if this is not a fragment result
	//SceneQuery::WorldFragment* worldFragment;
	/// Comparison operator for sorting
	bool operator < (const RaySceneQueryResultEntry& rhs) const
	{
	    return this->distance < rhs.distance;
	}

};
typedef std::vector<RaySceneQueryResultEntry> RaySceneQueryResult;

class SceneManager;
/** Specialises the SceneQuery class for querying along a ray. */
class RaySceneQuery :  public RaySceneQueryListener
{
protected:
	SceneManager *mpSceneManager;
	Ray mRay;
	bool mSortByDistance;
	unsigned short mMaxResults;
	RaySceneQueryResult mResult;

public:
    RaySceneQuery(SceneManager* mgr);
    virtual ~RaySceneQuery();
    /** Sets the ray which is to be used for this query. */
    virtual void setRay(const Ray& ray);
    /** Gets the ray which is to be used for this query. */
    virtual const Ray& getRay(void) const;
    /** Sets whether the results of this query will be sorted by distance along the ray.
    @remarks
        Often you want to know what was the first object a ray intersected with, and this 
        method allows you to ask the query to sort the results so that the nearest results
        are listed first.
    @par
        Note that because the query returns results based on bounding volumes, the ray may not
        actually intersect the detail of the objects returned from the query, just their 
        bounding volumes. For this reason the caller is advised to use more detailed 
        intersection tests on the results if a more accurate result is required; OGRE uses 
        bounds checking in order to give the most speedy results since not all applications 
        need extreme accuracy.
    @param sort If true, results will be sorted.
    @param maxresults If sorting is enabled, this value can be used to constrain the maximum number
        of results that are returned. Please note (as above) that the use of bounding volumes mean that
        accuracy is not guaranteed; if in doubt, allow more results and filter them in more detail.
        Important: 0 means unlimited results.
    */
    virtual void setSortByDistance(bool sort, unsigned short maxresults = 0);
    /** Gets whether the results are sorted by distance. */
    virtual bool getSortByDistance(void) const;
    /** Gets the maximum number of results returned from the query (only relevant if 
    results are being sorted) */
    virtual unsigned short getMaxResults(void) const;
    /** Executes the query, returning the results back in one list.
    @remarks
        This method executes the scene query as configured, gathers the results
        into one structure and returns a reference to that structure. These
        results will also persist in this query object until the next query is
        executed, or clearResults() is called. An more lightweight version of
        this method that returns results through a listener is also available.
    */
    virtual RaySceneQueryResult& execute(void);

    /** Executes the query and returns each match through a listener interface. 
    @remarks
        Note that this method does not store the results of the query internally 
        so does not update the 'last result' value. This means that this version of
        execute is more lightweight and therefore more efficient than the version 
        which returns the results as a collection.
    */
    virtual void execute(RaySceneQueryListener* listener);

    /** Gets the results of the last query that was run using this object, provided
        the query was executed using the collection-returning version of execute. 
    */
    virtual RaySceneQueryResult& getLastResults(void);
    /** Clears the results of the last query execution.
    @remarks
        You only need to call this if you specifically want to free up the memory
        used by this object to hold the last query results. This object clears the
        results itself when executing and when destroying itself.
    */
    virtual void clearResults(void);

    /** Self-callback in order to deal with execute which returns collection. */
    bool queryResult(SceneNode* pNode, float distance);
    /** Self-callback in order to deal with execute which returns collection. */
    //bool queryResult(SceneQuery::WorldFragment* fragment, float distance);
};

#endif
