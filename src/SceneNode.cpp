#include "SceneNode.h"

//-----------------------------------------------------------------------------
SceneNode:: SceneNode(const std::string &name) :
	Node(name),
	mp_attachedMoveableObject(NULL)
{
}
//-----------------------------------------------------------------------------
SceneNode *SceneNode::CreateChildSceneNode(const std::string &name)
{
	return  static_cast<SceneNode*>(this->CreateChild(name));
}
//-----------------------------------------------------------------------------
SceneNode* SceneNode::CreateChildImpl(const std::string &name)
{
	return new SceneNode(name);
}
//-----------------------------------------------------------------------------
void SceneNode::AttachObject(MoveableObject *pObject)
{
	mp_attachedMoveableObject = pObject;
	pObject->SetParent(this);
}
//-----------------------------------------------------------------------------
void SceneNode::DetachObject(void)
{
	mp_attachedMoveableObject = NULL;
}
//-----------------------------------------------------------------------------
MoveableObject* SceneNode::GetAttachedMoveableObject(void)
{
	return mp_attachedMoveableObject;
}
//-----------------------------------------------------------------------------
void SceneNode::_Update(bool updateChildren, bool parentHasChanged)
{
	Node::_Update(updateChildren, parentHasChanged);
	_UpdateBounds();
}
//-----------------------------------------------------------------------------
void SceneNode::_NeedUpdate(bool forceParentUpdate)
{
	Node::_NeedUpdate(forceParentUpdate);
	if(mp_attachedMoveableObject)
	{
		mp_attachedMoveableObject->UpdateMoved();
	}
}
//-----------------------------------------------------------------------------
void SceneNode::_UpdateBounds(void)
{
	// Reset bounds first
	m_worldAABB.setNull();

	// Update bounds from own attached objects
	if(mp_attachedMoveableObject)
	{
		m_worldAABB = mp_attachedMoveableObject->GetBoundingBox();
		m_worldAABB.transformAffine(_GetFullTransform());
	}
/*
	// Merge with children
	ChildNodeIterator child;
	for (child =m_childVec.begin(); child != m_childVec.end(); ++child)
	{
	    SceneNode* sceneChild = static_cast<SceneNode*>(*child);
	    m_worldAABB.merge(sceneChild->m_worldAABB);
	}
*/
}
