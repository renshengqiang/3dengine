#include "SceneManager.h"
#include "Render.h"
#include "Shader.h"
#include "Mesh.h"
#include "Timer.h"
#include <stdlib.h>
#include <unistd.h>

//-----------------------------------------------------------------------
SceneManager::SceneManager(enum ManagerType type):
	m_managerType(type),
	mp_cameraInUse(0),
	mp_renderWindow(0),
	mp_frameListener(0),
	mp_eventListener(0),
	mp_rootNode(0)
{
	//所有node都是在该node下面进行创建
	mp_rootNode = new SceneNode("rootNode");
	//create a singleton MeshManager
	mMeshManager = new MeshManager();
	pthread_mutex_init(&m_renderingQueueMutex, NULL);
	pthread_mutex_init(&m_contextMutex, NULL);
	pthread_mutex_init(&m_sdlMutex, NULL);
	m_renderThread = 0;
}
//-----------------------------------------------------------------------
SceneManager::~SceneManager()
{
	if(m_renderThread > 0)
	{
		pthread_cancel(m_renderThread);
	}
	DestroyAllAnimationStates();
	DestroyAllAnimations();
	delete mMeshManager;
	pthread_mutex_destroy(&m_renderingQueueMutex);
	pthread_mutex_destroy(&m_contextMutex);
	pthread_mutex_destroy(&m_sdlMutex);
}
//-----------------------------------------------------------------------
#include "Texture.h"
GLuint texture_obj[6];

