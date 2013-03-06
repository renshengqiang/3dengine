#include "Entity.h"
#include "Shader.h"

//--------------------------------------------------------------------------------------
Entity::Entity(const std::string&)
{
}
//--------------------------------------------------------------------------------------
Entity::Entity() :
	mp_mesh(NULL),
	mp_skeleton(NULL),
	m_numBones(0),
	mp_activeAnimationState(NULL),
	mp_activeSkeletonAnim(NULL)
{
}
//--------------------------------------------------------------------------------------
Entity::~Entity()
{
}
//--------------------------------------------------------------------------------------
void Entity::SetMesh(Mesh *mesh)
{
	mp_mesh = mesh;
			
	if(mesh->mp_skeleton){
		mp_skeleton = mesh->mp_skeleton;
		m_numBones = mesh->m_numBones;
		m_boneMapping = mesh->m_boneMapping;
		m_boneInfo = mesh->m_boneInfo;
		m_globalInverseTransform = mesh->m_globalInverseTransform;
		for(unsigned i=0; i < mp_skeleton->GetAnimationNum(); ++i){
			SkeletonAnimation *pSkeletonAnim = mp_skeleton->GetAnimation(i);
			std::string name = pSkeletonAnim->GetName();
			float length = pSkeletonAnim->GetTicks() /pSkeletonAnim->GetTicksPersecond();
			
			m_animationStateSet.CreateAnimationState(name,length, 0.0f, false);
		}
	}
}
//--------------------------------------------------------------------------------------
AnimationState *Entity::GetAnimationState(const std::string &name)
{
	mp_activeAnimationState = m_animationStateSet.GetAnimationState(name);
	if(mp_activeAnimationState){
		mp_activeSkeletonAnim = mp_skeleton->GetAnimation(name);
	}
	return mp_activeAnimationState;
}
//--------------------------------------------------------------------------------------
void Entity::Render(void)
{
	if(mp_activeAnimationState && mp_activeSkeletonAnim)  
		mp_activeSkeletonAnim->ApplyToEntity(mp_activeAnimationState->GetTimePosition(), this);
	if(m_numBones>0) SetIntValue(g_hasBonesLocation,1);
	else SetIntValue(g_hasBonesLocation,0);
	for(unsigned i=0; i<m_boneInfo.size();++i){
		SetTranslateMatrix(g_boneTransformLocation[i],&(m_boneInfo[i].m_finalTransformation));
	}
	if(mp_mesh) mp_mesh->RenderUseShader();
}
