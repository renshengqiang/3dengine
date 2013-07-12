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
#include <SimpleMeshEffect.h>
class Entity{
typedef std::vector<Matrix4f> BoneOffsetMatrixVector;
typedef std::map<std::string, SkeletonAnimation *> SkeletonAnimationMap;
typedef SkeletonAnimationMap::iterator SkeletonAnimationMapIterator;
typedef SkeletonAnimationMap::value_type SkeletonAnimationMapValueType;

public:
	Entity();
	Entity(const std::string& meshFile);
	~Entity();
	AnimationState *GetAnimationState(const std::string &name);
	const AxisAlignedBox& GetBoundingBox() const;
	void SetMeshPtr(MeshPtr mesh);
	void Render(SimpleMeshEffect& effect);//use shader to render
private:
	void _updateAnimation(void);
		
private:
	//��Ӧ��Mesh
	MeshPtr m_mesh;
	//�����ı任�������ڴ��䵽shader��
	unsigned m_numBoneMatrices;
	BoneOffsetMatrixVector m_boneOffsetMatrixVec;
	//Entity��Ӧ�Ĺ�������
	AnimationStateSet m_animationStateSet;
	SkeletonAnimationMap m_animationMap;
	//local space ��̬��Χ��
	AxisAlignedBox m_fullBoundingBox;
};

#endif
