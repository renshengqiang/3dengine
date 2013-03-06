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

public:
	Entity(const std::string& meshFile);
	Entity();
	~Entity();
	void SetMesh(Mesh *mesh);
	AnimationState *GetAnimationState(const std::string &name);
	void Render(void);//use shader to render
	
private:
	Mesh *mp_mesh;
	Skeleton *mp_skeleton;
	
	unsigned m_numBones;
	BoneOffsetMatrixVector m_boneOffsetMatrixVec;

	//Entity��Ӧ�Ĺ�������
	AnimationStateSet m_animationStateSet;
	//Ogre��֧��һ��Entity�е�������������һ����������Ч���ǳ���
	//��������ֻ֧��һ������
	AnimationState *mp_activeAnimationState;
	SkeletonAnimation *mp_activeSkeletonAnim;
};

#endif
