#ifndef _SCENE_NODE_H
#define _SCENE_NODE_H
#include "Node.h"
#include "Export.h"
#include "Entity.h"
#include <string>

class ENGINE_EXPORT SceneNode : public Node{
public:
	SceneNode(const std::string &name);
	SceneNode *CreateChildSceneNode(const std::string &name);

	void AttachEntity(Entity *pEntity);
	void DetachEntity(void);
	Entity *GetAttachedEntity(void);
	
private:
	virtual SceneNode* CreateChildImpl(const std::string &name);
private:
	//Mesh *mp_attachedMesh;
	Entity *mp_attachedEntity;
};

#endif