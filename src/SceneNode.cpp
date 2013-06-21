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
//-----------------------------------------------------------------------
void SceneNode::_Update(bool updateChildren, bool parentHasChanged)
{
	Node::_Update(updateChildren, parentHasChanged);
	_UpdateBounds();
}
//-----------------------------------------------------------------------
void SceneNode::_UpdateBounds(void)
{
	// Reset bounds first
	m_worldAABB.setNull();

	// Update bounds from own attached objects
	if(mp_attachedEntity)
	{
		m_worldAABB = mp_attachedEntity->GetBoundingBox();
		m_worldAABB.transformAffine(_GetFullTransform());
	}

	// Merge with children
	ChildNodeIterator child;
	for (child =m_childVec.begin(); child != m_childVec.end(); ++child)
	{
	    SceneNode* sceneChild = static_cast<SceneNode*>(*child);
	    m_worldAABB.merge(sceneChild->m_worldAABB);
	}
}