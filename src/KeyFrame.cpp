#include "KeyFrame.h"
#include "AnimationTrack.h"

KeyFrame::KeyFrame(const AnimationTrack * parent,float time)
{
	mp_parentTrack = parent;
	m_time = time;
}
TransformKeyFrame::TransformKeyFrame(const AnimationTrack * parent,float time):
	KeyFrame(parent,time),m_translate(0.0f,0.0f,0.0f),
	m_scale(1.0f, 1.0f, 1.0f),m_rotate(0.0f, 0.0f,0.0f,1.0f)
{
}
void TransformKeyFrame::SetTranslate(const Vector3f & trans)
{
	m_translate = trans;
	return;
}
const Vector3f &TransformKeyFrame::GetTranslate(void) const
{
	return m_translate;
}
void TransformKeyFrame::SetRotation(const Quaternion & rotate)
{
	m_rotate = rotate;
	return;
}
const Quaternion &TransformKeyFrame::GetRotation(void) const
{
	return m_rotate;
}
void TransformKeyFrame::SetScale(const Vector3f & scale)
{
	m_scale = scale;
	return;
}
const Vector3f &TransformKeyFrame::GetScale(void) const
{
	return m_scale;
}