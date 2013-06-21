#include "Camera.h"
#include "Shader.h"
#include "Render.h"
#include <stdio.h>


//-----------------------------------------------------------------------
Camera::Camera(Vector3f pos,Vector3f target,Vector3f up) : Frustum(pos, target, up)
{
}
//-----------------------------------------------------------------------
Matrix4f Camera::GetProjViewMatrix(void)
{
	_UpdateProj();
	_UpdateView();
	return m_projMatrix * m_viewMatrix;
}
//-----------------------------------------------------------------------
void Camera::Translate(Vector3f movement)
{
	m_positionVector+=movement;
	m_viewChanged = true;
	return;
}
//-----------------------------------------------------------------------
void Camera::Yaw(float angle)
{
	m_angleHorizontal+=angle;
	m_viewChanged = true;
	return;
}
//-----------------------------------------------------------------------
void Camera::Pitch(float angle)
{
	m_angleVertical+=angle;
	m_viewChanged = true;
	return;
}
