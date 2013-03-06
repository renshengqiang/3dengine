#ifndef _SKELETON_H
#define _SKELETON_H
#include "SkeletonBone.h"
#include "SkeletonAnimation.h"
#include "math_3d.h"
#include <vector>
#include <string>

class Skeleton {
public:
	typedef  std::vector<SkeletonAnimation *> AnimationVector;
	typedef  std::vector<SkeletonAnimation *>::iterator AnimationVectorIterator;

	Skeleton();
	~Skeleton();
	void AddAnimation(SkeletonAnimation *animation);
	void SetRootBoneNode(SkeletonBone *rootBoneNode);
	SkeletonBone *GetRootBoneNode(void);
	SkeletonBone *GetBoneNode(const std::string &name);
	unsigned GetAnimationNum(void);
	SkeletonAnimation *GetAnimation(unsigned index);
	SkeletonAnimation *GetAnimation(const std::string& name);
	
private:
	AnimationVector m_animationVec;
	SkeletonBone *mp_rootBoneNode;
};

struct BoneInfo{
	Matrix4f m_boneOffset;
	Matrix4f m_finalTransformation;
	BoneInfo()
	{
		m_boneOffset.InitIdentity();
		m_finalTransformation.InitIdentity();
	}
};

#endif
