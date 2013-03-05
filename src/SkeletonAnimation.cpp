#include "SkeletonAnimation.h"

//----------------------------------------------------------------------------------
SkeletonAnimation::SkeletonAnimation(double ticks, double ticksPerSecond) : 
	m_ticks(ticks),
	m_ticksPerSecond(ticksPerSecond)
{
}
//----------------------------------------------------------------------------------
SkeletonAnimation::~SkeletonAnimation()
{	
	for(TrackVectorIterator iter = m_trackVec.begin(); iter != m_trackVec.end(); ++iter){
		delete const_cast<SkeletonNodeTrack *>(*iter);
	}
}
//----------------------------------------------------------------------------------
void SkeletonAnimation::AddTrack(const SkeletonNodeTrack *pTrack)
{
	m_trackVec.push_back(pTrack);
}
//----------------------------------------------------------------------------------
double SkeletonAnimation::GetTicks(void) const
{
	return m_ticks;
}
//----------------------------------------------------------------------------------
double SkeletonAnimation::GetTicksPersecond(void) const
{
	return m_ticksPerSecond;
}
//----------------------------------------------------------------------------------
const SkeletonNodeTrack *SkeletonAnimation::GetTrack(const std::string& name) const
{
	for(TrackVectorConstIterator iter = m_trackVec.begin(); iter != m_trackVec.end(); ++iter){
		const SkeletonNodeTrack *pTrack = *iter;
		if(pTrack->GetName() == name) return pTrack;
	}
	return NULL;
}
