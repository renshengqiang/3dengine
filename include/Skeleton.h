#ifndef _SKELETON_H
#define _SKELETON_H
#include "SkeletonBone.h"
#include "SkeletonAnimation.h"
#include "math_3d.h"
#include <vector>
#include <map>
#include <string>

class Skeleton {
public:
	typedef  std::vector<SkeletonAnimation *> AnimationVector;
	typedef  std::vector<SkeletonAnimation *>::iterator AnimationVectorIterator;
	typedef std::map<std::string, unsigned> BoneNameIndexMap;
	typedef std::map<std::string, unsigned>::const_iterator BoneNameIndexMapConstIterator;
	typedef std::map<std::string, unsigned>::value_type BoneNameIndexMapValueType;
	typedef std::vector<Matrix4f> BoneOffsetVector;
	typedef std::map<std::string, SkeletonBone*> BoneMap;
	typedef std::map<std::string, SkeletonBone*>::value_type BoneMapValueType;
	typedef std::map<std::string, SkeletonBone*>::iterator BoneMapIterator;
	
	Skeleton();
	~Skeleton();
	
	void 				SetRootBoneNode(SkeletonBone *rootBoneNode);
	SkeletonBone 		*GetRootBoneNode(void);
	void				AddBoneNode(const std::string& name, SkeletonBone *pBone);
	SkeletonBone 		*GetBoneNode(const std::string& name);
	
	void 				AddAnimation(SkeletonAnimation *animation);
	unsigned 			GetAnimationNum(void);
	SkeletonAnimation *GetAnimation(unsigned index);
	SkeletonAnimation *GetAnimation(const std::string& name);

	int				GetBoneIndex(const std::string& name) const;
	const Matrix4f&	GetBoneOffset(unsigned index) const;
	unsigned			GetBoneNum(void) const;
	const Matrix4f&	GetGlobalInverseMatrix(void) const;
	void				SetGloabalInverseMatrix(const Matrix4f& matrix);
	unsigned			AddBoneOffset(const std::string& name, const Matrix4f& offset);
private:
	//skeleton animation
	AnimationVector m_animationVec;
	//root bone node
	SkeletonBone *mp_rootBoneNode;
	//bone belongs to this skeleton, for query 
	BoneMap m_boneMap;
	//find a bone id with it's name
	BoneNameIndexMap m_boneNameIndexMap;
	//bone original offset matrix
	BoneOffsetVector m_boneInfo;
	//rootBone inverse offset matrix
	Matrix4f m_globalInverseMatrix;
};
#endif
