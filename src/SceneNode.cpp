#include "SceneNode.h"

//-------------------------------------------------------------
SceneNode:: SceneNode(const std::string &name) :
	Node(name),
	mp_attachedMesh(0)
{
}
//-------------------------------------------------------------
SceneNode *SceneNode::CreateChildSceneNode(const std::string &name)
{
	return  static_cast<SceneNode*>(this->CreateChild(name));
}
SceneNode* SceneNode::CreateChildImpl(const std::string &name)
{
	return new SceneNode(name);
}
//-------------------------------------------------------------
void SceneNode::AttachMesh(Mesh * mesh)
{
	mp_attachedMesh = mesh;
	return;
}
//-------------------------------------------------------------
void SceneNode::DetachMesh(Mesh * mesh)
{
	if(mp_attachedMesh == mesh){
		mp_attachedMesh=NULL;
	}
	return;
}
//-------------------------------------------------------------
Mesh *SceneNode::GetMesh(void)
{
	return mp_attachedMesh;
}
