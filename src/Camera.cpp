#include <Camera.h>
#include <Shader.h>
#include <Render.h>
#include <stdio.h>
Camera::Camera(Vector3f pos,Vector3f target,Vector3f up)
{
	m_projectionMatrixChanged = true;
	m_viewMatrixChanged = true;
	m_positionVector = pos;
	m_targetVector = target;
	m_upVector= up;
	m_FOV = 45.0f;
	m_aspectRadio= 1.3333f;/*4/3*/
	m_zNear = 1;
	m_zFar = 10000;

	m_targetVector.Normalize();
	m_upVector.Normalize();
	m_viewMatrix.InitCameraTransform(m_positionVector,m_targetVector,m_upVector);
	m_perspectiveProjectionMatrix.InitPersProjTransform(m_FOV,m_aspectRadio,m_zNear,m_zFar);
	//Initialize two angles
	Vector3f targetVector(m_targetVector.x, 0.0f, m_targetVector.z);//horizontal target
	targetVector.Normalize();
	if(targetVector.z >= 0.0f){
		if(targetVector.x >= 0.0f){
			m_angleHorizontal = 360.0f - ToDegree(asin(targetVector.z));
		}else{
			m_angleHorizontal = 180.0f + ToDegree(asin(targetVector.z));
		 }
	}
	else{
		if(targetVector.x >= 0.0f){
			m_angleHorizontal = ToDegree(asin(-targetVector.z));
		}else{
			m_angleHorizontal = 90.0f + ToDegree(asin(-targetVector.z));
		}
	}
	m_angleVertical = -ToDegree(asin(m_targetVector.y));
}
void Camera::Update()
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
	return;
}
void Camera::SetAspetcRadio(float aspectRadio)
{
	m_aspectRadio= aspectRadio;
	m_projectionMatrixChanged = true;
	return;
}
void Camera::SetFov(float angle)
{
	m_FOV = angle;
	m_projectionMatrixChanged = true;
	return;
}
void Camera::SetClipDistance(float near,float far)
{
	m_zNear = near;
	m_zFar = far;
	m_projectionMatrixChanged = true;
	return;
}
void Camera::Translate(Vector3f movement)
{
	m_positionVector+=movement;
	m_viewMatrixChanged = true;
	return;
}
void Camera::Yaw(float angle)
{
	m_angleHorizontal+=angle;
	m_viewMatrixChanged = true;
	return;
}
void Camera::Pitch(float angle)
{
	m_angleVertical+=angle;
	m_viewMatrixChanged = true;
	return;
}
void Camera::Render(SceneNode *rootNode, bool ifUseShader)
{
	if(m_projectionMatrixChanged){
		m_perspectiveProjectionMatrix.InitPersProjTransform(m_FOV,m_aspectRadio,m_zNear,m_zFar);
		m_projectionMatrixChanged = false;
		SetProjectMatrix(&m_perspectiveProjectionMatrix);
	}
	if(m_viewMatrixChanged){
		Update();
		m_viewMatrix.InitCameraTransform(m_positionVector,m_targetVector,m_upVector);
		m_viewMatrixChanged = false;
	}
	if(ifUseShader)
		RenderNodeUseShader(rootNode);
	else
		RenderNode(rootNode);
	return;
}
void Camera::RenderNode(SceneNode *rootNode)
{
	if(rootNode!=NULL){
		int n = rootNode->NumChildren();
		Matrix4f modelViewMatrix;
		for(int i=0;i<n;++i){
			SceneNode *node = rootNode->GetChild(i);
			Mesh *mesh = node->GetMesh();
			node->_Update(false,false);
			if(mesh != NULL){
				const Matrix4f &modelMatrix = node->_GetFullTransform();
				modelViewMatrix = m_viewMatrix * modelMatrix;
				//set matrix
				SetModelViewMatrix(&modelViewMatrix);
				//render mesh
				mesh->Render();
			}
			RenderNode(node);//µÝ¹éäÖÈ¾×Ónode
		}
	}
	return;	
}
void Camera::RenderNodeUseShader(SceneNode *rootNode)
{
	Matrix4f perspectViewMatrix;
	perspectViewMatrix = m_perspectiveProjectionMatrix * m_viewMatrix;
	if(rootNode!=NULL){
		int n = rootNode->NumChildren();
		Matrix4f perspectViewModelMatrix;
		for(int i=0;i<n;++i){
			SceneNode *node = rootNode->GetChild(i);
			Mesh *mesh = node->GetMesh();
			node->_Update(false, false);
			if(mesh!=NULL){
				const Matrix4f &modelMatrix = node->_GetFullTransform();
				perspectViewModelMatrix = perspectViewMatrix * modelMatrix;
				//set matrix
				SetTranslateMatrix(g_PVMMatrixLocation,&perspectViewModelMatrix);
				//render mesh
				mesh->RenderUseShader();
			}
			RenderNodeUseShader(node);
		}
	}
	return ;
}