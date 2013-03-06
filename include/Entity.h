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
friend class SkeletonAnimation;
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
	std::map<std::string, unsigned> m_boneMapping;
	std::vector<struct BoneInfo> m_boneInfo;
	Matrix4f m_globalInverseTransform;

	//Entity��Ӧ�Ĺ�������
	AnimationStateSet m_animationStateSet;
	//Ogre��֧��һ��Entity�е�������������һ����������Ч���ǳ���
	//��������ֻ֧��һ������
	AnimationState *mp_activeAnimationState;
	SkeletonAnimation *mp_activeSkeletonAnim;
};

#endif
