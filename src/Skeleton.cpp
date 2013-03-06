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
void	Skeleton::AddBoneNode(const std::string& name, SkeletonBone *pBone)
{
	m_boneMap.insert(BoneMapValueType(pBone->GetName(), pBone));
}
//---------------------------------------------------------------------------------
SkeletonBone *Skeleton::GetBoneNode(const std::string &name)
{
	BoneMapIterator iter = m_boneMap.find(name);
	if(iter!=m_boneMap.end()) return iter->second;
	return NULL;
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
//---------------------------------------------------------------------------------
int Skeleton::GetBoneIndex(const std::string& name)  const
{
	BoneNameIndexMapConstIterator iter = m_boneNameIndexMap.find(name);
	if(iter != m_boneNameIndexMap.end()){
		return iter->second;
	}
	return -1;/*not find*/
}
//---------------------------------------------------------------------------------
const Matrix4f& Skeleton::GetBoneOffset(unsigned index) const
{
	return m_boneInfo[index];
}
//---------------------------------------------------------------------------------
unsigned	Skeleton::GetBoneNum(void) const
{
	return m_boneInfo.size();
}
//---------------------------------------------------------------------------------
unsigned Skeleton::AddBoneOffset(const std::string& name, const Matrix4f& offset)
{
	int index = GetBoneIndex(name);
	if(index >= 0) return index;
	m_boneInfo.push_back(offset);
	m_boneNameIndexMap.insert(BoneNameIndexMapValueType(name, m_boneInfo.size() -1));
	return m_boneInfo.size() - 1;
}
//---------------------------------------------------------------------------------
const Matrix4f& Skeleton::GetGlobalInverseMatrix(void) const
{
	return m_globalInverseMatrix;
}
//---------------------------------------------------------------------------------
void	Skeleton::SetGloabalInverseMatrix(const Matrix4f& matrix)
{
	m_globalInverseMatrix = matrix;
}
