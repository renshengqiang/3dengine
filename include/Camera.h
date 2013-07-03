#ifndef _CAMERA_H
#define _CAMERA_H
#include <math_3d.h>
#include <SceneNode.h>
#include <Export.h>
#include <Frustum.h>
#include <Ray.h>

class ENGINE_EXPORT Camera : public Frustum
{
	friend class SceneManager;
	public:
		Camera(Vector3f pos=Vector3f(0,0,0),Vector3f target=Vector3f(0,0,-1), Vector3f up=Vector3f(0,1,0));
		Matrix4f GetProjViewMatrix(void);

		void Yaw(float angle);/*水平方向旋转*/
		void Pitch(float angle);/*竖直方向旋转*/
		void Translate(Vector3f movement);
		Ray GetCameraToViewportRay(float screenX, float screenY);
};
#endif
