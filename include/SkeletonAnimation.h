#ifndef _SKELETON_ANIMATION_H
#define _SKELETON_ANIMATION_H
#include "SkeletonNodeTrack.h"
#include "SkeletonBone.h"
#include "math_3d.h"
#include <vector>
#include <string>

class Skeleton;
class Entity;
class SkeletonAnimation{
public:
	typedef std::vector<const SkeletonNodeTrack *> TrackVector;
	typedef std::vector<const SkeletonNodeTrack *>::iterator TrackVectorIterator;
	typedef std::vector<const SkeletonNodeTrack *>::const_iterator TrackVectorConstIterator;

	SkeletonAnimation(Skeleton *pSkeleton, const std::string name, double ticks, double ticksPerSecond);
	~SkeletonAnimation();
	double GetTicks(void) const;
	double GetTicksPersecond(void) const ;
	const std::string& GetName(void) const;
	const SkeletonNodeTrack *GetTrack(const std::string& name) const;
	void AddTrack(const SkeletonNodeTrack *);
	void ApplyToEntity(float timePos, Entity *pEntity);	
	void ReadNodeHeirarchy(float animationTime, SkeletonBone *pBone, const Matrix4f &parentTransform, Entity *pEntity);
	const Matrix4f CalcuInterPolatedTransform(const SkeletonNodeTrack *pTrack, float animationTime);
private:
	double m_ticks;//keyFrame num of each track
	double m_ticksPerSecond;
	std::string m_name;
	TrackVector m_trackVec;
	Skeleton *mp_skeleton;
};

#endif
