#include "Engine.h"
#include <SDL/SDL.h>
#include <pthread.h>

#define STEPSIZE 10
#define HANGLESTEP (180/(float)800)
#define VANGLESTEP (180/(float)600)

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
	sceceFps = fps;
	fps = 0;
    }
	return NULL;
}
/****************** class definition ************/
class DemoApp:public FrameListener, public EventListener
{
	public:
		DemoApp(bool ifUseShader);
		~DemoApp();
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
		AnimationState *mp_animationState1,*mp_animationState2;
		AnimationState *mp_skeletonAnimationState;
		Entity *pEntity1, *pEntity2, *pEntity3;
		
};
DemoApp::DemoApp(bool ifUseShader)
{
	m_ifUseShader = ifUseShader;
	mp_sceneManager = new SceneManager();
	mp_renderWindow = mp_sceneManager->CreateRenderWindow(800,600);
	mp_camera = mp_sceneManager->CreateCamera(Vector3f(0.0f,0.0f,0.0f),Vector3f(0, 0, -1.0f));
	mp_sceneManager->AddFrameListener(this);
	mp_sceneManager->AddEventListener(this);
	pEntity1=pEntity2=NULL;
	mp_animationState1 = NULL;
	mp_animationState2 = NULL;
}
DemoApp::~DemoApp()
{
	delete mp_sceneManager;
	delete pEntity1;
	delete pEntity2;
	delete pEntity3;
}
bool DemoApp::FrameQueued(long timeSinceLastFrame)
{	
	static float time = 0;
	time+=timeSinceLastFrame/(float)1000;
	fps++;
	if(mp_animationState1){
		mp_animationState1->AddTime(timeSinceLastFrame/(float)1000);
	}
	if(mp_animationState2){
		mp_animationState2->AddTime(timeSinceLastFrame/(float)1000);
	}
	if(mp_skeletonAnimationState){
		mp_skeletonAnimationState->AddTime(timeSinceLastFrame/(float)1000);
	}
	return FrameListener::FrameQueued(timeSinceLastFrame);
}
void DemoApp::CreateScene(void)
{
	pEntity1 = new Entity("./models/phoenix_ugv.md2");
	pEntity2 = new Entity("./models/boblampclean.md5mesh");
	pEntity3 = new Entity("./models/boblampclean.md5mesh");

	SceneNode *rootNode = mp_sceneManager->GetRootNode();
	SceneNode *node1 = rootNode->CreateChildSceneNode("childnode1");
	node1->Translate(0,0,-500, SceneNode::TS_LOCAL);
	SceneNode *node2 = rootNode->CreateChildSceneNode("childnode2");
	node2->Translate(Vector3f(100, 0, -500), SceneNode::TS_LOCAL);
	node2->Rotate(Vector3f(1,0,0), -90, SceneNode::TS_LOCAL);
	SceneNode *node3 = rootNode->CreateChildSceneNode("childnode3");
	node3->Translate(Vector3f(100, -100, -250), SceneNode::TS_LOCAL);
	node3->Rotate(Vector3f(1,0,0), -90, SceneNode::TS_LOCAL);
	
	node1->AttachEntity(pEntity1);
	node2->AttachEntity(pEntity2);
	node3->AttachEntity(pEntity3);

	//create one scenenode animation
	Animation *animation = mp_sceneManager->CreateAnimation("transAnim1",9);
	NodeAnimationTrack *track = animation->CreateNodeTrack(0, node1);
	TransformKeyFrame *keyFrame = track->CreateNodeKeyFrame(3);
	keyFrame->SetTranslate(Vector3f(-100, 0,0));
	keyFrame = track->CreateNodeKeyFrame(9);
	keyFrame->SetTranslate(Vector3f(0,0,0));
	keyFrame = track->CreateNodeKeyFrame(6);
	keyFrame->SetTranslate(Vector3f(-100, 0, -100));
	keyFrame->SetRotation(Quaternion(Vector3f(0,1,0),-90));
	keyFrame->SetScale(Vector3f(2,2,2));

	node1->SetInitialState();
	mp_animationState1= mp_sceneManager->CreateAnimationState("transAnim1");
	mp_animationState1->SetEnabled(true);
	mp_animationState1->SetLoop(true);
	
	//another scenenode animation
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
	mp_animationState2= mp_sceneManager->CreateAnimationState("transAnim2");
	mp_animationState2->SetEnabled(true);
	mp_animationState2->SetLoop(true);

	mp_skeletonAnimationState = pEntity2->GetAnimationState("");
	mp_skeletonAnimationState->SetEnabled(true);
	mp_skeletonAnimationState->SetLoop(false);
}
void DemoApp::Run()
{
	CreateScene();
	mp_sceneManager->StartRendering(m_ifUseShader);
}
/************************ handle sdl events **********************************/
void DemoApp::ProcessEvents()
{
	SDL_Event event;    /* Grab all the events off the queue. */    
	while( SDL_PollEvent( &event ) ) {        
		switch( event.type ) {        
			case SDL_KEYDOWN:            /* Handle key presses. */            
				HandleKeyDown( &event.key.keysym );            
				break;     
			case SDL_MOUSEMOTION:
				HandleMouseMotion(event.motion);
				break;
			case SDL_QUIT:            /* Handle quit requests (like Ctrl-c). */            
				mp_sceneManager->QuitFromRendering();
				break;        
			}    
	}
}
void DemoApp::HandleKeyDown( SDL_keysym* keysym )
{    
	if(mp_camera){
		switch( keysym->sym ) {    
			case SDLK_ESCAPE:        
				mp_sceneManager->QuitFromRendering();
				break;    
			case SDLK_DOWN:
			case SDLK_s:
				mp_camera->Translate(Vector3f(0,0,STEPSIZE));
				break;
			case SDLK_UP:
			case SDLK_w:
				mp_camera->Translate(Vector3f(0,0,-STEPSIZE));
				break;
			case SDLK_LEFT:
				mp_camera->Translate(Vector3f(-STEPSIZE, 0,0));
				break;
			case SDLK_RIGHT:
				mp_camera->Translate(Vector3f(STEPSIZE, 0, 0));
				break;
			  default:       
			  	break;    
			}
	}
}
void DemoApp::HandleMouseMotion(SDL_MouseMotionEvent event)
{
	if(mp_camera){
		//printf("rel : (%d %d)\n", event.xrel, event.yrel);
		//0.225 = 180/800 0.3 = 180/600
		mp_camera->Yaw(-2*event.xrel*HANGLESTEP);
		mp_camera->Pitch(-event.yrel*VANGLESTEP);
	}
}
/******************* main function ***************************/
int main()
{
	DemoApp *app = new DemoApp(true);
	pthread_create(&fpsThreadId, NULL, FpsThread, NULL);
	app->Run();
	return 0;
}
