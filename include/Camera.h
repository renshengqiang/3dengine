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
		void Yaw(float angle);/*水平方向旋转*/
		void Pitch(float angle);/*竖直方向旋转*/
		void Translate(Vector3f movement);

		//////////////////////////////////////////////////////
		void Render(SceneNode *rootNode, bool ifUseShader=false);
		void RenderNode(SceneNode *rootNode);
		void RenderNodeUseShader(SceneNode *rootNode);
		
	private:
		//相机位置和方向相关变量 gluLookAt函数的参数都在下面
		Vector3f m_positionVector;
		Vector3f m_targetVector;
		Vector3f m_upVector;
		Matrix4f m_viewMatrix;
		
		//辅助计算上面三个变量的工具变量
		float m_angleHorizontal;/*水平方向上的旋转角度，X正轴方向是0度，以度数计*/
		float m_angleVertical;/*竖直方向上的旋转角度，Z负轴方向是0度，以度数计*/
		
		//投影变换相关变量 gluPerspective函数的参数如下
		float m_FOV;/*以角度计的Y方向的视角角度*/
		float m_zNear, m_zFar;
		float m_aspectRadio;/*aspect radio = screen_width/screen_height*/
		Matrix4f m_perspectiveProjectionMatrix;
		//标识量，标识是否需要更新投影变换矩阵
		bool m_projectionMatrixChanged;/*投影矩阵是否变换，变换则使用gluPerspective重新设置*/
		bool m_viewMatrixChanged;/*视图矩阵是否变化，变换了则使用gluLookAt重新设置*/
};

#endif
