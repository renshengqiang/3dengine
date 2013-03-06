#include "SkeletonAnimation.h"
#include "Entity.h"

//----------------------------------------------------------------------------------
SkeletonAnimation::SkeletonAnimation(Skeleton *pSkeleton, const std::string name, double ticks, double ticksPerSecond) : 
	m_ticks(ticks),
	m_ticksPerSecond(ticksPerSecond),
	m_name(name),
	mp_skeleton(pSkeleton)
{
}
//----------------------------------------------------------------------------------
SkeletonAnimation::~SkeletonAnimation()
{	
	for(TrackVectorIterator iter = m_trackVec.begin(); iter != m_trackVec.end(); ++iter){
		delete const_cast<SkeletonNodeTrack *>(*iter);
	}
}
//----------------------------------------------------------------------------------
void SkeletonAnimation::AddTrack(const SkeletonNodeTrack *pTrack)
{
	m_trackVec.push_back(pTrack);
}
//----------------------------------------------------------------------------------
double SkeletonAnimation::GetTicks(void) const
{
	return m_ticks;
}
//----------------------------------------------------------------------------------
double SkeletonAnimation::GetTicksPersecond(void) const
{
	return m_ticksPerSecond;
}
//----------------------------------------------------------------------------------
const std::string& SkeletonAnimation::GetName(void) const
{
	return m_name;
}
//----------------------------------------------------------------------------------
const SkeletonNodeTrack *SkeletonAnimation::GetTrack(const std::string& name) const
{
	for(TrackVectorConstIterator iter = m_trackVec.begin(); iter != m_trackVec.end(); ++iter){
		const SkeletonNodeTrack *pTrack = *iter;
		if(pTrack->GetName() == name) return pTrack;
	}
	return NULL;
}
//----------------------------------------------------------------------------------
void SkeletonAnimation::ApplyToEntity(float timePos, Entity *pEntity)
{
	Matrix4f transform;
	float timeInTicks = m_ticksPerSecond * timePos;
	float animationTime = fmodf(timeInTicks, m_ticks - 1);
	transform.InitIdentity();
	ReadNodeHeirarchy(animationTime, mp_skeleton->GetRootBoneNode(), transform, pEntity);
}
//--------------------------------------------------------------------------------------
void SkeletonAnimation::ReadNodeHeirarchy(float animationTime, SkeletonBone *pBone, const Matrix4f &parentTransform, Entity *pEntity)
{	
	Matrix4f nodeTransformation(pBone->GetTransform());
	const SkeletonNodeTrack *pTrack = GetTrack(pBone->GetName());

	if(pTrack) nodeTransformation = CalcuInterPolatedTransform(pTrack, animationTime);
	Matrix4f globalTranformation = parentTransform * nodeTransformation;
	if(pEntity->m_boneMapping.find(pBone->GetName()) != pEntity->m_boneMapping.end()){
		unsigned boneIndex = pEntity->m_boneMapping[pBone->GetName()];
		pEntity->m_boneInfo[boneIndex].m_finalTransformation = pEntity->m_globalInverseTransform * globalTranformation * pEntity->m_boneInfo[boneIndex].m_boneOffset;
	}
	for(unsigned i=0; i < pBone->NumChildren(); ++i){
		ReadNodeHeirarchy(animationTime, (SkeletonBone *)pBone->GetChild(i), globalTranformation, pEntity);
	}
}
//--------------------------------------------------------------------------------------
const Matrix4f SkeletonAnimation::CalcuInterPolatedTransform(const SkeletonNodeTrack *pTrack, float animationTime)
{
	Matrix4f trans;
	unsigned firstIndex = animationTime;
	unsigned secondIndex = animationTime + 1;
	float firstRadio = secondIndex - animationTime;
	float secondRadio = animationTime - firstIndex;

	Vector3f position = pTrack->GetPosition(firstIndex) * firstRadio + pTrack->GetPosition(secondIndex) * secondRadio;
	Vector3f scale = pTrack->GetScale(firstIndex) * firstRadio + pTrack->GetScale(secondIndex) * secondRadio;
	Quaternion rotate = pTrack->GetRotation(firstIndex) * firstRadio + pTrack->GetRotation(secondIndex) * secondRadio;

	trans.MakeTransform(position,scale,rotate);
	return trans;
}

