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

	//Entity对应的骨骼动画
	AnimationStateSet m_animationStateSet;
	//Ogre中支持一个Entity中的两个骨骼动画一起做，但是效果非常差
	//我们这里只支持一个动画
	AnimationState *mp_activeAnimationState;
	SkeletonAnimation *mp_activeSkeletonAnim;
};

#endif
