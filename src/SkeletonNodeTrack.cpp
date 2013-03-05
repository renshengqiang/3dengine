#include "SkeletonNodeTrack.h"

//----------------------------------------------------------------------------------
SkeletonNodeTrack::SkeletonNodeTrack(const std::string& name)
{
	m_skeletonNodeName = name;
}
//----------------------------------------------------------------------------------
void SkeletonNodeTrack::AddKeyFrame(const Vector3f& position, const Quaternion& rotation, const Vector3f& scale)
{
	m_positionVec.push_back(position);
	m_rotationVec.push_back(rotation);
	m_scaleVec.push_back(scale);
}
//----------------------------------------------------------------------------------
const std::string& SkeletonNodeTrack::GetName(void) const 
{
	return m_skeletonNodeName;
}
//----------------------------------------------------------------------------------
const Vector3f SkeletonNodeTrack::GetPosition(unsigned index) const 
{
	return m_positionVec[index];
}
//----------------------------------------------------------------------------------
const Quaternion SkeletonNodeTrack::GetRotation(unsigned index) const 
{
	return m_rotationVec[index];
}
//----------------------------------------------------------------------------------
const Vector3f SkeletonNodeTrack::GetScale(unsigned index) const
{
	return m_scaleVec[index];
}
