#ifndef _CAMERA_H
#define _CAMERA_H
#include <SceneNode.h>
#include <Export.h>
#include <Frustum.h>
#include <MoveableObject.h>
#include <Ray.h>

class ENGINE_EXPORT Camera : public Frustum, public MoveableObject
{
friend class SceneManager;
public:
	Camera(Vector3f pos=Vector3f(0,0,0),Vector3f target=Vector3f(0,0,-1), Vector3f up=Vector3f(0,1,0));
	
	Matrix4f GetProjViewMatrix(void);
	virtual void _UpdateViewImpl(void);
	
	const AxisAlignedBox& GetBoundingBox(void) const { return m_boundingBox; }
	Ray GetCameraToViewportRay(float screenX, float screenY);

	void Yaw(float angle);/*水平方向旋转*/
	void Pitch(float angle);/*竖直方向旋转*/
	void Translate(Vector3f movement);
	
private:
	AxisAlignedBox m_boundingBox;
};
#endif
