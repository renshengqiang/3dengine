#include "Frustum.h"
#include "AxisAlignedBox.h"

//-----------------------------------------------------------------------
Frustum::Frustum(Vector3f pos,Vector3f target,Vector3f up):
	m_positionVector(pos),
	m_targetVector(target),
	m_upVector(up),
	m_FOVy(45.0f),
	m_nearDist(1),	
	m_farDist(10000),
	m_aspectRadio(1.333333333f),
	m_viewChanged(true),
	m_projChanged(true),
	m_frustumPlanesChanged(true)
{	
	m_targetVector.Normalize();
	m_upVector.Normalize();
	
	//Initialize two angles
	Vector3f targetVector(m_targetVector.x, 0.0f, m_targetVector.z);//horizontal target
	targetVector.Normalize();
	if(targetVector.z >= 0.0f)
	{
		if(targetVector.x >= 0.0f)
		{
			m_angleHorizontal = 360.0f - ToDegree(asin(targetVector.z));
		}
		else
		{
			m_angleHorizontal = 180.0f + ToDegree(asin(targetVector.z));
		}
	}
	else
	{
		if(targetVector.x >= 0.0f)
		{
			m_angleHorizontal = ToDegree(asin(-targetVector.z));
		}else
		{
			m_angleHorizontal = 90.0f + ToDegree(asin(-targetVector.z));
		}
	}
	m_angleVertical = -ToDegree(asin(m_targetVector.y));
}
//-----------------------------------------------------------------------
void Frustum::SetAspetcRadio(float aspectRadio)
{
	m_aspectRadio= aspectRadio;
	m_projChanged = true;
	return;
}
//-----------------------------------------------------------------------
void Frustum::SetFov(float angle)
{
	m_FOVy = angle;
	m_projChanged = true;
	return;
}
//-----------------------------------------------------------------------
void Frustum::SetClipDistance(float near,float far)
{
	m_nearDist = near;
	m_farDist = far;
	m_projChanged = true;
	return;
}
//-----------------------------------------------------------------------
bool Frustum::IsVisible(const AxisAlignedBox& bound, FrustumPlane* culledBy)
{
	// Null boxes always invisible
	if (bound.isNull()) return false;

	// Infinite boxes always visible
	if (bound.isInfinite()) return true;

	// Make any pending updates to the calculated frustum planes
	_UpdateFrustumPlanes();

	// Get centre of the box
	Vector3f centre = bound.getCenter();
	// Get the half-size of the box
	Vector3f halfSize = bound.getHalfSize();

	// For each plane, see if all points are on the negative side
	// If so, object is not visible
	for (int plane = 0; plane < 6; ++plane)
	{
		// Skip far plane if infinite view frustum
		if (plane == FRUSTUM_PLANE_FAR && m_farDist == 0)
			continue;

		Plane::Side side = m_frustumPlanes[plane].getSide(centre, halfSize);
		if (side == Plane::NEGATIVE_SIDE)
		{
			// ALL corners on negative side therefore out of view
			if (culledBy)
				*culledBy = (FrustumPlane)plane;
			return false;
		}
	}
	return true;
}
//-----------------------------------------------------------------------
void Frustum::_UpdateView(void)
{
	if(m_viewChanged)
	{
		_UpdateViewImpl();
		m_viewChanged = false;
		m_frustumPlanesChanged = true;
	}
}
//-----------------------------------------------------------------------
void Frustum::_UpdateProj(void)
{
	if(m_projChanged)
	{
		_UpdateProjImpl();
		m_projChanged = false;
		m_frustumPlanesChanged = true;
	}
}
//-----------------------------------------------------------------------
void Frustum::_UpdateFrustumPlanes(void)
{
	_UpdateView();
	_UpdateProj();
	if(m_frustumPlanesChanged)
	{
		_UpdateFrustumPlanesImpl();
		m_frustumPlanesChanged = false;
	}
}
//-----------------------------------------------------------------------
void Frustum::_UpdateViewImpl(void)
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
	m_viewMatrix.InitCameraTransform(m_positionVector,m_targetVector,m_upVector);
}
//-----------------------------------------------------------------------
void Frustum::_UpdateProjImpl(void)
{
	m_projMatrix.InitPersProjTransform(m_FOVy, m_aspectRadio,m_nearDist, m_farDist);
}
//-----------------------------------------------------------------------
void Frustum::_UpdateFrustumPlanesImpl(void)
{
	// -------------------------
	// Update the frustum planes
	// -------------------------
	Matrix4f combo = m_projMatrix * m_viewMatrix;

	m_frustumPlanes[FRUSTUM_PLANE_LEFT].normal.x = combo[3][0] + combo[0][0];
	m_frustumPlanes[FRUSTUM_PLANE_LEFT].normal.y = combo[3][1] + combo[0][1];
	m_frustumPlanes[FRUSTUM_PLANE_LEFT].normal.z = combo[3][2] + combo[0][2];
	m_frustumPlanes[FRUSTUM_PLANE_LEFT].d = combo[3][3] + combo[0][3];

	m_frustumPlanes[FRUSTUM_PLANE_RIGHT].normal.x = combo[3][0] - combo[0][0];
	m_frustumPlanes[FRUSTUM_PLANE_RIGHT].normal.y = combo[3][1] - combo[0][1];
	m_frustumPlanes[FRUSTUM_PLANE_RIGHT].normal.z = combo[3][2] - combo[0][2];
	m_frustumPlanes[FRUSTUM_PLANE_RIGHT].d = combo[3][3] - combo[0][3];

	m_frustumPlanes[FRUSTUM_PLANE_TOP].normal.x = combo[3][0] - combo[1][0];
	m_frustumPlanes[FRUSTUM_PLANE_TOP].normal.y = combo[3][1] - combo[1][1];
	m_frustumPlanes[FRUSTUM_PLANE_TOP].normal.z = combo[3][2] - combo[1][2];
	m_frustumPlanes[FRUSTUM_PLANE_TOP].d = combo[3][3] - combo[1][3];

	m_frustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.x = combo[3][0] + combo[1][0];
	m_frustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.y = combo[3][1] + combo[1][1];
	m_frustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.z = combo[3][2] + combo[1][2];
	m_frustumPlanes[FRUSTUM_PLANE_BOTTOM].d = combo[3][3] + combo[1][3];

	m_frustumPlanes[FRUSTUM_PLANE_NEAR].normal.x = combo[3][0] + combo[2][0];
	m_frustumPlanes[FRUSTUM_PLANE_NEAR].normal.y = combo[3][1] + combo[2][1];
	m_frustumPlanes[FRUSTUM_PLANE_NEAR].normal.z = combo[3][2] + combo[2][2];
	m_frustumPlanes[FRUSTUM_PLANE_NEAR].d = combo[3][3] + combo[2][3];

	m_frustumPlanes[FRUSTUM_PLANE_FAR].normal.x = combo[3][0] - combo[2][0];
	m_frustumPlanes[FRUSTUM_PLANE_FAR].normal.y = combo[3][1] - combo[2][1];
	m_frustumPlanes[FRUSTUM_PLANE_FAR].normal.z = combo[3][2] - combo[2][2];
	m_frustumPlanes[FRUSTUM_PLANE_FAR].d = combo[3][3] - combo[2][3];

	// Renormalise any normals which were not unit length
	for(int i=0; i<6; i++ )
	{
		float length = m_frustumPlanes[i].normal.Normalize();
		m_frustumPlanes[i].d /= length;
	}
}
