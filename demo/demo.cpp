#include <Engine.h>
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
};
DemoApp::DemoApp(bool ifUseShader)
{
	m_ifUseShader = ifUseShader;
	mp_sceneManager = new SceneManager();
	mp_renderWindow = mp_sceneManager->CreateRenderWindow(800,600);
	mp_camera = mp_sceneManager->CreateCamera(Vector3f(0.0f,0.0f,0.0f),Vector3f(0, 0, -1.0f));
	//初始化  水平方向180度， 竖直方向-90度,以配合SDL
	//mp_camera = mp_sceneManager->CreateCamera(Vector3f(0.0f,0.0f,10.0f),Vector3f(-0.01f, 1.0f, 0.0f));
	mp_sceneManager->AddFrameListener(this);
	mp_sceneManager->AddEventListener(this);
}
DemoApp::~DemoApp()
{
	delete mp_sceneManager;
}
bool DemoApp::FrameQueued(long timeSinceLastFrame)
{
	fps++;
	return FrameListener::FrameQueued(timeSinceLastFrame);
}
void DemoApp::CreateScene(void)
{
	Mesh *mesh = new Mesh();
	mesh->LoadMesh("phoenix_ugv.md2");
	SceneNode *rootNode = mp_sceneManager->GetRootNode();
	SceneNode *node = rootNode->CreateSceneNode();
	node->Translate(0,0,-100, SceneNode::TS_LOCAL);
	SceneNode *node2 = node->CreateSceneNode();
	node2->Translate(Vector3f(100, 0, 0), SceneNode::TS_LOCAL);
	node2->Rotate(Vector3f(0,1,0), -90, SceneNode::TS_LOCAL);

	node->AttachMesh(mesh);
	node2->AttachMesh(mesh);
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
		mp_camera->Yaw(-event.xrel*HANGLESTEP);
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
