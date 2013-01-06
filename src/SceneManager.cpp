#include <SceneManager.h>
#include <Render.h>
#include <Shader.h>
#include <Mesh.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// BUFFER OBJECT    ////////////////////////////////////////
VERTEX_OBJ *gp_vertexObj;
INDEX_OBJ *gp_indexObj;
PIXEL_OBJ *gp_pixelObj;
Mesh g_mesh;
void _CreateBufferObject()
{
	Vertex vertices[4] ={
		Vertex(Vector3f(-1.0f,-1.0f, 0.0f), Vector2f(0.0f, 0.0f)),
		Vertex(Vector3f( 0.0f,-1.0f, 1.0f), Vector2f(0.5f, 0.0f)),
		Vertex(Vector3f( 1.0f,-1.0f, 0.0f), Vector2f(1.0f, 0.0f)),
		Vertex(Vector3f( 0.0f, 1.0f, 0.0f), Vector2f(0.5f, 1.0f))
	};
	unsigned int indices[] = {
		0,1,3,
		1,2,3,
		3,2,0,
		2,1,0
	};
	gp_vertexObj = CreateVertexObject(COORD_3|TEXTURE_2,sizeof(vertices)/sizeof(Vertex),sizeof(Vertex),(float *)vertices);
	gp_indexObj = CreateIndexObject(sizeof(indices)/sizeof(unsigned int),indices);
	gp_pixelObj = CreatePixelObject("test.png",GL_TEXTURE0,GL_TEXTURE_2D);

	g_mesh.LoadMesh("phoenix_ugv.md2");
}
bool SceneManager::_DrawObject()
{
	if(m_ifUseShader){
		DrawOjectUseShader(gp_indexObj,gp_vertexObj,gp_pixelObj);
		g_mesh.RenderUseShader();
	}
	else{
		DrawObject(0,gp_indexObj,gp_vertexObj,gp_pixelObj,NULL);
		g_mesh.Render();
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Create something ////////////////////////////////////////
SceneManager::SceneManager(enum ManagerType type)
{
	m_managerType = type;
	mp_cameraInUse = NULL;
	mp_renderWindow = NULL;
	mp_eventListener = NULL;
	mp_frameListener = NULL;
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
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Rendering Operation //////////////////////////////////////////
void SceneManager::StartRendering(bool m_ifUseShader)
{
	this->m_ifUseShader = m_ifUseShader;
	if(InitRendering()==false && mp_renderWindow)
		mp_renderWindow->Quit(-1);
	_CreateBufferObject();

	// rendering loop
	while( 1 ) {
		if(mp_eventListener) mp_eventListener->ProcessEvents(); 
		if(RenderOneFrame() == false) break;
	}
}
bool SceneManager::InitRendering(void)
{
	if(InitRender() == false)
		return false;
	//create and use shader
	if(m_ifUseShader)
		if(CreateShaders() == false && mp_renderWindow)
			mp_renderWindow->Quit(-1);
	return true;
}
void SceneManager::QuitFromRendering(void)
{
	if(mp_renderWindow){
		mp_renderWindow->Quit(0);
	}
}
/*
 *1.设置投影矩阵和视图矩阵；
 *2.清空缓冲区；
 *3.绘制所有物体；
 *4.执行FrameQueued函数；
 *5.交换缓冲区
 */
bool SceneManager::RenderOneFrame(void)
{   
	if(mp_cameraInUse){
		if(m_ifUseShader)
			mp_cameraInUse->SetShaderProjectionViewMatrix();
		else
			mp_cameraInUse->SetProjectionViewMatrix();
	}
	ClearBuffer();
	if(_DrawObject() == false) return false;
	if(mp_frameListener) {
		if(mp_frameListener->FrameQueued(0.0f) == false)
			return false;
	}
	if(mp_renderWindow) mp_renderWindow->SwapBuffer();
	return true;
}
