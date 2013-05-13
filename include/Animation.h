#ifndef _ANIMATION_H
#define _ANIMATION_H
#include "Export.h"
#include "AnimationTrack.h"
#include "Node.h"
#include "struct.h"
#include <string>

/*
	Animation类存在的作用:对多个Animation Track进行统一管理
	这些Track共享的信息有:
	1. 动画的时间长度
	2. 动画的当前位置
	3. 动画的插值方式
*/
class ENGINE_EXPORT Animation{
	public:
		/** The types of animation interpolation available. */
		enum InterpolationMode
		{
			/** Values are interpolated along straight lines. */
			IM_LINEAR,
			/** Values are interpolated along a spline, resulting in smoother changes in direction. */
			IM_SPLINE
		};

		/** The types of rotational interpolation available. */
		enum RotationInterpolationMode
		{
			/** Values are interpolated linearly. This is faster but does not 
				necessarily give a completely accurate result.
			*/
			RIM_LINEAR,
			/** Values are interpolated spherically. This is more accurate but
				has a higher cost.
			*/
			RIM_SPHERICAL
		};
		struct NodeTrackListNode{
			unsigned short handle;
			NodeAnimationTrack *track;
			GD_LIST siblingList;
		};
		Animation(const std::string &name, float length);
		virtual ~Animation();
		const std::string &GetName(void) const;
		float GetLength(void) const;
		void SetLength(float len);
		InterpolationMode GetInterpolationMode(void);
		void SetRotationInterpolationMode(RotationInterpolationMode rim);
		RotationInterpolationMode GetRotationInterpolationMode(void);
		/*
			创建一个可驱动一个SceneNode的NodeAnimationTrack
			@param handle是用来检索该track在Animation中的位置；
			该Animation中的所有NodeAnimationTrack的handle必须是独立的
		*/
		NodeAnimationTrack *CreateNodeTrack(unsigned short handle, const std::string &name="");
		NodeAnimationTrack *CreateNodeTrack(unsigned short handle, Node *node);
		unsigned short GetNumNodeTracks(void) const;
		bool HasNodeTrack(unsigned short handle) const;
		NodeAnimationTrack *GetNodeTrack(unsigned short handle) const;
		void DestoryNodeTrack(unsigned short handle);
		void DestroyAllNodeTracks(void);
		void Apply(float timePos, float weight = 1.0, float scale=1.0);
		void ApplyToNode(Node *node, float timePos, float weight = 1.0, float scale = 1.0);
	private:
		GD_LIST m_nodeTrackListHead;
		std::string m_name;
		float m_length;
		enum InterpolationMode m_interpolationMode;
		enum RotationInterpolationMode m_rotateInterpolationMode;
};

#endif
