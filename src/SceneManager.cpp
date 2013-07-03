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
	pthread_cond_init(&m_renderingQueueFullCond, NULL);
	pthread_cond_init(&m_renderingQueueEmptyCond, NULL);
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
	pthread_cond_destroy(&m_renderingQueueFullCond);
	pthread_cond_destroy(&m_renderingQueueEmptyCond);
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
//-----------------------------------------------------------------------
RaySceneQuery* 
SceneManager::CreateRayQuery(const Ray& ray)
{
	RaySceneQuery* q = new RaySceneQuery(this);
	q->setRay(ray);
	//q->setQueryMask(mask);
	return q;
}
//-----------------------------------------------------------------------
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
void SceneManager::_ApplySceneAnimations()
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
//-----------------------------------------------------------------------
void SceneManager::_UpdateSceneGraph( Camera * cam )
{
	mp_rootNode->_Update(true, false);
}
//-----------------------------------------------------------------------
void SceneManager::_FindVisibleObjects(Camera * cam, SceneManager::RenderQueue& renderQueue)
{
	std::queue<SceneNode*> IterQueue;
	struct RenderItem renderItem;
	
    	renderQueue.clear();
	IterQueue.push(mp_rootNode);
	while(!IterQueue.empty())
	{
		SceneNode *pNode = IterQueue.front();
		IterQueue.pop();

		if(cam->IsVisible(pNode->GetWorldBoundingBox(), NULL))
		{
			renderItem.pNode = pNode;
			renderItem.transMatrix = pNode->_GetFullTransform();
			renderQueue.push_back(renderItem);			
		}
		
		int n  = pNode->NumChildren();
		for(int i=0; i<n; ++i)
			IterQueue.push((SceneNode*)(pNode->GetChild(i)));		
	}	
}

//------------------------------Rendering Operation----------------------------
void SceneManager::StartRendering()
{
	RenderQueue *pQueue;
	
	while( 1 ) 
	{
		pthread_mutex_lock(&m_renderingQueueMutex);
		while(mp_renderingQueue != NULL)
			pthread_cond_wait(&m_renderingQueueEmptyCond, &m_renderingQueueMutex);
		pthread_mutex_unlock(&m_renderingQueueMutex);

		pQueue= new RenderQueue();

		//apply animation
		_ApplySceneAnimations();
		//update sceneGraph
		_UpdateSceneGraph(mp_cameraInUse);
		//find visible scenenodes
		_FindVisibleObjects(mp_cameraInUse, *pQueue);

		//printf("queue num: %d\n", pQueue->size());
		pthread_mutex_lock(&m_renderingQueueMutex);
		mp_renderingQueue = pQueue;
		pthread_mutex_unlock(&m_renderingQueueMutex);
		pthread_cond_signal(&m_renderingQueueFullCond);
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
//-----------------------------------------------------------------------
//-----------------------  Rendering Thread Func ----------------------------
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
	RenderQueue *pQueue;

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
		while(pManager->mp_renderingQueue == NULL)
			pthread_cond_wait(&(pManager->m_renderingQueueFullCond), &(pManager->m_renderingQueueMutex));
		pQueue = pManager->mp_renderingQueue;
		pManager->mp_renderingQueue = NULL;
		pthread_cond_signal(&(pManager->m_renderingQueueEmptyCond));
		pthread_mutex_unlock(&(pManager->m_renderingQueueMutex));

		//render scene
		UseShaderToRender();
		for(RenderQueueIterator iter = pQueue->begin(); iter != pQueue->end(); ++iter)
		{
			Entity *pEntity = (iter->pNode)->GetAttachedEntity();
			if(pEntity!= NULL)
			{
				perspectViewModelMatrix = projViewMatrix * iter->transMatrix;
				//set matrix
				SetTranslateMatrix(g_PVMMatrixLocation,&perspectViewModelMatrix);
				//render entity
				pEntity->Render();
			}
			//DrawAABB(iter->pNode->GetWorldBoundingBox());
		}
		delete pQueue;
		
		//render overlay
		UseFixedPipeline();
		DrawOverlay(sceneFps);

		//swap buffer
		pManager->mp_renderWindow->SwapBuffer();
	}
	return NULL;
}
