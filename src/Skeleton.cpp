#include "Skeleton.h"

Skeleton::Skeleton():
	mp_rootBoneNode(NULL)
{
}
//---------------------------------------------------------------------------------
Skeleton::~Skeleton()
{
	if(mp_rootBoneNode){
		delete mp_rootBoneNode;
	}
	for(AnimationVectorIterator iter = m_animationVec.begin(); iter != m_animationVec.end(); ++iter){
		delete *iter;
	}
}
//---------------------------------------------------------------------------------
void Skeleton::AddAnimation(SkeletonAnimation *animation)
{
	m_animationVec.push_back(animation);
}
//---------------------------------------------------------------------------------
void Skeleton::SetRootBoneNode(SkeletonBone *rootBoneNode)
{
	if(mp_rootBoneNode) delete mp_rootBoneNode;
	mp_rootBoneNode = rootBoneNode;
}
//---------------------------------------------------------------------------------
SkeletonBone *Skeleton::GetRootBoneNode(void)
{
	return mp_rootBoneNode;
}
//---------------------------------------------------------------------------------
SkeletonBone *Skeleton::GetBoneNode(const std::string &name)
{
	if(mp_rootBoneNode->GetName() == name) return mp_rootBoneNode;
	else return (SkeletonBone *)mp_rootBoneNode->GetChild(name);
}
//---------------------------------------------------------------------------------
unsigned Skeleton::GetAnimationNum(void)
{
	return m_animationVec.size();
}
//---------------------------------------------------------------------------------
SkeletonAnimation *Skeleton::GetAnimation(unsigned index)
{
	if(index > m_animationVec.size()) return NULL;
	return m_animationVec[index];
}
//---------------------------------------------------------------------------------
SkeletonAnimation *Skeleton::GetAnimation(const std::string& name)
{
	for(unsigned i=0; i < m_animationVec.size(); ++i){
		if(m_animationVec[i]->GetName() == name) return m_animationVec[i];
	}
	return NULL;
}
