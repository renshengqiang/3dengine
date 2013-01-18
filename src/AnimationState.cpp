#include <AnimationState.h>
#include <math_3d.h>

AnimationState::AnimationState(const  std::string & name,float length, float timePos, bool enabled)
	:m_name(name),m_timePos(timePos), m_length(length), m_enabled(enabled)
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
void AnimationState::SetEnabled(bool enalbed)
{
	m_enabled = enalbed;
	return;
}