#ifndef _SCENE_MANAGER_H
#define _SCENE_MANAGER_H
#include "Export.h"
#include "RenderWindow.h"
#include "Camera.h"
#include "SceneNode.h"
#include "Animation.h"
#include "AnimationState.h"
#include "FrameListener.h"
#include "EventListener.h"
#include "MeshManager.h"
#include "Ray.h"
#include "RaySceneQuery.h"
#include "ParticleSystem.h"
#include <string>
#include <map>
#include <vector>
#include <pthread.h>

extern int sceneFps;
class ENGINE_EXPORT SceneManager{
public:
	enum ManagerType{
		MANAGER_GENERAL = 0x1
	};

	typedef std::map<std::string, Animation*> AnimationList;
	typedef AnimationList::iterator AnimationIterator;
	typedef AnimationList::const_iterator AnimationConstIterator;

	struct RenderItem{
		SceneNode *pNode;
		Matrix4f	transMatrix;
	};
	typedef std::vector<struct RenderItem> RenderQueue;
	typedef RenderQueue::iterator RenderQueueIterator;
	
	SceneManager(enum ManagerType=MANAGER_GENERAL);
	~SceneManager();
	virtual Camera* CreateCamera(Vector3f pos= Vector3f(0,0,0), Vector3f target=Vector3f(0,0,-1), Vector3f up=Vector3f(0,1,0));
	RenderWindow* CreateRenderWindow(int w=800, int h=600);
	void AddFrameListener(FrameListener *frameListener);
	void AddEventListener(EventListener *eventListener);
	void StartRendering();
	SceneNode *GetRootNode(void);
	virtual RaySceneQuery* CreateRayQuery(const Ray& ray);
	//////////////////////////////////////////////////////////////////////
	/** Creates an animation which can be used to animate scene nodes.
	@remarks
		An animation is a collection of 'tracks' which over time change the position / orientation
		of Node objects. In this case, the animation will likely have tracks to modify the position
		/ orientation of SceneNode objects, e.g. to make objects move along a path.
	@par
		You don't need to use an Animation object to move objects around - you can do it yourself
		using the methods of the Node in your FrameListener class. However, when you need relatively
		complex scripted animation, this is the class to use since it will interpolate between
		keyframes for you and generally make the whole process easier to manage.
	@par
		A single animation can affect multiple Node objects (each AnimationTrack affects a single Node).
		In addition, through animation blending a single Node can be affected by multiple animations,
		athough this is more useful when performing skeletal animation (see Skeleton::createAnimation).
	@par
		Note that whilst it uses the same classes, the animations created here are kept separate from the
		skeletal animations of meshes (each Skeleton owns those animations).
	@param name The name of the animation, must be unique within this SceneManager.
	@param length The total length of the animation.
	*/
	virtual Animation *CreateAnimation(const std::string &name, float length);
	
	/** Looks up an Animation object previously created with CreateAnimation
	@note return NULL if the named animation does not exist
	*/
	virtual Animation *GetAnimation(const std::string &name) const;

	/**
		returns whether an animation with the given name exists
	*/
	virtual bool HasAnimation(const std::string &name) const;

	/**
	 	Destroy an animation with the given name
	*/
	virtual void DestroyAnimation(const std::string &name);

	/** 
		Removes all animations created using this SceneManager
	*/
	virtual void DestroyAllAnimations(void);
	/** Create an AnimationState object for managing application of animations.
	@remarks
		You can create Animation objects for animating SceneNode obejcts using the
		createAnimation method. However, in order to actually apply those animations
		you have to call methods on Node and Animation in a particular order (namely
		Node::resetToInitialState and Animation::apply). To make this easier and to
		help track the current time position of animations, the AnimationState object
		is provided. </p>
		So if you don't want to control animation application manually, call this method,
		update the returned object as you like every frame and let SceneManager apply 
		the animation state for you.
	@par
		Remember, AnimationState objects are disabled by default at creation time. 
		Turn them on when you want them using their setEnabled method.
	@par
		Note that any SceneNode affected by this automatic animation will have it's state
		reset to it's initial position before application of the animation. Unless specifically
		modified using Node::setInitialState the Node assumes it's initial state is at the
		origin. If you want the base state of the SceneNode to be elsewhere, make your changes
		to the node using the standard transform methods, then call setInitialState to 
		'bake' this reference position into the node.
	@par
		If the target of your animation is to be a generic AnimableValue, you
		should ensure that it has a base value set (unlike nodes this has no
		default). @see AnimableValue::setAsBaseValue.
	@param animName The name of an animation created already with createAnimation.
	*/
	virtual AnimationState *CreateAnimationState(const std::string &name);
	virtual AnimationState *GetAnimationState(const std::string name);
	virtual bool HasAnimationState(const std::string &name) const;
	virtual void DestroyAnimationState(const std::string &name);
	virtual void DestroyAllAnimationStates(void);
	
	/** update the scene node graph and spatial graph */
	virtual void _UpdateSceneGraph( Camera * cam );
	/** Recurses through the octree determining which nodes are visible. */
	virtual void _FindVisibleObjects(Camera * cam, SceneManager::RenderQueue& renderQueue);

	ParticleSystem* CreateParticleSystem(void);
protected:
	void _ApplySceneAnimations();
	static void* _RenderThreadFunc(void *);

protected:
	enum ManagerType m_managerType;
	Camera *mp_cameraInUse;
	RenderWindow *mp_renderWindow;
	FrameListener *mp_frameListener;
	EventListener *mp_eventListener;
	SceneNode	*mp_rootNode;
	bool m_windowInit, m_windowInitSucceed;
	
	AnimationList m_animationList;
	AnimationStateSet m_animationStateSet;

	pthread_t			m_renderThread;
	RenderQueue		*mp_renderingQueue;
	pthread_mutex_t	m_renderingQueueMutex;
	pthread_cond_t	m_renderingQueueFullCond, m_renderingQueueEmptyCond;
	pthread_mutex_t	m_sdlMutex;
	pthread_cond_t	m_sdlCond;
	
	MeshManager *mMeshManager;
	ParticleSystem *pParticleSystem;
};

#endif
