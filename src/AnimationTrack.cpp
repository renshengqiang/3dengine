#include "AnimationTrack.h"
#include "Animation.h"
#include <stdlib.h>

AnimationTrack::AnimationTrack(Animation * parent)
	:mp_parent(parent)
{
	GdArrayInit(&m_keyFrameArray,sizeof(KeyFrame *));
}
AnimationTrack::~AnimationTrack()
{
	RemoveAllKeyFrames();
}
unsigned short AnimationTrack::GetNumKeyFrames(void) const
{
	return GD_ARRAY_LENGTH(m_keyFrameArray);
}
KeyFrame *AnimationTrack::GetKeyFrame(unsigned short index)
{
	KeyFrame *kf;
	if(index<GD_ARRAY_LENGTH(m_keyFrameArray)){
		kf = *(KeyFrame**)GdArrayGet(&m_keyFrameArray,index);
		return kf;
	}else return NULL;
}
KeyFrame *AnimationTrack::CreateKeyFrame(float timePos)
{
	KeyFrame *kf = _CreateKeyFrameImpl(timePos);
	KeyFrame *temp1,**temp2;
	int i;

	for(i=0;i < GD_ARRAY_LENGTH(m_keyFrameArray); ++i){
		temp1 = *(KeyFrame **)GdArrayGet(&m_keyFrameArray,i);
		if(timePos<temp1->GetTime()) break;
	}
	temp2 = (KeyFrame **)GdArrayAdd(&m_keyFrameArray,i,1);
	*temp2 = kf;
	return kf;	
}
void AnimationTrack::RemoveKeyFrame(unsigned short index)
{
	KeyFrame *keyFrame = 0;
	if(index<GD_ARRAY_LENGTH(m_keyFrameArray)){
		keyFrame = *(KeyFrame **)GdArrayGet(&m_keyFrameArray, index);
		delete keyFrame;
		GdArrayDelete(&m_keyFrameArray,index,1);
	}
	return;
}
void AnimationTrack::RemoveAllKeyFrames(void)
{
	KeyFrame *keyFrame = 0;
	for(int i=0;i<GD_ARRAY_LENGTH(m_keyFrameArray);++i){
		keyFrame = *(KeyFrame **)GdArrayGet(&m_keyFrameArray,i);
		if(keyFrame) delete keyFrame;
	}
	GdArrayReset(&m_keyFrameArray);	
	return;
}
/*
	������������зֱ���:
	1. Animation��ʱ�� == ���һ��keyFrame��ʱ�� + Animation��ʱ�� != ���һ��keyFrame��ʱ�䣬����timePos�ڴ�����keyFrame��ʱ��ֵ֮��
	2. Animation��ʱ�� != ���һ��keyFrame��ʱ�� ��timePos���������һ��keyFrame��ʱ��
	����:
	1. ������:
		1.1 ���ص�keyFrame1 == keyFrame2,��ʾ�ڵ�һ֮֡ǰ������timePos/keyFrame2��ʱ��
		1.2 ���ص�keyFrame1 != keyFrame2,��ʾ����������֮֡�䣬����(timePos - keyFrame1��ʱ��)/(keyFram2��ʱ��/keyFrame1��ʱ��)
	2.���һ֡��ΪkeyFrame1,  ��һ֡��ΪkeyFrame2,���� = (timePos - ���һ֡��ʱ��)/(�������� + ��һ֡ʱ��)
*/
float AnimationTrack::GetKeyFrameAtTime(float timePos, KeyFrame **keyFrame1, KeyFrame **keyFrame2)
{
	float totalAnimationLength = mp_parent->GetLength();
	KeyFrame *temp;
	int i; 
	float t1,t2;
	assert(totalAnimationLength > 0.0f && "Invalid animation length!");

	if(timePos > totalAnimationLength && totalAnimationLength> 0.0f)
		timePos = fmodf(timePos, totalAnimationLength);
	for(i=0;i<GD_ARRAY_LENGTH(m_keyFrameArray);++i){
		temp = *(KeyFrame **)GdArrayGet(&m_keyFrameArray,i);
		if(timePos<temp->GetTime()) break;
	}
	//���1
	if(i==GD_ARRAY_LENGTH(m_keyFrameArray)){
		*keyFrame1 = *(KeyFrame **)GdArrayGet(&m_keyFrameArray,GD_ARRAY_LENGTH(m_keyFrameArray)-1);
		*keyFrame2 = *(KeyFrame **)GdArrayGet(&m_keyFrameArray,0);
		t1 = (*keyFrame1)->GetTime();
		t2 = mp_parent->GetLength() + (*keyFrame2)->GetTime();
	}
	//���2
	else
	{
		*keyFrame2 = *(KeyFrame **)GdArrayGet(&m_keyFrameArray,i);
		t2=(*keyFrame2)->GetTime();
		if(i!=0)--i;
		*keyFrame1 = *(KeyFrame **)GdArrayGet(&m_keyFrameArray, i);
		t1=(*keyFrame1)->GetTime();
	}
	if(t1==t2) return timePos/t2;
	else return (timePos-t1)/(t2-t1);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
NodeAnimationTrack::NodeAnimationTrack(Animation *parent)
	:AnimationTrack(parent),mp_targetNode(0),m_useShortestRotationPath(true)
{
}
NodeAnimationTrack::NodeAnimationTrack(Animation *parent, SceneNode *targetNode)
	:AnimationTrack(parent),mp_targetNode(targetNode),m_useShortestRotationPath(true)
{
}
SceneNode *NodeAnimationTrack::GetAssociatedNode(void) const
{
	return mp_targetNode;
}
void NodeAnimationTrack::SetAssociatedNode(SceneNode *node)
{
	mp_targetNode = node;
	return;	
}
bool NodeAnimationTrack::GetUseShortestPath(void) const
{
	return m_useShortestRotationPath;
}
void NodeAnimationTrack::SetUseShortestPath(bool useShortestPath)
{
	m_useShortestRotationPath = useShortestPath;
}
TransformKeyFrame *NodeAnimationTrack::CreateNodeKeyFrame(float timePos)
{
	return static_cast<TransformKeyFrame*>(CreateKeyFrame(timePos));
}
KeyFrame *NodeAnimationTrack::_CreateKeyFrameImpl(float timePos)
{
	TransformKeyFrame *keyFrame = new TransformKeyFrame(this,timePos);
	return keyFrame;
}	
/*
	����GetKeyFrameAtTime�õ�����KeyFrame,������������keyFrame���в�ֵ
	ע�� : ������GetKeyFrameAtTime�õ�������KeyFrame��ͬ����KeyFrame1�ĸ�����������Ϊ"��λ" KeyFrame
*/
void NodeAnimationTrack::GetInterpolatedKeyFrame(float timePos,KeyFrame * kf)
{
	TransformKeyFrame *keyFrameRet = static_cast<TransformKeyFrame*>(kf);
	KeyFrame *kBase1,*kBase2;
	TransformKeyFrame *k1, *k2;
	Quaternion k1Quaternion(0,0,0,1);
	Vector3f k1Translate(0,0,0);
	Vector3f k1Scale(1,1,1);

	float rate2 = this->GetKeyFrameAtTime(timePos,&kBase1,&kBase2);
	k1 = static_cast<TransformKeyFrame*>(kBase1);
	k2 = static_cast<TransformKeyFrame*>(kBase2);
	/*
		���ﱾ��Ӧ���ǰ����û����õĲ�ֵ��ʽ���в�ֵ
		Ŀǰ�Ҷ�splineû��ʲô��ʶ������Ȳ�ȡ��򵥵ķ�ʽ���в�ֵ�������Բ�ֵ
		����ת�Ĳ�ֵ������m_useShortestPathѡ���ǰ���ֱ�ߵķ�ʽ���в�ֵ���ǰ������ߵķ�ʽ
	*/
	if(k1!=k2){
		k1Quaternion = k1->GetRotation();
		k1Translate = k1->GetTranslate();
		k1Scale = k1->GetScale();
	}
	keyFrameRet->SetRotation(Quaternion::nlerp(rate2, k1Quaternion,k2->GetRotation(), m_useShortestRotationPath));
	keyFrameRet->SetTranslate(k1Translate*(1-rate2) + k2->GetTranslate()*rate2);
	keyFrameRet->SetScale(k1Scale*(1-rate2) + k2->GetScale()*rate2);
	return;
}
void NodeAnimationTrack::Apply(float timePos,float weight,float scale)
{
	ApplyToNode(mp_targetNode,timePos,weight,scale);
	return;
}
void NodeAnimationTrack::ApplyToNode(SceneNode *node, float timePos, float weight, float scl)
{
	TransformKeyFrame kf(0,timePos);

	GetInterpolatedKeyFrame(timePos,&kf);

	Vector3f translate = kf.GetTranslate()*(weight*scl);
	node->Translate(translate);
	Quaternion rotate = Quaternion::nlerp(weight,Quaternion(0,0,0,1),kf.GetRotation(),m_useShortestRotationPath);
	node->Rotate(rotate);
	Vector3f scale = kf.GetScale();
	if(!(scale==Vector3f(1,1,1))){
		if(scl != 1.0f)
			scale = Vector3f(1,1,1) + (scale - Vector3f(1,1,1)) * scl;
		else if(weight!=1.0f)
			scale = Vector3f(1,1,1) + (scale - Vector3f(1,1,1)) * weight;
	}
	node->Scale(scale);
	return;
}
