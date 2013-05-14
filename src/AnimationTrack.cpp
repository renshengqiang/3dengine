#include "AnimationTrack.h"
#include "Animation.h"

//-----------------------------------------------------------------------
AnimationTrack::AnimationTrack(Animation * parent)
	:mp_parent(parent)
{
}
//-----------------------------------------------------------------------
AnimationTrack::~AnimationTrack()
{
	RemoveAllKeyFrames();
}
//-----------------------------------------------------------------------
unsigned short AnimationTrack::GetNumKeyFrames(void) const
{
	return m_keyFrameList.size();
}
//-----------------------------------------------------------------------
KeyFrame *AnimationTrack::GetKeyFrame(unsigned short index)
{
	if(index < m_keyFrameList.size())
	{
		return m_keyFrameList[index];
	}
	return NULL;
}
//-----------------------------------------------------------------------
KeyFrame *AnimationTrack::CreateKeyFrame(float timePos)
{
	KeyFrame *kf = _CreateKeyFrameImpl(timePos);
	KeyFrameReverseIterator iter;
	
	//采用插入排序的方法
	m_keyFrameList.push_back(kf);
	for(iter = m_keyFrameList.rbegin() +1; iter != m_keyFrameList.rend(); ++iter)
	{
		if((*iter)->GetTime() > timePos)
		{
			*(iter - 1) = *iter;
		}
		else
		{
			--iter;//就是插在iter这个位置
			break;
		}
	}
	if(iter == m_keyFrameList.rend())
	{
		m_keyFrameList[0] = kf;
	}
	else
	{
		*iter = kf;
	}
	return kf;	
}
//-----------------------------------------------------------------------
void AnimationTrack::RemoveKeyFrame(unsigned short index)
{
	if(index < m_keyFrameList.size())
	{
		KeyFrameIterator iter = m_keyFrameList.begin();
		while(index--) ++iter;
		delete *iter;
		m_keyFrameList.erase(iter);
	}
	return;
}
//-----------------------------------------------------------------------
void AnimationTrack::RemoveAllKeyFrames(void)
{
	for(KeyFrameIterator iter = m_keyFrameList.begin(); iter != m_keyFrameList.end(); ++iter)
	{
		KeyFrame *pKeyFrame = *iter;
		delete pKeyFrame;
	}
	m_keyFrameList.clear();
	return;
}
//-----------------------------------------------------------------------
/*
	按两种情况进行分别处理:
	1. Animation的时间 == 最后一个keyFrame的时间 和 Animation的时间 != 最后一个keyFrame的时间，但是timePos在创建的keyFrame的时间值之间
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
	float t1,t2;
	unsigned i; 
	assert(totalAnimationLength > 0.0f && "Invalid animation length!");

	if(timePos > totalAnimationLength && totalAnimationLength> 0.0f)
		timePos = fmodf(timePos, totalAnimationLength);
	
	for(i=0; i<m_keyFrameList.size(); ++i)
	{
		if(m_keyFrameList[i]->GetTime() > timePos) break;
	}
	//情况1, 一般不会遇到
	if(i == m_keyFrameList.size())
	{
		*keyFrame1 = m_keyFrameList[m_keyFrameList.size() - 1];
		*keyFrame2 = m_keyFrameList[0];
		t1 = (*keyFrame1)->GetTime();
		t2 = mp_parent->GetLength() + (*keyFrame2)->GetTime();
	}
	//情况2
	else
	{
		*keyFrame2 = m_keyFrameList[i];
		t2=(*keyFrame2)->GetTime();
		if(i!=0)--i;
		*keyFrame1 = m_keyFrameList[i];
		t1=(*keyFrame1)->GetTime();
	}
	if(t1==t2) return timePos/t2;
	else return (timePos-t1)/(t2-t1);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
NodeAnimationTrack::NodeAnimationTrack(Animation *parent, const std::string &name)
	:AnimationTrack(parent),
	m_name(name),
	mp_targetNode(0),
	m_useShortestRotationPath(false)
{
}
//-----------------------------------------------------------------------------
NodeAnimationTrack::NodeAnimationTrack(Animation *parent, Node *targetNode, const std::string &name)
	:AnimationTrack(parent),
	m_name(name),
	mp_targetNode(targetNode),
	m_useShortestRotationPath(false)
{
}
//-----------------------------------------------------------------------------
Node *NodeAnimationTrack::GetAssociatedNode(void) const
{
	return mp_targetNode;
}
//-----------------------------------------------------------------------------
void NodeAnimationTrack::SetAssociatedNode(Node *node)
{
	mp_targetNode = node;
	return;	
}
//-----------------------------------------------------------------------------
bool NodeAnimationTrack::GetUseShortestPath(void) const
{
	return m_useShortestRotationPath;
}
//-----------------------------------------------------------------------------
void NodeAnimationTrack::SetUseShortestPath(bool useShortestPath)
{
	m_useShortestRotationPath = useShortestPath;
}
//-----------------------------------------------------------------------------
TransformKeyFrame *NodeAnimationTrack::CreateNodeKeyFrame(float timePos)
{
	return static_cast<TransformKeyFrame*>(CreateKeyFrame(timePos));
}
//-----------------------------------------------------------------------------
KeyFrame *NodeAnimationTrack::_CreateKeyFrameImpl(float timePos)
{
	TransformKeyFrame *keyFrame = new TransformKeyFrame(this,timePos);
	return keyFrame;
}	
//-----------------------------------------------------------------------------
/*
	调用GetKeyFrameAtTime得到两个KeyFrame,并根据这两个keyFrame进行插值
	注意 : 若调用GetKeyFrameAtTime得到的两个KeyFrame相同，则将KeyFrame1的各个变量设置为"单位" KeyFrame
	这是因为我们没有为所有的动画创建默认的第一帧，而OGRE是默认的添加了第一帧的
*/
void NodeAnimationTrack::GetInterpolatedKeyFrame(float timePos, KeyFrame *kf)
{
	TransformKeyFrame *keyFrameRet = static_cast<TransformKeyFrame*>(kf);
	KeyFrame *kBase1,*kBase2;
	float rate2 = this->GetKeyFrameAtTime(timePos,&kBase1,&kBase2);
	
	TransformKeyFrame *k1 = static_cast<TransformKeyFrame*>(kBase1);
	TransformKeyFrame *k2 = static_cast<TransformKeyFrame*>(kBase2);

	Quaternion k1Quaternion(0,0,0,1);
	Vector3f k1Translate(0,0,0);
	Vector3f k1Scale(1,1,1);
	if(k1!=k2)
	{
		k1Quaternion = k1->GetRotation();
		k1Translate = k1->GetTranslate();
		k1Scale = k1->GetScale();
	}

	Animation::InterpolationMode im = mp_parent->GetInterpolationMode();
	Animation::RotationInterpolationMode rim = mp_parent->GetRotationInterpolationMode();
	switch(im){
		case Animation::IM_LINEAR:
			if(rim == Animation::RIM_LINEAR)
			{
				keyFrameRet->SetRotation(Quaternion::slerp(rate2, k1Quaternion, k2->GetRotation(), m_useShortestRotationPath));
			}
			else
			{
				keyFrameRet->SetRotation(Quaternion::nlerp(rate2, k1Quaternion, k2->GetRotation(), m_useShortestRotationPath));
			}
			keyFrameRet->SetTranslate(k1Translate*(1-rate2) + k2->GetTranslate()*rate2);
			keyFrameRet->SetScale(k1Scale*(1-rate2) + k2->GetScale()*rate2);
			break;
		case Animation::IM_SPLINE:
			//todo: add spline interpolation support
			break;
	}
	return;
}
//-----------------------------------------------------------------------
void NodeAnimationTrack::Apply(float timePos,float weight,float scale)
{
	ApplyToNode(mp_targetNode,timePos,weight,scale);
	return;
}
//-----------------------------------------------------------------------
void NodeAnimationTrack::ApplyToNode(Node *node, float timePos, float weight, float scl)
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
