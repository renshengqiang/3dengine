#ifndef _ANIMATION_STATE_H
#define _ANIMATION_STATE_H
#include "Export.h"
#include <string>
#include <map>
#include <set>
/*
	AnimationState�������:�Զ�����״̬���й���
	������״̬:
	1. �������ͳ�����Ϣ(Animation��Ҳ�б��棬��ʵ���Ǵ�Animation�и��ƹ�����)
	2. ��ǰ��ʱ��λ��
	3. �����Ƿ�ʹ��
	4. �����Ƿ�ѭ������
*/
class AnimationStateSet;
class ENGINE_EXPORT AnimationState{
	public:
		AnimationState(const std::string &name, float length, AnimationStateSet *parent,float timePos=0.0, bool enabled=false);
		~AnimationState();
		const std::string &GetName(void) const;
		float GetTimePosition(void) const;
		void SetTimePosition(float timePos);
		float GetLength(void) const;
		void SetLength(float length);
		void AddTime(float offset);
		bool HasEnded(void) const;
		bool GetEnabled(void) const;
		void SetEnabled(bool enabled);
		void SetLoop(bool loop){ m_loop = loop;};
		bool GetLoop(void){ return m_loop;};
		
	private:
		AnimationStateSet *mp_parent;
		std::string m_name;
		float m_timePos;
		float m_length;
		bool m_enabled;
		bool m_loop;
};
class ENGINE_EXPORT AnimationStateSet{
	public:
		typedef std::map<std::string, AnimationState*> AnimationStateList;
		typedef AnimationStateList::iterator AnimationStateIterator;
		typedef std::set<AnimationState*> EnabledAnimationStateList;
		typedef EnabledAnimationStateList::iterator EnabledAnimationStateIterator;
		
		AnimationStateSet();
		~AnimationStateSet();
		AnimationState *CreateAnimationState(const std::string &name, float length, float timePos = 0.0, bool enabled = false);
		AnimationState *GetAnimationState(const std::string &name);
		bool HasAnimationState(const std::string &name) const;
		void RemoveAnimationState(const std::string &name);
		void RemoveAllAnimationStates(void);

		EnabledAnimationStateIterator _GetEnabledAnimationIteratorBegin(void);
		EnabledAnimationStateIterator _GetEnabledAnimationIeratorEnd(void);

		void _NotifyAnimationStateEnabled(AnimationState *target, bool enabled);
	protected:
		AnimationStateList m_animationStateList;
		EnabledAnimationStateList m_enabledAnimationStateList;
};

#endif
