#ifndef _CAMERA_H
#define _CAMERA_H
#include "math_3d.h"
#include "SceneNode.h"
#include "Export.h"

class ENGINE_EXPORT Camera{
	public:
		Camera(Vector3f pos=Vector3f(0,0,0),Vector3f target=Vector3f(0,0,-1), Vector3f up=Vector3f(0,1,0));
		void Update();		
		/*
		void SetPos(Vector3f pos);
		void SetPos(float x, float y, float z);
		void SetUpVec(Vector3f upVec);
		void SetTarget(Vector3f target);
		*/
		void SetFov(float angle);
		void SetClipDistance(float near, float far);
		void SetAspetcRadio(float aspectRadio);
		void Yaw(float angle);/*ˮƽ������ת*/
		void Pitch(float angle);/*��ֱ������ת*/
		void Translate(Vector3f movement);

		//////////////////////////////////////////////////////
		void Render(SceneNode *rootNode, bool ifUseShader=false);
		void RenderNode(SceneNode *rootNode);
		void RenderNodeUseShader(SceneNode *rootNode);
		
	private:
		//���λ�úͷ�����ر��� gluLookAt�����Ĳ�����������
		Vector3f m_positionVector;
		Vector3f m_targetVector;
		Vector3f m_upVector;
		Matrix4f m_viewMatrix;
		
		//���������������������Ĺ��߱���
		float m_angleHorizontal;/*ˮƽ�����ϵ���ת�Ƕȣ�X���᷽����0�ȣ��Զ�����*/
		float m_angleVertical;/*��ֱ�����ϵ���ת�Ƕȣ�Z���᷽����0�ȣ��Զ�����*/
		
		//ͶӰ�任��ر��� gluPerspective�����Ĳ�������
		float m_FOV;/*�ԽǶȼƵ�Y������ӽǽǶ�*/
		float m_zNear, m_zFar;
		float m_aspectRadio;/*aspect radio = screen_width/screen_height*/
		Matrix4f m_perspectiveProjectionMatrix;
		//��ʶ������ʶ�Ƿ���Ҫ����ͶӰ�任����
		bool m_projectionMatrixChanged;/*ͶӰ�����Ƿ�任���任��ʹ��gluPerspective��������*/
		bool m_viewMatrixChanged;/*��ͼ�����Ƿ�仯���任����ʹ��gluLookAt��������*/
};

#endif
