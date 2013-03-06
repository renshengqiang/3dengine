#include <AnimationState.h>
#include <math_3d.h>
#include <stdio.h>
#include <stdlib.h>

AnimationState::AnimationState(const  std::string & name,float length, AnimationStateSet *parent, float timePos, bool enabled)
	:mp_parent(parent),
	m_name(name),
	m_timePos(timePos), 
	m_length(length), 
	m_enabled(enabled)
{
}
AnimationState::~AnimationState()
{
}
const std::string &AnimationState::GetName(void) const
{
	return m_name;
}
float AnimationState::GetTimePosition(void) const
{
	return m_timePos;
}
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
float AnimationState::GetLength() const
{
	return m_length;
}
void AnimationState::SetLength(float length)
{
	m_length = length;
	return;
}
void AnimationState::AddTime(float offset)
{
	if(m_enabled)
		SetTimePosition(m_timePos + offset);
	return;
}
bool AnimationState::HasEnded(void) const
{
	return (m_timePos>= m_length && !m_loop);
}
bool AnimationState::GetEnabled(void) const
{
	return m_enabled;
}
void AnimationState::SetEnabled(bool enabled)
{
	if(mp_parent)
		mp_parent->_NotifyAnimationStateEnabled(this,enabled);
	m_enabled = enabled;
	return;
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
AnimationStateSet::AnimationStateSet()
{
	gd_list_init_head(&m_animationStateHead);
	gd_list_init_head(&m_enabledAnimationStateHead);
}
AnimationStateSet::~AnimationStateSet()
{
	RemoveAllAnimationStates();
}
AnimationState *AnimationStateSet::CreateAnimationState(const std::string &name, float length, float timePos, bool enabled)
{
	if(HasAnimationState(name)){
		fprintf(stderr, "AnimationStateSet::CreateAnimationState : ERROR Creating a AnimationState : State for animation named %s already exist\n", name.c_str());
		return NULL;
	}
	AnimationState *newState = new AnimationState(name, length, this, timePos, enabled);
	struct AnimationStateListNode *listNode = (struct AnimationStateListNode *)malloc(sizeof(struct AnimationStateListNode));
	listNode->animationState = newState;
	gd_list_add_tail(&(listNode->siblingList),&m_animationStateHead);
	if(enabled){
		gd_list_add_tail(&(listNode->siblingList),&m_enabledAnimationStateHead);
	}
	else{
		gd_list_init_node(&(listNode->enabledSiblingList));
	}
	return newState;
}
AnimationState *AnimationStateSet::GetAnimationState(const std::string &name)
{
	GD_LIST *pos;
	struct AnimationStateListNode *listNode;

	gd_list_for_each(pos,&m_animationStateHead){
		listNode = (struct AnimationStateListNode *)GD_ENTRY(pos,struct AnimationStateListNode,siblingList);
		if(name == listNode->animationState->GetName())
			return listNode->animationState;
	}
	return NULL;
}
bool AnimationStateSet::HasAnimationState(const std::string &name) const
{
	GD_LIST *pos;
	struct AnimationStateListNode *listNode;

	gd_list_for_each(pos,&m_animationStateHead){
		listNode = (struct AnimationStateListNode *)GD_ENTRY(pos,struct AnimationStateListNode,siblingList);
		if(name == listNode->animationState->GetName()) return true;
	}
	return false;
}
void AnimationStateSet::RemoveAnimationState(const std::string &name)
{
	GD_LIST *pos;
	struct AnimationStateListNode *listNode;

	gd_list_for_each(pos, &m_animationStateHead){
		listNode = (struct AnimationStateListNode *)GD_ENTRY(pos,struct AnimationStateListNode,siblingList);
		if(name == listNode->animationState->GetName()){
			gd_list_del(&(listNode->siblingList));
			if(listNode->animationState->GetEnabled())
				gd_list_del(&(listNode->enabledSiblingList));
			delete listNode->animationState;
			free(listNode);
			break;
		}//if
	}//gd_list_for_each
	return;
}
void AnimationStateSet::RemoveAllAnimationStates(void)
{
	GD_LIST *pos, *temp;
	struct AnimationStateListNode *listNode;

	gd_list_for_each_safe(pos,temp,&m_animationStateHead){
		listNode = (struct AnimationStateListNode *)GD_ENTRY(pos,struct AnimationStateListNode,siblingList);
		gd_list_del(&(listNode->siblingList));
		if(listNode->animationState->GetEnabled())
			gd_list_del(&(listNode->enabledSiblingList));
		delete listNode->animationState;
		free(listNode);
	}
	return;
}
bool AnimationStateSet::HasEnabledAnimationState(void) const
{
	return gd_list_empty(&m_enabledAnimationStateHead);
}
int AnimationStateSet::GetEnabledAnimationStateNum(void) const
{
	GD_LIST *pos;
	int n = 0;

	gd_list_for_each(pos,&m_enabledAnimationStateHead)  ++n;
	return n;
}
AnimationState *AnimationStateSet::GetEnabledAnimationState(int index)
{
	GD_LIST *pos;
	struct AnimationStateListNode *listNode;
	int i=0;

	gd_list_for_each(pos, &m_enabledAnimationStateHead){
		if(i==index){
			listNode = (struct AnimationStateListNode *)GD_ENTRY(pos,struct AnimationStateListNode,enabledSiblingList);
			return listNode->animationState;
		}
		++i;
	}
	fprintf(stderr, "AnimationState::GetEnabledAnimationState : index out of bound\n");
	return NULL;
}
void AnimationStateSet::_NotifyAnimationStateEnabled(AnimationState *target, bool enabled)
{
	GD_LIST *pos;
	struct AnimationStateListNode *listNode;

	gd_list_for_each(pos, &m_animationStateHead){
		listNode = (struct AnimationStateListNode *)GD_ENTRY(pos,struct AnimationStateListNode,siblingList);
		if(target == listNode->animationState){
			if(enabled != listNode->animationState->GetEnabled()){
				if(enabled)
					gd_list_add_tail(&(listNode->enabledSiblingList),&m_enabledAnimationStateHead);
				else
					gd_list_del(&(listNode->enabledSiblingList));
			}//if(enabled
		}//if(target
	}
	return;
}
