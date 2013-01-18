#ifndef _ANIMATION_TRACK_H
#define _ANIMATION_TRACK_H
#include <Export.h>
#include <KeyFrame.h>
#include <SceneNode.h>
#include <struct.h>

/*
	Animation�������:
	1. ��Track�е�ʱ����صĲ������з�װ��
	2. Ϊ����Ӧ�������͵ĺͽڵ���صĶ�������������������Ϊ���ֽڵ���ض�����ĸ���
*/
class Animation;
class ENGINE_EXPORT AnimationTrack{
	public:
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
		virtual float GetKeyFrameAtTime(float timePos, KeyFrame **keyFrame1, KeyFrame **keyFrame2);
		/*
			��ȡtimePosʱ�䴦��KeyFrame��Ϣ����KeyFrame�б�����timePos��Ӧ�ñ����trans��Ϣ
		@param
			timePos:ָ��ĳʱ��㴦�Ķ���
			kf:�õ���KeyFrame�Ĵ��λ��
		*/
		virtual void GetInterpolatedKeyFrame(float timePos, KeyFrame *kf) = 0;
		virtual void Apply(float timePos, float weight = 1.0, float scale = 1.0) = 0;
		Animation *GetParent() const { return mp_parent;};
	protected:
		GD_ARRAY m_keyFrameArray;
		Animation *mp_parent;

		virtual KeyFrame *_CreateKeyFrameImpl(float timePos) = 0;
};



/*
	NodeAnimationTrack�������:������NodeAnimation������(λ�ƣ���ת������)���з�װ�͹���
*/
class ENGINE_EXPORT NodeAnimationTrack : public AnimationTrack
{
	public:
		NodeAnimationTrack(Animation *parent);
		NodeAnimationTrack(Animation *parent, SceneNode *targetNode);
		virtual SceneNode *GetAssociatedNode(void) const;
		virtual void SetAssociatedNode(SceneNode *node);
		virtual bool GetUseShortestPath(void) const;
		virtual void SetUseShortestPath(bool useShortestPath);
		virtual TransformKeyFrame *CreateNodeKeyFrame(float timePos);
		virtual void GetInterpolatedKeyFrame(float timePos,KeyFrame * kf);
		virtual void Apply(float timePos,float weight = 1.0,float scale = 1.0);
		virtual void ApplyToNode(SceneNode *node, float timePos, float weight=1.0, float scale=1.0);
	private:
		KeyFrame *_CreateKeyFrameImpl(float timePos);
		SceneNode *mp_targetNode;
		bool m_useShortestRotationPath;
};
#endif
