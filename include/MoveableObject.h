#ifndef _MOVEABLE_OBJECT_H
#define _MOVEABLE_OBJECT_H
#include <AxisAlignedBox.h>

class Node;
class SceneNode;
//MoveableObject���ǿ��Ը��ݳ����ڵ㵽���ƶ��Ľڵ�
class MoveableObject
{
public:
	MoveableObject();
	virtual ~MoveableObject();

	virtual Node* GetParentNode(void);
	virtual SceneNode* GetParentSceneNode(void);
	virtual void DetachFromParent(void);
	virtual void SetParent(Node *pParentNode);

	void UpdateMoved(void);
	bool IsMovded(void);
	bool Visible(void) const;
	void SetVisible(bool visible);
	
	/* Retrieves the local axis-aligned bounding box for this object
	*/
	virtual const AxisAlignedBox& GetBoundingBox(void) const = 0;
	
protected:
	Node	*mp_parentNode;
	bool		m_visible;	
	bool		m_moved;
};

#endif
