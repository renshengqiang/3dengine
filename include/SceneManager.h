#ifndef _SCENE_MANAGER_H
#define _SCENE_MANAGER_H
#include <Export.h>
#include <RenderWindow.h>
#include <Camera.h>
#include <SceneNode.h>
#include <FrameListener.h>
#include <EventListener.h>

class ENGINE_EXPORT SceneManager{
	public:
		enum ManagerType{
			MANAGER_GENERAL = 0x1
		};
		SceneManager(enum ManagerType=MANAGER_GENERAL);
		Camera* CreateCamera(Vector3f pos= Vector3f(0,0,0), Vector3f target=Vector3f(0,0,-1), Vector3f up=Vector3f(0,1,0));
		RenderWindow* CreateRenderWindow(int w=800, int h=600);
		void AddFrameListener(FrameListener *frameListener);
		void AddEventListener(EventListener *eventListener);
		bool InitRendering(void);
		void StartRendering(bool use_shader);
		void QuitFromRendering(void);
		bool RenderOneFrame(void);
		SceneNode *GetRootNode(void);
	private:
		enum ManagerType m_managerType;
		Camera *mp_cameraInUse;
		RenderWindow *mp_renderWindow;
		FrameListener *mp_frameListener;
		EventListener *mp_eventListener;
		SceneNode	*mp_rootNode;
		bool m_ifUseShader;
};

#endif
