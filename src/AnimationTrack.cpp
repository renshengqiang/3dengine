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
	按两种情况进行分别处理:
	1. Animation的时间 == 最后一个keyFrame的时间 + Animation的时间 != 最后一个keyFrame的时间，但是timePos在创建的keyFrame的时间值之间
	2. Animation的时间 != 最后一个keyFrame的时间 且timePos超出了最后一个keyFrame的时间
	策略:
	1. 分两种:
		1.1 返回的keyFrame1 == keyFrame2,表示在第一帧之前，返回timePos/keyFrame2的时间
		1.2 返回的keyFrame1 != keyFrame2,表示正常的在两帧之间，返回(timePos - keyFrame1的时间)/(keyFram2的时间/keyFrame1的时间)
	2.最后一帧作为keyFrame1,  第一帧作为keyFrame2,比例 = (timePos - 最后一帧的时间)/(动画长度 + 第一帧时间)
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
	//情况1
	if(i==GD_ARRAY_LENGTH(m_keyFrameArray)){
		*keyFrame1 = *(KeyFrame **)GdArrayGet(&m_keyFrameArray,GD_ARRAY_LENGTH(m_keyFrameArray)-1);
		*keyFrame2 = *(KeyFrame **)GdArrayGet(&m_keyFrameArray,0);
		t1 = (*keyFrame1)->GetTime();
		t2 = mp_parent->GetLength() + (*keyFrame2)->GetTime();
	}
	//情况2
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
	调用GetKeyFrameAtTime得到两个KeyFrame,并根据这两个keyFrame进行插值
	注意 : 若调用GetKeyFrameAtTime得到的两个KeyFrame相同，则将KeyFrame1的各个变量设置为"单位" KeyFrame
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
		这里本来应该是按照用户设置的插值方式进行插值
		目前我对spline没有什么认识，因此先采取最简单的方式进行插值，即线性插值
		对旋转的插值，根据m_useShortestPath选择是按照直线的方式进行插值还是按照曲线的方式
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
