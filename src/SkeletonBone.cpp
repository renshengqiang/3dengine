#include "SkeletonBone.h"

SkeletonBone::SkeletonBone(const std::string &name, const Matrix4f &transform):
	Node(name),
	m_transformMatrix(transform)
{
}
//---------------------------------------------------------------------------------
SkeletonBone::SkeletonBone(const std::string &name):
	Node(name)
{
}
//---------------------------------------------------------------------------------
SkeletonBone::~SkeletonBone()
{
}
//---------------------------------------------------------------------------------
const Matrix4f& SkeletonBone::GetTransform(void)
{
	return m_transformMatrix;
}
//---------------------------------------------------------------------------------
void SkeletonBone::SetTransform(const Matrix4f& transform)
{
	m_transformMatrix = transform;
}
//---------------------------------------------------------------------------------
SkeletonBone *SkeletonBone::CreateChildBone(const std::string &name, const Matrix4f &transform)
{
	SkeletonBone *pBone = static_cast<SkeletonBone*>(this->CreateChild(name));
	pBone->SetTransform(transform);
	
	return pBone;
}
//---------------------------------------------------------------------------------
SkeletonBone* SkeletonBone::CreateChildImpl(const std::string &name)
{
	return new SkeletonBone(name);
}

