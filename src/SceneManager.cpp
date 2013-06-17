#include "SceneManager.h"
#include "Render.h"
#include "Shader.h"
#include "Mesh.h"
#include "Timer.h"
#include <stdlib.h>
#include <unistd.h>
#include <queue>

//-----------------------------------------------------------------------
SceneManager::SceneManager(enum ManagerType type):
	m_managerType(type),
	mp_cameraInUse(0),
	mp_renderWindow(0),
	mp_frameListener(0),
	mp_eventListener(0),
	mp_rootNode(0),
	m_windowInit(false),
	m_windowInitSucceed(false),
	m_renderThread(0),
	mp_renderingQueue(NULL)
{
	//所有node都是在该node下面进行创建
	mp_rootNode = new SceneNode("rootNode");
	//create a singleton MeshManager
	mMeshManager = new MeshManager();
	pthread_mutex_init(&m_renderingQueueMutex, NULL);
	pthread_mutex_init(&m_sdlMutex, NULL);
	pthread_cond_init(&m_sdlCond, NULL);
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
	pthread_mutex_destroy(&m_sdlMutex);
	pthread_cond_destroy(&m_sdlCond);
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

	pthread_create(&m_renderThread,  NULL, _RenderThreadFunc, (void *)this);

	//等待窗口初始化完毕
	pthread_mutex_lock(&m_sdlMutex);
	while(m_windowInit == false)
		pthread_cond_wait(&m_sdlCond, &m_sdlMutex);
	pthread_mutex_unlock(&m_sdlMutex);
	
	if(false == m_windowInitSucceed)
	{
		fprintf(stderr, "SceneManager::CreateWindow : create a render window for OpenGL FAIL!\n");
		exit(1);
	}	
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
void SceneManager::StartRendering()
{
	std::queue<SceneNode*> IterQueue;
	RenderQueue *pQueue;
	while( 1 ) 
	{
		pthread_mutex_lock(&m_renderingQueueMutex);
		pQueue = mp_renderingQueue;
		pthread_mutex_unlock(&m_renderingQueueMutex);

		if(pQueue == NULL)
		{
			pQueue= new RenderQueue();
			IterQueue.push(mp_rootNode);
			
			//这个地方是闪烁的原因，前面的一帧在绘制，后面的一帧在更新导致的
			//但是这个地方又不能对每个节点进行加锁控制，代价太大
			//这里动画的原理:先复位，后计算关键帧的位置，然后应用关键帧
			//暂时的策略是让渲染线程等待10毫秒，让主线程有时间更新
			//下一步的改进应该让渲染节点中加入每个节点的变换矩阵，这样设计需要对节点模块进行重新设计
			_ApplySceneAnimations();
			while(!IterQueue.empty())
			{
				SceneNode *PNode = IterQueue.front();
				IterQueue.pop();
				pQueue->push_back(PNode);
				int n  = PNode->NumChildren();
				for(int i=0; i<n; ++i)
					IterQueue.push((SceneNode*)(PNode->GetChild(i)));		
			}
			pthread_mutex_lock(&m_renderingQueueMutex);
			mp_renderingQueue = pQueue;
			pthread_mutex_unlock(&m_renderingQueueMutex);
			if(mp_frameListener)
			{
				if(mp_frameListener->FrameQueued(GetElapsedTime()) == false)
					break;
			}
			if(mp_eventListener) 
			{	
				mp_eventListener->ProcessEvents();
			}
		}
	}
}
//-----------------------------------------------------------------------
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
int sceneFps = 0;
void* SceneManager::_RenderThreadFunc(void *p)
{
	SceneManager *pManager = static_cast<SceneManager*>(p);

	if(pManager->mp_renderWindow->InitSDL() == false)
	{
		pManager->m_windowInitSucceed = false;
	}
	else
		pManager->m_windowInitSucceed = true;
	
	pthread_mutex_lock(&(pManager->m_sdlMutex));
	pManager->m_windowInit = true;
	pthread_cond_signal(&(pManager->m_sdlCond));
	pthread_mutex_unlock(&(pManager->m_sdlMutex));

	CreateShaders();

	for(int i=0; i<6; ++i)
	{
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

		pthread_mutex_lock(&(pManager->m_renderingQueueMutex));
		RenderQueue *pQueue = pManager->mp_renderingQueue;
		pManager->mp_renderingQueue = NULL;
		pthread_mutex_unlock(&(pManager->m_renderingQueueMutex));

		usleep(10);//休眠10毫秒是为了让主线程更新渲染线程用
		if(pQueue != NULL)
		{
			//render scene
			UseShaderToRender();
			for(RenderQueueIterator iter = pQueue->begin(); iter != pQueue->end(); ++iter)
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
			delete pQueue;
		}
		
		//render overlay
		UseFixedPipeline();
		DrawOverlay(sceneFps);

		if(pManager->mp_renderWindow) pManager->mp_renderWindow->SwapBuffer();
	}
	return NULL;
}

