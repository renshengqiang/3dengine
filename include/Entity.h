#ifndef _ENTITY_H
#define _ENTITY_H
#include <Mesh.h>
#include <Skeleton.h>
#include <AnimationState.h>
#include <SkeletonAnimation.h>
#include <AxisAlignedBox.h>
#include <map>
#include <vector>
#include <string>

class Entity{
typedef std::vector<Matrix4f> BoneOffsetMatrixVector;
typedef std::map<std::string, SkeletonAnimation *> SkeletonAnimationMap;
typedef SkeletonAnimationMap::iterator SkeletonAnimationMapIterator;
typedef SkeletonAnimationMap::value_type SkeletonAnimationMapValueType;

public:
	Entity(const std::string& meshFile);
	~Entity();
	AnimationState *GetAnimationState(const std::string &name);
	 const AxisAlignedBox& GetWorldBoundingBox() const
	{
	    return m_fullBoundingBox;
	}
	void Render(void);//use shader to render
private:
	void _updateAnimation(void);
		
private:
	//对应的Mesh
	MeshPtr m_mesh;
	//骨骼的变换矩阵，用于传输到shader中
	unsigned m_numBoneMatrices;
	BoneOffsetMatrixVector m_boneOffsetMatrixVec;
	//Entity对应的骨骼动画
	AnimationStateSet m_animationStateSet;
	SkeletonAnimationMap m_animationMap;
	AxisAlignedBox m_fullBoundingBox;
};

#endif
