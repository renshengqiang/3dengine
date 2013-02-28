#ifndef _SCENE_NODE_H
#define _SCENE_NODE_H
#include "Node.h"
#include "Export.h"
#include "Mesh.h"
#include <string>

class ENGINE_EXPORT SceneNode : public Node{
public:
	SceneNode(const std::string &name);
	SceneNode *CreateChildSceneNode(const std::string &name);
	/**  attach and detach mesh from this node
		@remarks: one SceneNode correspond to one Mesh
	*/
	void AttachMesh(Mesh *mesh);
	void DetachMesh(Mesh *mesh);
	Mesh *GetMesh(void);
protected:
	virtual SceneNode* CreateChildImpl(const std::string &name);
private:
	Mesh *mp_attachedMesh;
};

#endif