char texture_name[][100] = {
	"./skybox/sp3front.jpg",
	"./skybox/sp3back.jpg",
	"./skybox/sp3top.jpg",
	"./skybox/sp3bot.jpg",
	"./skybox/sp3left.jpg",
	"./skybox/sp3right.jpg"	
};
int sceceFps = 0;
void* SceneManager::_RenderThreadFunc(void *p)
{
	SceneManager *pManager = static_cast<SceneManager*>(p);

	pManager->mp_renderWindow->InitSDL();
	pthread_mutex_lock(&(pManager->m_contextMutex));
	pthread_mutex_unlock(&(pManager->m_contextMutex));
	pthread_mutex_unlock(&(pManager->m_sdlMutex));
	CreateShaders();
	InitRenderState();

	for(int i=0; i<6; ++i){
		Texture *pTexture = new Texture(GL_TEXTURE_2D, texture_name[i]);
		pTexture->Load();
		texture_obj[i] = pTexture->GetTextureObj();
		delete pTexture;
	}
	while(1)
	{
		Matrix4f projViewMatrix = pManager->mp_cameraInUse->GetProjViewMatrix();
		Matrix4f perspectViewModelMatrix;
		
		ClearBuffer();
		//render skybox
		UseFixedPipeline();
		DrawSkyBox(texture_obj, pManager->mp_cameraInUse->m_angleHorizontal, pManager->mp_cameraInUse->m_angleVertical);

		UseShaderToRender();
		
		pthread_mutex_lock(&(pManager->m_renderingQueueMutex));
		for(RenderQueueIterator iter = pManager->m_renderingQueue.begin(); iter != pManager->m_renderingQueue.end(); ++iter)
		{
			Entity *pEntity = (*iter)->GetAttachedEntity();
			if(pEntity!= NULL){
				const Matrix4f &modelMatrix = (*iter)->_GetFullTransform();
				perspectViewModelMatrix = projViewMatrix * modelMatrix;
				//set matrix
				SetTranslateMatrix(g_PVMMatrixLocation,&perspectViewModelMatrix);
				//render entity
				pEntity->Render();
			}
		}
		pManager->m_renderingQueue.clear();
		pthread_mutex_unlock(&(pManager->m_renderingQueueMutex));
		//render overlay
		UseFixedPipeline();
		DrawOverlay(sceceFps);

		if(pManager->mp_renderWindow) pManager->mp_renderWindow->SwapBuffer();
	}
	return NULL;
}
//-----------------------------------------------------------------------
Camera* SceneManager::CreateCamera(Vector3f pos, Vector3f target, Vector3f up)
{
	if(mp_cameraInUse!=NULL)
		delete mp_cameraInUse;
	mp_cameraInUse = new Camera(pos, target, up);
	return mp_cameraInUse;
}
//-----------------------------------------------------------------------
RenderWindow* SceneManager::CreateRenderWindow(int w, int h)
{
	mp_renderWindow = new RenderWindow(w,h);

	//XInitThreads();
	pthread_mutex_lock(&m_contextMutex);
	pthread_mutex_lock(&m_sdlMutex);
	pthread_create(&m_renderThread,  NULL, _RenderThreadFunc, (void *)this);
	/*
	if(NULL == mp_renderWindow){
		fprintf(stderr, "SceneManager::CreateWindow : create a render window for OpenGL FAIL!");
		exit(1);
	}
	//必须在SDL初始化完成后才可以初始化GLEW
	//GLEW可以使得众多扩展可以正确运行，如glGenBuffers等
	if(InitGlew()==false)
		mp_renderWindow->Quit(-1);
	//show the OpenGL library version info , this is window-system specified
	GetGLInfo();
	*/
	return mp_renderWindow;
}
//-----------------------------------------------------------------------
void SceneManager::AddFrameListener(FrameListener * frameListener)
{
	mp_frameListener = frameListener;
	return;
}
//-----------------------------------------------------------------------
void SceneManager::AddEventListener(EventListener * eventListener)
{
	mp_eventListener = eventListener;
	return;
}
//-----------------------------------------------------------------------
SceneNode * SceneManager::GetRootNode(void)
{
	return mp_rootNode;
}
//-----------------------------------animation------------------------------------
bool SceneManager::HasAnimation(const  std::string & name) const
{
	return m_animationList.find(name) != m_animationList.end();
}
//-----------------------------------------------------------------------
Animation *SceneManager::GetAnimation(const std::string &name) const
{
	AnimationConstIterator iter = m_animationList.find(name);
	if(iter != m_animationList.end())
	{
		return iter->second;
	}
	return NULL;
}
//-----------------------------------------------------------------------
Animation *SceneManager::CreateAnimation(const  std::string & name,float length)
{
	if(HasAnimation(name)){
		fprintf(stderr, "SceneManager::CreateAnimation : Error Create Node Animation, animation named %s already exists\n", name.c_str());
		return NULL;
	}
	Animation *animation = new Animation(name, length);
	m_animationList.insert(std::make_pair(name, animation));
	return animation;
}
//-----------------------------------------------------------------------
void SceneManager::DestroyAnimation(const  std::string & name)
{
	AnimationIterator iter = m_animationList.find(name);

	if(iter != m_animationList.end())
	{
		delete iter->second;
		m_animationList.erase(iter);
	}
	else
	{
		fprintf(stderr, "SceneManager::DestroyAnimation : Error Destroy Node Animation, animation named %s already exists\n", name.c_str());
	}
	return ;
}
//-----------------------------------------------------------------------
void SceneManager::DestroyAllAnimations(void)
{
	for(AnimationIterator iter = m_animationList.begin(); iter != m_animationList.end(); ++iter)
	{
		delete iter->second;
	}
	m_animationList.clear();
	return;
}
//-----------------------------------------------------------------------
bool SceneManager::HasAnimationState(const std::string &name) const
{
	return m_animationStateSet.HasAnimationState(name);
}
//-----------------------------------------------------------------------
AnimationState *SceneManager::GetAnimationState(const  std::string name)
{
	return m_animationStateSet.GetAnimationState(name);
}
//-----------------------------------------------------------------------
AnimationState *SceneManager::CreateAnimationState(const  std::string & name)
{
	if(HasAnimationState(name)){
		fprintf(stderr, "SceneManager::HasAnimationState : Error Create Animation State, AnimationState named %s already exists\n", name.c_str());
		return NULL;
	}
	Animation *animation = GetAnimation(name);
	if(animation == NULL){
		fprintf(stderr, "SceneManager::CreateAnimationState : Error Create Animaion State, Animation named %s does not exist\n", name.c_str());
		return NULL;
	}

	return m_animationStateSet.CreateAnimationState(name, animation->GetLength());
}
//-----------------------------------------------------------------------
void SceneManager::DestroyAnimationState(const  std::string & name)
{
	m_animationStateSet.RemoveAnimationState(name);
	return;
}
//-----------------------------------------------------------------------
void SceneManager::DestroyAllAnimationStates(void)
{
	m_animationStateSet.RemoveAllAnimationStates();
	return;
}
//-----------------------------------------------------------------------
void SceneManager::_ApplySceneAnimations(void)
{
	AnimationStateSet::EnabledAnimationStateIterator iter = m_animationStateSet._GetEnabledAnimationIteratorBegin();
	
	for(; iter!=m_animationStateSet._GetEnabledAnimationIeratorEnd(); ++iter){
		AnimationState * animationState = *iter;
		Animation *animation = GetAnimation(animationState->GetName());
		//reset
		for(Animation::NodeTrackIterator iter1 = animation->GetNodeTrackBegin(); 
			iter1 != animation->GetNodeTrackEnd(); ++iter1)
		{
			NodeAnimationTrack *track = iter1->second;
			Node *node = track->GetAssociatedNode();
			node->ResetToInitialState();
		}
		//use animation
		animation->Apply(animationState->GetTimePosition());
	}
	return;
}
//------------------------------Rendering Operation----------------------------
#include <queue>
void SceneManager::StartRendering(bool m_ifUseShader)
{
	this->m_ifUseShader = m_ifUseShader;
	//if(InitRendering()==false && mp_renderWindow)
		//mp_renderWindow->Quit(-1);

	pthread_mutex_unlock(&m_contextMutex);
	pthread_mutex_lock(&m_sdlMutex);
	/* rendering loop
	** 1. 处理用户输入；
	** 2. 渲染一帧
	*/
	std::queue<SceneNode*> IterQueue;
	while( 1 ) 
	{
		//if(RenderOneFrame() == false) break;
		pthread_mutex_lock(&m_renderingQueueMutex);
		_ApplySceneAnimations();
		if(m_renderingQueue.empty())
		{			
			IterQueue.push(mp_rootNode);
			while(!IterQueue.empty())
			{
				SceneNode *PNode = IterQueue.front();
				IterQueue.pop();
				m_renderingQueue.push_back(PNode);
				int n  = PNode->NumChildren();
				for(int i=0; i<n; ++i)
					IterQueue.push((SceneNode*)(PNode->GetChild(i)));		
			}
			if(mp_frameListener) {
				if(mp_frameListener->FrameQueued(GetElapsedTime()) == false)
					break;
			}
			if(mp_eventListener) mp_eventListener->ProcessEvents();
		}		
		pthread_mutex_unlock(&m_renderingQueueMutex);
	}
}
//-----------------------------------------------------------------------
bool SceneManager::InitRendering(void)
{
	//create and use shader
	if(m_ifUseShader){
		if(CreateShaders() == false && mp_renderWindow){
			fprintf(stderr, "SceneManager::InitRendering : create shader error\n");
			mp_renderWindow->Quit(-1);
			return false;
		}	
	}
	InitRenderState();
	return true;
}
//-----------------------------------------------------------------------
void SceneManager::QuitFromRendering(void)
{
	if(mp_renderWindow){
		mp_renderWindow->Quit(0);
	}
	return;
}
//-----------------------------------------------------------------------
/*
 *1.清空缓冲区；
 *2.绘制所有物体；
 *3.执行FrameQueued函数；
 *4.交换缓冲区
 */
bool SceneManager::RenderOneFrame(void)
{   
	ClearBuffer();
	//render skybox
	UseFixedPipeline();
	DrawSkyBox(texture_obj, mp_cameraInUse->m_angleHorizontal, mp_cameraInUse->m_angleVertical);
	//render scene
	_ApplySceneAnimations();
	UseShaderToRender();
	if(mp_cameraInUse!=NULL) mp_cameraInUse->Render(mp_rootNode, m_ifUseShader);
	//render overlay
	UseFixedPipeline();
	DrawOverlay(sceceFps);
	if(mp_frameListener) {
		if(mp_frameListener->FrameQueued(GetElapsedTime()) == false)
			return false;
	}
	if(mp_renderWindow) mp_renderWindow->SwapBuffer();
	return true;
}
