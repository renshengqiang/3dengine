#include "Camera.h"
#include "Shader.h"
#include "Render.h"

//-----------------------------------------------------------------------
Camera::Camera(Vector3f pos,Vector3f target,Vector3f up) : Frustum(pos, target, up)
{
	m_boundingBox.setExtents(Vector3f(0,0,0), Vector3f(0,0,0));
}
//-----------------------------------------------------------------------
Matrix4f Camera::GetProjViewMatrix(void)
{
	if(MoveableObject::IsMovded())
	{
		m_viewChanged = true;
	}
	_UpdateProj();
	_UpdateView();
	
	return m_projMatrix * m_viewMatrix;
}
//-----------------------------------------------------------------------
void Camera::_UpdateViewImpl(void)
{
	const Vector3f Vaxis(0.0f, 1.0f, 0.0f);
	Vector3f View(1.0f, 0.0f, 0.0f);
	View.Rotate(m_angleHorizontal, Vaxis);
	View.Normalize();

	Vector3f Haxis = View.Cross(Vaxis);
	Haxis.Normalize();
	View.Rotate(m_angleVertical, Haxis);

	m_targetVector= View;
	m_targetVector.Normalize();

	m_upVector = Haxis.Cross(m_targetVector);
	m_upVector.Normalize();

	Vector3f pos = m_positionVector;
	if(mp_parentNode)
	{
		pos += mp_parentNode->_GetDerivedPosition();
	}
	
	m_viewMatrix.InitCameraTransform(pos, m_targetVector, m_upVector);
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
