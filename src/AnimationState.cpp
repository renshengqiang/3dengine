#include <AnimationState.h>
#include <math_3d.h>
#include <stdio.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
AnimationState::AnimationState(const  std::string & name,float length, AnimationStateSet *parent, float timePos, bool enabled)
	:mp_parent(parent),
	m_name(name),
	m_timePos(timePos), 
	m_length(length), 
	m_enabled(enabled)
{
}
//-----------------------------------------------------------------------------
AnimationState::~AnimationState()
{
}
//-----------------------------------------------------------------------------
const std::string &AnimationState::GetName(void) const
{
	return m_name;
}
//-----------------------------------------------------------------------------
float AnimationState::GetTimePosition(void) const
{
	return m_timePos;
}
//-----------------------------------------------------------------------------
void AnimationState::SetTimePosition(float timePos)
{
	if (timePos != m_timePos)
	{
		m_timePos = timePos;
		if (m_loop)
		{
			// Wrap
			m_timePos = fmodf(m_timePos, m_length);
			if(m_timePos < 0)
				m_timePos += m_length;     
		}
		else
		{
			// Clamp
			if(m_timePos < 0)
				m_timePos = 0;
			else if (m_timePos > m_length)
				m_timePos = m_length;
		}
    }
	return;
}
//-----------------------------------------------------------------------------
float AnimationState::GetLength() const
{
	return m_length;
}
//-----------------------------------------------------------------------------
void AnimationState::SetLength(float length)
{
	m_length = length;
	return;
}
//-----------------------------------------------------------------------------
void AnimationState::AddTime(float offset)
{
	if(m_enabled)
		SetTimePosition(m_timePos + offset);
	return;
}
//-----------------------------------------------------------------------------
bool AnimationState::HasEnded(void) const
{
	return (m_timePos>= m_length && !m_loop);
}
//-----------------------------------------------------------------------------
bool AnimationState::GetEnabled(void) const
{
	return m_enabled;
}
//-----------------------------------------------------------------------------
void AnimationState::SetEnabled(bool enabled)
{
	if(mp_parent)
		mp_parent->_NotifyAnimationStateEnabled(this,enabled);
	m_enabled = enabled;
	return;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AnimationStateSet::AnimationStateSet()
{
}
//-----------------------------------------------------------------------------
AnimationStateSet::~AnimationStateSet()
{
	RemoveAllAnimationStates();
}
//-----------------------------------------------------------------------------
AnimationState *AnimationStateSet::CreateAnimationState(const std::string &name, float length, float timePos, bool enabled)
{
	if(HasAnimationState(name)){
		fprintf(stderr, "AnimationStateSet::CreateAnimationState : ERROR Creating a AnimationState : State for animation named %s already exist\n", name.c_str());
		return NULL;
	}
	AnimationState *newState = new AnimationState(name, length, this, timePos, enabled);
	m_animationStateList.insert(std::make_pair(name, newState));
	if(enabled) m_enabledAnimationStateList.insert(newState);
	return newState;
}
//-----------------------------------------------------------------------------
AnimationState *AnimationStateSet::GetAnimationState(const std::string &name)
{
	AnimationStateIterator iter = m_animationStateList.find(name);
	if(iter != m_animationStateList.end())
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}
//-----------------------------------------------------------------------------
bool AnimationStateSet::HasAnimationState(const std::string &name) const
{
	return m_animationStateList.count(name) > 0;
}
//-----------------------------------------------------------------------------
void AnimationStateSet::RemoveAnimationState(const std::string &name)
{
	AnimationStateIterator iter = m_animationStateList.find(name);
	if(iter != m_animationStateList.end())
	{
		AnimationState *pState = iter->second;
		if(pState->GetEnabled())
		{
			EnabledAnimationStateIterator iter1 = m_enabledAnimationStateList.find(pState);
			if(iter1 != m_enabledAnimationStateList.end())
			{
				m_enabledAnimationStateList.erase(iter1);
			}
		}
		delete pState;
		m_animationStateList.erase(iter);
	}
	return;
}
//-----------------------------------------------------------------------------
void AnimationStateSet::RemoveAllAnimationStates(void)
{
	for(AnimationStateIterator iter = m_animationStateList.begin();
		iter != m_animationStateList.end(); ++iter)
	{
		delete iter->second;
	}
	m_animationStateList.clear();
	m_enabledAnimationStateList.clear();
	return;
}
//-----------------------------------------------------------------------------
AnimationStateSet::EnabledAnimationStateIterator 
	AnimationStateSet::_GetEnabledAnimationIteratorBegin(void)
{
	return m_enabledAnimationStateList.begin();
}
//-----------------------------------------------------------------------------
AnimationStateSet::EnabledAnimationStateIterator 
	AnimationStateSet::_GetEnabledAnimationIeratorEnd(void)
{
	return m_enabledAnimationStateList.end();
}
//-----------------------------------------------------------------------------
void AnimationStateSet::_NotifyAnimationStateEnabled(AnimationState *target, bool enabled)
{
	if(target==NULL) return;
	
	if(target->GetEnabled() != enabled)
	{
		if(enabled)
		{
			m_enabledAnimationStateList.insert(target);
		}
		else
		{
			EnabledAnimationStateIterator iter = m_enabledAnimationStateList.find(target);
			if(iter != m_enabledAnimationStateList.end())
			{
				m_enabledAnimationStateList.erase(iter);
			}
		}
	}
	return;
}
