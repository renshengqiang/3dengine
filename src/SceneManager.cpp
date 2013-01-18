#include <SceneManager.h>
#include <Render.h>
#include <Shader.h>
#include <Mesh.h>
#include <Timer.h>
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
	gd_list_init_head(&m_animationListHead);
	gd_list_init_head(&m_animationStateListHead);
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
	//必须在SDL初始化完成后才可以初始化GLEW
	//GLEW可以使得众多扩展可以正确运行，如glGenBuffers等
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
//////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// animation /////////////////////////////////////////////
bool SceneManager::HasAnimation(const  std::string & name) const
{
	GD_LIST *pos;
	struct AnimationListNode *listNode;

	gd_list_for_each(pos,&m_animationListHead){
		listNode = (struct AnimationListNode *)GD_ENTRY(pos,struct AnimationListNode,siblingList);
		if(name == listNode->animation->GetName())
			return true;
	}
	return false;
}
Animation *SceneManager::GetAnimation(const std::string &name) const
{
	GD_LIST *pos;
	struct AnimationListNode *listNode;

	gd_list_for_each(pos,&m_animationListHead){
		listNode = (struct AnimationListNode *)GD_ENTRY(pos,struct AnimationListNode,siblingList);
		if(name == listNode->animation->GetName()) return listNode->animation;
	}
	return NULL;
}

Animation *SceneManager::CreateAnimation(const  std::string & name,float length)
{
	if(HasAnimation(name)){
		fprintf(stderr, "SceneManager::CreateAnimation : Error Create SceneNode Animation, animation named %s already exists\n", name.c_str());
		return NULL;
	}
	Animation *animation = new Animation(name, length);
	struct AnimationListNode *listNode = (struct AnimationListNode *)malloc(sizeof(struct AnimationListNode));
	listNode->animation=animation;
	gd_list_add(&(listNode->siblingList),&m_animationListHead);
	return animation;
}
void SceneManager::DestroyAnimation(const  std::string & name)
{
	GD_LIST *pos;
	struct AnimationListNode *listNode;

	gd_list_for_each(pos,&m_animationListHead){
		listNode = (struct AnimationListNode *)GD_ENTRY(pos,struct AnimationListNode,siblingList);
		if(name == listNode->animation->GetName()){
			gd_list_del(pos);
			delete listNode->animation;
			free(listNode);
			return;
		}
	}
	fprintf(stderr, "SceneManager::DestroyAnimation : Error Destroy SceneNode Animation, animation named %s already exists\n", name.c_str());
	return;
}
void SceneManager::DestroyAllAnimations(void)
{
	GD_LIST *pos, *temp;
	struct AnimationListNode *listNode;

	gd_list_for_each_safe(pos,temp,&m_animationListHead){
		listNode = (struct AnimationListNode*)GD_ENTRY(pos,struct AnimationListNode,siblingList);
		gd_list_del(pos);
		delete listNode->animation;
		free(listNode);
	}
	return;
}
bool SceneManager::HasAnimationState(const std::string &name) const
{
	GD_LIST *pos;
	struct AnimationStateListNode *listNode;

	gd_list_for_each(pos,&m_animationStateListHead){
		listNode = (struct AnimationStateListNode *)GD_ENTRY(pos,struct AnimationStateListNode,siblingList);
		if(name == listNode->animationState->GetName())
			return true;
	}
	return false;	
}
AnimationState *SceneManager::GetAnimationState(const  std::string name)
{
	GD_LIST *pos;
	struct AnimationStateListNode *listNode;

	gd_list_for_each(pos,&m_animationStateListHead){
		listNode = (struct AnimationStateListNode *)GD_ENTRY(pos,struct AnimationStateListNode,siblingList);
		if(name == listNode->animationState->GetName()) return listNode->animationState;
	}
	return NULL;
}
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
	AnimationState *animationState = new AnimationState(name, animation->GetLength());
	struct AnimationStateListNode *listNode = (struct AnimationStateListNode *)malloc(sizeof(struct AnimationStateListNode));
	listNode->animationState=animationState;
	gd_list_add(&(listNode->siblingList),&m_animationStateListHead);
	return animationState;	
}
void SceneManager::DestroyAnimationState(const  std::string & name)
{
	GD_LIST *pos;
	struct AnimationStateListNode *listNode;

	gd_list_for_each(pos,&m_animationStateListHead){
		listNode = (struct AnimationStateListNode *)GD_ENTRY(pos,struct AnimationStateListNode,siblingList);
		if(name == listNode->animationState->GetName()){
			gd_list_del(pos);
			delete listNode->animationState;
			free(listNode);
			return;
		}
	}
	fprintf(stderr, "SceneManager::DestroyAnimationState : Error Destroy AnimationState, AnimationState named %s already exists\n", name.c_str());
	return;	
}
void SceneManager::DestroyAllAnimationStates(void)
{
	GD_LIST *pos, *temp;
	struct AnimationStateListNode *listNode;

	gd_list_for_each_safe(pos,temp,&m_animationStateListHead){
		listNode = (struct AnimationStateListNode*)GD_ENTRY(pos,struct AnimationStateListNode,siblingList);
		gd_list_del(pos);
		delete listNode->animationState;
		free(listNode);
	}
	return;	
}
void SceneManager::_ApplySceneAnimations(void)
{
	GD_LIST *pos;
	struct AnimationStateListNode *animationStateListNode;

	//reset animated node to initial state
	gd_list_for_each(pos,&m_animationStateListHead){
		animationStateListNode = (struct AnimationStateListNode*)GD_ENTRY(pos, struct AnimationStateListNode, siblingList);
		if(animationStateListNode->animationState->GetEnabled()){
			Animation *animation = GetAnimation(animationStateListNode->animationState->GetName());
			for(int i=0;i<animation->GetNumNodeTracks();++i){
				NodeAnimationTrack *track = animation->GetNodeTrack(i);
				SceneNode *node = track->GetAssociatedNode();
				node->ResetToInitialState();
			}//for
		}//if
	}//gd_list_for_each
	//apply animation to each node
	gd_list_for_each(pos, &m_animationStateListHead){
		animationStateListNode = (struct AnimationStateListNode*)GD_ENTRY(pos, struct AnimationStateListNode, siblingList);
		if(animationStateListNode->animationState->GetEnabled()){
			Animation *animation = GetAnimation(animationStateListNode->animationState->GetName());
			animation->Apply(animationStateListNode->animationState->GetTimePosition());
		}
	}
	return;
}

///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Rendering Operation //////////////////////////////////////////
void SceneManager::StartRendering(bool m_ifUseShader)
{
	this->m_ifUseShader = m_ifUseShader;
	if(InitRendering()==false && mp_renderWindow)
		mp_renderWindow->Quit(-1);
	/* rendering loop
	** 1. 处理用户输入；
	** 2. 渲染一帧
	*/
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
	return;
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
	_ApplySceneAnimations();
	if(mp_cameraInUse!=NULL) mp_cameraInUse->Render(mp_rootNode, m_ifUseShader);
	if(mp_frameListener) {
		if(mp_frameListener->FrameQueued(GetElapsedTime()) == false)
			return false;
	}
	if(mp_renderWindow) mp_renderWindow->SwapBuffer();
	return true;
}
