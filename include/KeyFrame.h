#ifndef _KEYFRAME_H
#define _KEYFRAME_H
#include <Export.h>
#include <math_3d.h>

/*
	将这两个类进行这样组织的原因:为了和Track类相适应
	AnimationTrack类使用KeyFrame;
	NodeAnimation类使用TransformKeyFrame类
*/
class AnimationTrack;
class ENGINE_EXPORT KeyFrame{
	public:
		KeyFrame(const AnimationTrack *parent,float time);
		virtual ~KeyFrame(){};
		float GetTime(void) const { return m_time;};

	protected:
		float m_time;
		const AnimationTrack *mp_parentTrack;
};
class ENGINE_EXPORT TransformKeyFrame:public KeyFrame
{
	public:
		TransformKeyFrame(const AnimationTrack *parent, float time);
		~TransformKeyFrame(){};

		virtual void SetTranslate(const Vector3f &trans);
		const Vector3f &GetTranslate(void) const;
		virtual void SetRotation(const Quaternion&rotate);
		const Quaternion &GetRotation(void) const;
		virtual void SetScale(const Vector3f &scale);
		const Vector3f &GetScale(void) const;
	protected:
		Vector3f m_translate;
		Vector3f m_scale;
		Quaternion m_rotate;
};
#endif
