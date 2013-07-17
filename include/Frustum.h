#ifndef _FRUSTUM_H
#define _FRUSTUM_H

#include <math_3d.h>
#include <Plane.h>

/** Worldspace clipping planes.
*/
enum FrustumPlane
{
	FRUSTUM_PLANE_NEAR	 = 0,
	FRUSTUM_PLANE_FAR	 = 1,
	FRUSTUM_PLANE_LEFT	 = 2,
	FRUSTUM_PLANE_RIGHT  = 3,
	FRUSTUM_PLANE_TOP	 = 4,
	FRUSTUM_PLANE_BOTTOM = 5
};

class Frustum
{
public:
	Frustum(Vector3f pos=Vector3f(0,0,0),Vector3f target=Vector3f(0,0,-1), Vector3f up=Vector3f(0,1,0));
	void SetFov(float angle);
	void SetClipDistance(float near, float far);
	void SetAspetcRadio(float aspectRadio);
	/*
	void SetPos(Vector3f pos);
	void SetPos(float x, float y, float z);
	void SetUpVec(Vector3f upVec);
	void SetTarget(Vector3f target);
	*/
	bool IsVisible(const AxisAlignedBox& bound, FrustumPlane* culledBy);
	
protected:
	void _UpdateView(void);
	void _UpdateProj(void);
	void _UpdateFrustumPlanes(void);
	virtual void _UpdateViewImpl(void);
	void _UpdateProjImpl(void);
	void _UpdateFrustumPlanesImpl(void);
	
protected:
	//view matrix ��ر���
	Vector3f m_positionVector;
	Vector3f m_targetVector;
	Vector3f m_upVector;
	mutable Matrix4f m_viewMatrix;

	//projection matrix��ر���
	float m_FOVy;/*�ԽǶȼƵ�Y������ӽǽǶ�*/
	float m_nearDist, m_farDist;
	float m_aspectRadio;/*aspect radio = screen_width/screen_height*/
	mutable Matrix4f m_projMatrix;			

	mutable Plane m_frustumPlanes[6];
	mutable bool m_viewChanged, m_projChanged, m_frustumPlanesChanged;

	mutable float m_angleHorizontal;/*ˮƽ�����ϵ���ת�Ƕȣ�X���᷽����0�ȣ��Զ�����*/
	mutable float m_angleVertical;/*��ֱ�����ϵ���ת�Ƕȣ�Z���᷽����0�ȣ��Զ�����*/
};

#endif
