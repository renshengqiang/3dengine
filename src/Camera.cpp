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
//-----------------------------------------------------------------------
Ray Camera::GetCameraToViewportRay(float screenX, float screenY)
{
	Ray ret;
	Matrix4f inverseVP = GetProjViewMatrix().Inverse();

	float nx = (2.0f * screenX) - 1.0f;
	float ny = 1.0f - (2.0f * screenY);
	Vector3f nearPoint(nx, ny, -1.f);
	// Use midPoint rather than far point to avoid issues with infinite projection
	Vector3f midPoint (nx, ny,  0.0f);

	// Get ray origin and ray target on near plane in world space
	Vector3f rayOrigin, rayTarget;
	
	rayOrigin = inverseVP * nearPoint;
	rayTarget = inverseVP * midPoint;

	Vector3f rayDirection = rayTarget - rayOrigin;
	rayDirection.Normalize();

	ret.setOrigin(rayOrigin);
	ret.setDirection(rayDirection);
	return ret;
}
