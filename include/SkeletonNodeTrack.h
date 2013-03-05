#ifndef _SKELETON_NODE_TRACK_H
#define _SKELETON_NODE_TRACK_H
#include "math_3d.h"
#include <string>
#include <vector>

class SkeletonNodeTrack {
public:
	SkeletonNodeTrack(const std::string& name);
	void AddKeyFrame(const Vector3f& position, const Quaternion& rotation, const Vector3f& scale);
	const std::string& GetName(void) const;
	const Vector3f GetPosition(unsigned index) const;
	const Quaternion GetRotation(unsigned index) const;
	const Vector3f GetScale(unsigned index)const;
private:
	std::string m_skeletonNodeName;
	std::vector<Vector3f> m_positionVec;
	std::vector<Quaternion> m_rotationVec;
	std::vector<Vector3f> m_scaleVec;
};

#endif
