#include "SceneNode.h"

//-------------------------------------------------------------
SceneNode:: SceneNode(const std::string &name) :
	Node(name),
	mp_attachedEntity(NULL)
{
}
//-------------------------------------------------------------
SceneNode *SceneNode::CreateChildSceneNode(const std::string &name)
{
	return  static_cast<SceneNode*>(this->CreateChild(name));
}
//-------------------------------------------------------------
SceneNode* SceneNode::CreateChildImpl(const std::string &name)
{
	return new SceneNode(name);
}
//-------------------------------------------------------------
void SceneNode::AttachEntity(Entity * pEntity)
{
	mp_attachedEntity = pEntity;
}
//-------------------------------------------------------------
void SceneNode::DetachEntity(void)
{
	mp_attachedEntity = NULL;
}
//-------------------------------------------------------------
Entity *SceneNode::GetAttachedEntity(void)
{
	return mp_attachedEntity;
}
