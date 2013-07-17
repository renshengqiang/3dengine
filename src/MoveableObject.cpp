#include "MoveableObject.h"
#include "SceneNode.h"

MoveableObject::MoveableObject():
	mp_parentNode(NULL),
	m_visible(false),
	m_moved(false)
{
}
MoveableObject::~MoveableObject()
{
	DetachFromParent();
}
Node* MoveableObject::GetParentNode(void)
{
	return mp_parentNode;
}
SceneNode* MoveableObject::GetParentSceneNode(void)
{
	return static_cast<SceneNode *>(mp_parentNode);
}
void MoveableObject::DetachFromParent(void)
{
	if(mp_parentNode)
	{
		//SceneNode *pParentNode = static_cast<SceneNode *>(mp_parentNode);

		//SceneNode需要加入detachObject
		//pParentNode->DetachObject(void);
	}
}
void MoveableObject::SetParent(Node *pParentNode)
{
	if(pParentNode)
	{
		//SceneNode *pParentNode = static_cast<SceneNode *>(mp_parentNode);
		 //pParentNode->AttachEntity(this);
	}
	mp_parentNode = pParentNode;
}
void MoveableObject::UpdateMoved(void)
{
	m_moved = true;
}
bool MoveableObject::IsMovded()
{
	return m_moved;
}
bool MoveableObject::Visible(void) const
{
	return m_visible;
}
void MoveableObject::SetVisible(bool visible)
{
	m_visible = visible;
}

