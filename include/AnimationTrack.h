#ifndef _ANIMATION_TRACK_H
#define _ANIMATION_TRACK_H
#include "Export.h"
#include "KeyFrame.h"
#include "Node.h"
#include "SimpleSpline.h"
#include "RotationalSpline.h"
#include <string>
#include <vector>
/*
	AnimationTrack�������:
	1. ��Track�е�ʱ����صĲ������з�װ��
	2. Ϊ����Ӧ�������͵ĺͽڵ���صĶ�������������������Ϊ���ֽڵ���ض�����ĸ���
*/
class Animation;
class ENGINE_EXPORT AnimationTrack{
public:
	typedef std::vector<KeyFrame*> KeyFrameList;
	typedef KeyFrameList::iterator KeyFrameIterator;
	typedef KeyFrameList::reverse_iterator KeyFrameReverseIterator;
	
	AnimationTrack(Animation *parent);
	~AnimationTrack();
	virtual unsigned short GetNumKeyFrames(void) const;
	virtual KeyFrame *GetKeyFrame(unsigned short index);
	/*
		���ݸ�����ʱ�䴴��һ��KeyFrame,������m_keyFrameArray������
		m_keyFrameArray�����е�keyFrame�Ǹ������ǵ�timePos�����������
	*/
	virtual KeyFrame *CreateKeyFrame(float timePos);
	virtual void RemoveKeyFrame(unsigned short index);
	virtual void RemoveAllKeyFrames(void);
	/*
		��ȡָ��timePos��������KeyFrame,����������KeyFrame֮��Ļ��ֵ(������)
	@remarks
		������ʱ���ܶ�Ӧ�Ŷ����е�1֡����2֡:1 timePos���ö�Ӧĳ��keyFrame��ʱ�䣬2 ��1֮�����������
		��֡�ֱ��ӦtimePosǰ��һ֡��timePos�����һ֡
		�ú�����������ʱ��ʱ�̶�Ӧ�Ĺؼ�֡��ͬʱ����һ��������ʾ���ص�����֮֡��Ĳ�ֵ������
		����:�������0��ʾ����ʱ�����ö�ӦkeyFrame1,0.5���ʾ��kerFrame1��keyFrame2֮��,�����ص�����ʾkeyFrame2��ռ�ı���
		
	*/
	virtual float GetKeyFrameAtTime(float timePos, KeyFrame **keyFrame1, KeyFrame **keyFrame2, int *index);
	/*
		��ȡtimePosʱ�䴦��KeyFrame��Ϣ����KeyFrame�б�����timePos��Ӧ�ñ����trans��Ϣ
	@param
		timePos:ָ��ĳʱ��㴦�Ķ���
		kf:�õ���KeyFrame�Ĵ��λ��
	*/
	virtual void GetInterpolatedKeyFrame(float timePos, KeyFrame *kf) = 0;
	virtual void Apply(float timePos, float weight = 1.0, float scale = 1.0) = 0;
	Animation *GetParent() const { return mp_parent;}
protected:
	Animation *mp_parent;
	KeyFrameList m_keyFrameList;
	
	virtual KeyFrame *_CreateKeyFrameImpl(float timePos) = 0;
	virtual void _KeyFrameDataChanged(void) const {}
};
/*
	NodeAnimationTrack�������: ������NodeAnimation������(λ�ƣ���ת������)���з�װ�͹���
*/
class ENGINE_EXPORT NodeAnimationTrack : public AnimationTrack
{
public:
	NodeAnimationTrack(Animation *parent, const std::string &name="");
	NodeAnimationTrack(Animation *parent, Node *targetNode, const std::string &name="");
	~NodeAnimationTrack();
	virtual Node *GetAssociatedNode(void) const;
	virtual void SetAssociatedNode(Node *node);
	virtual bool GetUseShortestPath(void) const;
	virtual void SetUseShortestPath(bool useShortestPath);
	virtual TransformKeyFrame *CreateNodeKeyFrame(float timePos);
	virtual void GetInterpolatedKeyFrame(float timePos,KeyFrame * kf);
	virtual void Apply(float timePos,float weight = 1.0,float scale = 1.0);
	virtual void ApplyToNode(Node *node, float timePos, float weight=1.0, float scale=1.0);
private:
	std::string m_name;
	// Struct for store splines, allocate on demand for better memory footprint
        struct Splines
        {
	    SimpleSpline positionSpline;
	    SimpleSpline scaleSpline;
	    RotationalSpline rotationSpline;
        };
	Node *mp_targetNode;
	// Prebuilt splines, must be mutable since lazy-update in const method
	mutable Splines* mp_splines;
	mutable bool m_useShortestRotationPath;
	mutable bool m_splineBuildNeeded;

	KeyFrame *_CreateKeyFrameImpl(float timePos);
	virtual void _KeyFrameDataChanged(void) const;
	void _BuildInterpolationSplines(void) const;
};
#endif
