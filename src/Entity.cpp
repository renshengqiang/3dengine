#include "Entity.h"
#include "MeshManager.h"

//--------------------------------------------------------------------------------------
Entity::Entity():
	m_numBoneMatrices(0)
{
	m_visible = true;//default visible
}
//--------------------------------------------------------------------------------------
Entity::Entity(const std::string& name):
	m_numBoneMatrices(0)
{
	Skeleton *pSkeleton;
	m_mesh = MeshManager::GetSingleton().CreateMesh(name);
	pSkeleton = m_mesh->GetSkeleton(); 
	if(pSkeleton){
		m_numBoneMatrices = m_mesh->GetRelatedBoneNum();
		m_boneOffsetMatrixVec.resize(m_numBoneMatrices);
		for(unsigned i=0; i < pSkeleton->GetAnimationNum(); ++i){
			SkeletonAnimation *pSkeletonAnim = pSkeleton->GetAnimation(i);
			std::string name = pSkeletonAnim->GetName();
			float length = pSkeletonAnim->GetTicks() /pSkeletonAnim->GetTicksPersecond();
			
			m_animationStateSet.CreateAnimationState(name,length, 0.0f, false);
			m_animationMap.insert(SkeletonAnimationMapValueType(name, pSkeletonAnim));
		}
	}
	m_visible = true;
}
//--------------------------------------------------------------------------------------
Entity::~Entity()
{
	DetachFromParent();
}
//--------------------------------------------------------------------------------------
AnimationState *Entity::GetAnimationState(const std::string &name)
{
	return m_animationStateSet.GetAnimationState(name);
}
//--------------------------------------------------------------------------------------
const AxisAlignedBox& Entity::GetBoundingBox() const
{
	return m_mesh->GetBoundingBox();
}
//--------------------------------------------------------------------------------------
void Entity::SetMeshPtr(MeshPtr mesh)
{
	m_mesh = mesh;
}
//--------------------------------------------------------------------------------------
void Entity::_updateAnimation(void)
{
	for(unsigned i = 0; i<m_numBoneMatrices; ++i){
		m_boneOffsetMatrixVec[i].InitIdentity();
	}
	AnimationStateSet::EnabledAnimationStateIterator iter = m_animationStateSet._GetEnabledAnimationIteratorBegin();
	for(;iter!=m_animationStateSet._GetEnabledAnimationIeratorEnd(); ++iter)
	{
		AnimationState *animationState = *iter;
		SkeletonAnimation *animation = m_animationMap.find(animationState->GetName())->second;
		//use animation
		animation->ApplyToEntity(animationState->GetTimePosition(), m_boneOffsetMatrixVec);
	}
	return ;
}
//--------------------------------------------------------------------------------------
void Entity::Render(Effect *pEffect)
{
	SimpleMeshEffect *pSimpleMeshEffect = dynamic_cast<SimpleMeshEffect *>(pEffect);
	
	_updateAnimation();
	/*
	if(m_numBoneMatrices > 0) SetIntValue(g_hasBonesLocation,1);
	else SetIntValue(g_hasBonesLocation,0);
	for(unsigned i=0; i<m_numBoneMatrices;++i){
		SetTranslateMatrix(g_boneTransformLocation[i],&(m_boneOffsetMatrixVec[i]));
	}
	*/
	if(m_numBoneMatrices > 0) pSimpleMeshEffect->SetHasBones(1);
	else pSimpleMeshEffect->SetHasBones(0);

	for(unsigned i=0; i<m_numBoneMatrices; ++i)
	{
		pSimpleMeshEffect->SetBoneTransMatrix(m_boneOffsetMatrixVec[i], i);
	}
	//if(mp_mesh) mp_mesh->RenderUseShader();
	if(m_mesh.get()) m_mesh->RenderUseShader(*pSimpleMeshEffect);
}
