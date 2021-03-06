#include "Engine.h"
#include <SDL/SDL.h>
#include <pthread.h>
#include <map>
using namespace std;

#define STEPSIZE 10
#define HANGLESTEP (180/(float)800)
#define VANGLESTEP (180/(float)600)
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

/**************** fps info ******************/
pthread_t fpsThreadId = 0;
static int fps = 0;
void* FpsThread(void*)
{
    struct timeval tv ={1, 0};

    while(1)
    {
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        //sleep
        select(1, NULL, NULL, NULL, &tv);
        printf("fps: %d\n", fps);
		fflush(stdout);
	sceneFps = fps;
	fps = 0;
    }
	return NULL;
}
/****************** class definition ************/
class DemoApp:public FrameListener, public EventListener
{
	public:
		DemoApp();
		~DemoApp();

		void ClearResource(void);
		bool FrameQueued(long timeSinceLastFrame);
		void ProcessEvents();
		void CreateScene(void);
		void Run();
		void HandleKeyDown( SDL_keysym* keysym );
		void HandleMouseMotion(SDL_MouseMotionEvent event);
	private:
		bool m_ifUseShader;
		SceneManager *mp_sceneManager;
		Camera	*mp_camera;
		RenderWindow *mp_renderWindow;
		AnimationState *mp_cameraNodeAnimState, *mp_animationState1,*mp_animationState2, *mp_animationState3;
		AnimationState *mp_skeletonAnimationState;
		Entity *mp_entity1, *mp_entity2, *mp_entity3;
		SceneNode *mp_cameraNode;
		RaySceneQuery *mp_raySceneQuery;
		std::map<SceneNode*, AnimationState*> m_nodeAnimStateMap;
		ParticleSystem *mp_particleSystem;
};
DemoApp::DemoApp()
{
	//mp_sceneManager = new SceneManager();
	mp_sceneManager = new OctreeSceneManager();
	mp_renderWindow = mp_sceneManager->CreateRenderWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
	mp_camera = mp_sceneManager->CreateCamera(Vector3f(0.0f,0.0f,0.0f),Vector3f(0, 0, -1.0f));
	mp_sceneManager->AddFrameListener(this);
	mp_sceneManager->AddEventListener(this);
	mp_entity1 = mp_entity2 = mp_entity3 = NULL;
	mp_cameraNode = NULL;
	mp_cameraNodeAnimState = NULL;
	mp_animationState1 = NULL;
	mp_animationState2 = NULL;
	mp_animationState3 = NULL;
	mp_raySceneQuery = mp_sceneManager->CreateRayQuery(Ray());
	mp_raySceneQuery->setSortByDistance(true);
	mp_particleSystem = NULL;
}
DemoApp::~DemoApp()
{
	ClearResource();
}
void DemoApp::ClearResource(void)
{
	if(mp_entity1) delete mp_entity1;
	if(mp_entity2) delete mp_entity2;
	if(mp_entity3) delete mp_entity3;
	//....
	if(mp_sceneManager) delete mp_sceneManager;
}
bool DemoApp::FrameQueued(long timeSinceLastFrame)
{	
	static float time = 0;
	time+=timeSinceLastFrame/(float)1000;
	fps++;
	if(mp_cameraNodeAnimState)
	{
		mp_cameraNodeAnimState->AddTime(timeSinceLastFrame/(float)1000);
	}
	if(mp_animationState1){
		mp_animationState1->AddTime(timeSinceLastFrame/(float)1000);
	}
	if(mp_animationState2){
		mp_animationState2->AddTime(timeSinceLastFrame/(float)1000);
	}
	if(mp_animationState3){
		mp_animationState3->AddTime(timeSinceLastFrame/(float)1000);
	}
	if(mp_skeletonAnimationState){
		mp_skeletonAnimationState->AddTime(timeSinceLastFrame/(float)1000);
	}
	if(mp_particleSystem)
	{
		mp_particleSystem->AddTime(timeSinceLastFrame/(float)1000);
	}
		
	return FrameListener::FrameQueued(timeSinceLastFrame);
}
void DemoApp::CreateScene(void)
{
	////Get root scenenode
	SceneNode *rootNode = mp_sceneManager->GetRootNode();

	////create camera node and attach camera on it
	mp_cameraNode = rootNode->CreateChildSceneNode("CameraNode");
	mp_cameraNode->AttachObject(mp_camera);
	
	////create a camera animation
	mp_cameraNode->Translate(Vector3f(0, 0, 200));
	Animation * animation = mp_sceneManager->CreateAnimation("CameraAnim", 2);
	NodeAnimationTrack * track  = animation->CreateNodeTrack(0, mp_cameraNode);
	TransformKeyFrame *keyFrame = track->CreateNodeKeyFrame(2);
	keyFrame->SetTranslate(Vector3f(0,0,-200));
	mp_cameraNode->SetInitialState();
	mp_cameraNodeAnimState = mp_sceneManager->CreateAnimationState("CameraAnim");
	mp_cameraNodeAnimState->SetEnabled(false);
	mp_cameraNodeAnimState->SetLoop(false);

	////create a particle system
	mp_particleSystem = mp_sceneManager->CreateParticleSystem();
	mp_particleSystem->SetTexture("./textures/particle.bmp");
	SceneNode *particleNode = rootNode->CreateChildSceneNode("particleNode");
	particleNode->AttachObject(mp_particleSystem);
	
	////create three entities
	mp_entity1 = new Entity("./models/phoenix_ugv.md2");
	mp_entity2 = new Entity("./models/boblampclean.md5mesh");
	mp_entity3 = new Entity("./models/boblampclean.md5mesh");

	////create three scenenode and attach entity on it
	SceneNode *node1 = rootNode->CreateChildSceneNode("childnode1");
	node1->Translate(0,0,-500, SceneNode::TS_LOCAL);
	SceneNode *node2 = rootNode->CreateChildSceneNode("childnode2");
	node2->Translate(Vector3f(100, 0, -500), SceneNode::TS_LOCAL);
	node2->Rotate(Vector3f(1,0,0), -90, SceneNode::TS_LOCAL);
	SceneNode *node3 = rootNode->CreateChildSceneNode("childnode3");
	node3->Translate(Vector3f(100, -100, -250), SceneNode::TS_LOCAL);
	node3->Rotate(Vector3f(1,0,0), -90, SceneNode::TS_LOCAL);
		
	node1->AttachObject(mp_entity1);
	node2->AttachObject(mp_entity2);
	node3->AttachObject(mp_entity3);
	
	////create one scenenode animation(左后回)
	animation = mp_sceneManager->CreateAnimation("transAnim1-1",5.792);
	//animation->SetInterpolationMode(Animation::IM_SPLINE);
	animation->SetRotationInterpolationMode(Animation::RIM_SPHERICAL);
	track = animation->CreateNodeTrack(0, node1);
	keyFrame = track->CreateNodeKeyFrame(1.448);
	keyFrame->SetTranslate(Vector3f(-100, 0,0));
	keyFrame = track->CreateNodeKeyFrame(5.792);
	keyFrame->SetTranslate(Vector3f(0,0,0));
	keyFrame = track->CreateNodeKeyFrame(2.896);
	keyFrame->SetTranslate(Vector3f(-100, 0, -100));
	keyFrame->SetRotation(Quaternion(Vector3f(0,1,0),-90));
	keyFrame->SetScale(Vector3f(2,2,2));

	////根据节点当前位置做节点动画，否则是根据节点创建时的位置进行动画
	node1->SetInitialState();
	mp_animationState1= mp_sceneManager->CreateAnimationState("transAnim1-1");
	mp_animationState1->SetEnabled(true);
	mp_animationState1->SetLoop(true);

	m_nodeAnimStateMap.insert(make_pair(node1, mp_animationState1));
	
	////another scenenode animation
	animation = mp_sceneManager->CreateAnimation("transAnim2",5.792);
	track = animation->CreateNodeTrack(0, node2);
	keyFrame = track->CreateNodeKeyFrame(1.448);
	keyFrame->SetTranslate(Vector3f(0, 0, 250));
	keyFrame = track->CreateNodeKeyFrame(2.896);
	keyFrame->SetTranslate(Vector3f(0, 0, 250));
	keyFrame->SetRotation(Quaternion(Vector3f(0,1,0),45));
	keyFrame = track->CreateNodeKeyFrame(4.344);
	keyFrame->SetTranslate(Vector3f(0, 0, 250));
	keyFrame->SetRotation(Quaternion(Vector3f(0,1,0),-45));
	keyFrame = track->CreateNodeKeyFrame(5.792);

	node2->SetInitialState();
	mp_animationState3= mp_sceneManager->CreateAnimationState("transAnim2");
	mp_animationState3->SetEnabled(true);
	mp_animationState3->SetLoop(true);
	
	m_nodeAnimStateMap.insert(make_pair(node2, mp_animationState3));

	////skeleton animation
	mp_skeletonAnimationState = mp_entity2->GetAnimationState("");
	mp_skeletonAnimationState->SetEnabled(true);
	mp_skeletonAnimationState->SetLoop(true);

	////create Plane
	Plane plane(Vector3f(0,-1,0), 10);
	MeshManager& meshManager = MeshManager::GetSingleton();
	MeshPtr meshPtr = meshManager.CreatePlane("plane",plane,200,200,1,1,false,1,1,Vector3f(0,0,-1));
	Texture *pTexture = new Texture(GL_TEXTURE_2D, "./textures/BeachStones.jpg");
	meshPtr->AddTexture(pTexture);
	SubMesh *pSubMesh = meshPtr->GetSubMesh(0);
	pSubMesh->SetTexture(pTexture);
	Entity *planeEntity = new Entity();
	planeEntity->SetMeshPtr(meshPtr);

	SceneNode *node4 = rootNode->CreateChildSceneNode("planeNode");
	node4->AttachObject(planeEntity);
}
void DemoApp::Run()
{
	CreateScene();
	mp_sceneManager->StartRendering();
}
/************************ handle sdl events **********************************/
void DemoApp::ProcessEvents()
{
	SDL_Event event;    /* Grab all the events off the queue. */ 
	static bool mousedown = false;
	while( SDL_PollEvent( &event ) ) {        
		switch( event.type ) {        
			case SDL_KEYDOWN:            /* Handle key presses. */            
				HandleKeyDown( &event.key.keysym );            
				break;
			case SDL_MOUSEBUTTONDOWN:
				{
					Ray mouseRay = mp_camera->GetCameraToViewportRay((float)event.button.x/WINDOW_WIDTH, (float)event.button.y/WINDOW_HEIGHT);
					mp_raySceneQuery->setRay(mouseRay);
					RaySceneQueryResult &result = mp_raySceneQuery->execute();
					RaySceneQueryResult::iterator iter;
					for(iter=result.begin(); iter!=result.end(); iter++)
					{
						if(m_nodeAnimStateMap.find(iter->movable) !=m_nodeAnimStateMap.end() )
						{
							AnimationState *pState  = m_nodeAnimStateMap.find(iter->movable)->second;
							pState->SetEnabled(false);
						}
						else
						{
							iter->movable->Translate(Vector3f(0,0,-100));
						}
					}
					mousedown = true;
					break;
				}
			case SDL_MOUSEBUTTONUP:
				{
					Ray mouseRay = mp_camera->GetCameraToViewportRay((float)event.button.x/WINDOW_WIDTH, (float)event.button.y/WINDOW_HEIGHT);
					mp_raySceneQuery->setRay(mouseRay);
					RaySceneQueryResult &result = mp_raySceneQuery->execute();
					RaySceneQueryResult::iterator iter;
					for(iter=result.begin(); iter!=result.end(); iter++)
					{

						if(m_nodeAnimStateMap.find(iter->movable) != m_nodeAnimStateMap.end())
						{
							AnimationState *pState  = m_nodeAnimStateMap.find(iter->movable)->second;
							pState->SetEnabled(true);
						}
						else
						{
							iter->movable->Translate(Vector3f(0,0,100));
						}
					}
					mousedown = false;
					break;
				}
			case SDL_MOUSEMOTION:
				if(mousedown)
					HandleMouseMotion(event.motion);
				break;
			case SDL_QUIT:            /* Handle quit requests (like Ctrl-c). */        
				//mp_sceneManager->QuitFromRendering();
				exit(0);
				break;        
			}
	}
}
void DemoApp::HandleKeyDown( SDL_keysym* keysym )
{    
	if(mp_camera){
		switch( keysym->sym ) {    
			case SDLK_ESCAPE:
				//mp_sceneManager->QuitFromRendering();
				pthread_cancel(fpsThreadId);
				ClearResource();
				exit(0);
				break; 
			case SDLK_DOWN:
			case SDLK_s:
				//mp_camera->Translate(Vector3f(0,0,STEPSIZE));
				mp_cameraNode->Translate(Vector3f(0,0,STEPSIZE));
				break;
			case SDLK_UP:
			case SDLK_w:
				//mp_camera->Translate(Vector3f(0,0,-STEPSIZE));
				mp_cameraNode->Translate(Vector3f(0,0,-STEPSIZE));
				break;
			case SDLK_LEFT:
				//mp_camera->Translate(Vector3f(-STEPSIZE, 0,0));
				mp_cameraNode->Translate(Vector3f(-STEPSIZE,0,0));
				break;
			case SDLK_RIGHT:
				//mp_camera->Translate(Vector3f(STEPSIZE, 0, 0));
				mp_cameraNode->Translate(Vector3f(STEPSIZE,0,0));
				break;
			  default:
			  	break;
			}
	}
}
void DemoApp::HandleMouseMotion(SDL_MouseMotionEvent event)
{
	if(mp_camera)
	{
		//printf("rel : (%d %d)\n", event.xrel, event.yrel);
		//0.225 = 180/800 0.3 = 180/600
		mp_camera->Yaw(-2*event.xrel*HANGLESTEP);
		mp_camera->Pitch(event.yrel*VANGLESTEP);
	}
}
/******************* main function ***************************/
int main()
{
	DemoApp app;
	pthread_create(&fpsThreadId, NULL, FpsThread, NULL);
	app.Run();
	return 0;
}
