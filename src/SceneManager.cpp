#include <SceneManager.h>
#include <Render.h>
#include <Shader.h>
#include <Mesh.h>
#include <stdlib.h>

SceneManager::SceneManager(enum ManagerType type):
	m_managerType(type),
	mp_cameraInUse(0),
	mp_renderWindow(0),
	mp_frameListener(0),
	mp_eventListener(0),
	mp_rootNode(0)
{
	//所有node都是在该node下面进行创建
	mp_rootNode = new SceneNode();
	
}
Camera* SceneManager::CreateCamera(Vector3f pos, Vector3f target, Vector3f up)
{
	if(mp_cameraInUse!=NULL)
		delete mp_cameraInUse;
	mp_cameraInUse = new Camera(pos, target, up);
	return mp_cameraInUse;
}
RenderWindow* SceneManager::CreateRenderWindow(int w, int h)
{
	mp_renderWindow = new RenderWindow(w,h);
	if(NULL == mp_renderWindow){
		fprintf(stderr, "SceneManager::CreateWindow : create a render window for OpenGL FAIL!");
		exit(1);
	}
	//必须在
	if(InitGlew()==false)
		mp_renderWindow->Quit(-1);
	return mp_renderWindow;
}
void SceneManager::AddFrameListener(FrameListener * frameListener)
{
	mp_frameListener = frameListener;
	return;
}
void SceneManager::AddEventListener(EventListener * eventListener)
{
	mp_eventListener = eventListener;
	return;
}
SceneNode * SceneManager::GetRootNode(void)
{
	return mp_rootNode;
}
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Rendering Operation //////////////////////////////////////////
void SceneManager::StartRendering(bool m_ifUseShader)
{
	this->m_ifUseShader = m_ifUseShader;
	if(InitRendering()==false && mp_renderWindow)
		mp_renderWindow->Quit(-1);
	// rendering loop
	while( 1 ) {
		if(mp_eventListener) mp_eventListener->ProcessEvents(); 
		if(RenderOneFrame() == false) break;
	}
}
bool SceneManager::InitRendering(void)
{
	//create and use shader
	if(m_ifUseShader){
		if(CreateShaders() == false && mp_renderWindow){
			mp_renderWindow->Quit(-1);
			return false;
		}	
	}
	InitRenderState();
	return true;
}
void SceneManager::QuitFromRendering(void)
{
	if(mp_renderWindow){
		mp_renderWindow->Quit(0);
	}
}
/*
 *1.清空缓冲区；
 *2.绘制所有物体；
 *3.执行FrameQueued函数；
 *4.交换缓冲区
 */
bool SceneManager::RenderOneFrame(void)
{   
	ClearBuffer();
	if(mp_cameraInUse!=NULL) mp_cameraInUse->Render(mp_rootNode, m_ifUseShader);
	if(mp_frameListener) {
		if(mp_frameListener->FrameQueued(0.0f) == false)
			return false;
	}
	if(mp_renderWindow) mp_renderWindow->SwapBuffer();
	return true;
}
