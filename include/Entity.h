#ifndef _ENTITY_H
#define _ENTITY_H
#include "Mesh.h"
#include "Skeleton.h"
#include "AnimationState.h"
#include "SkeletonAnimation.h"
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
	void Render(void);//use shader to render
private:
	void _updateAnimation(void);
		
private:
	//��Ӧ��Mesh
	Mesh *mp_mesh;
	MeshPtr m_mesh;
	//�����ı任�������ڴ��䵽shader��
	unsigned m_numBoneMatrices;
	BoneOffsetMatrixVector m_boneOffsetMatrixVec;

	//Entity��Ӧ�Ĺ�������
	AnimationStateSet m_animationStateSet;
	SkeletonAnimationMap m_animationMap;
	/*
	//Ogre��֧��һ��Entity�е�������������һ����������Ч���ǳ���
	//��������ֻ֧��һ������
	AnimationState *mp_activeAnimationState;
	SkeletonAnimation *mp_activeSkeletonAnim;
	*/
};

#endif
