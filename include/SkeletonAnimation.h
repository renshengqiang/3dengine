#ifndef _SKELETON_ANIMATION_H
#define _SKELETON_ANIMATION_H
#include "SkeletonNodeTrack.h"
#include <vector>
#include <string>

class SkeletonAnimation{
public:
	typedef std::vector<const SkeletonNodeTrack *> TrackVector;
	typedef std::vector<const SkeletonNodeTrack *>::iterator TrackVectorIterator;
	typedef std::vector<const SkeletonNodeTrack *>::const_iterator TrackVectorConstIterator;

	SkeletonAnimation(double ticks, double ticksPerSecond);
	~SkeletonAnimation();
	double GetTicks(void) const;
	double GetTicksPersecond(void) const ;
	const SkeletonNodeTrack *GetTrack(const std::string& name) const;
	void AddTrack(const SkeletonNodeTrack *);
private:
	double m_ticks;//keyFrame num of each track
	double m_ticksPerSecond;
	TrackVector m_trackVec;
};

#endif
