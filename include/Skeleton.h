#ifndef _SKELETON_H
#define _SKELETON_H
#include "SkeletonBone.h"
#include "SkeletonAnimation.h"
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
	
private:
	AnimationVector m_animationVec;
	SkeletonBone *mp_rootBoneNode;
};

#endif
