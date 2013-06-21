#ifndef _SCENE_NODE_H
#define _SCENE_NODE_H
#include <Node.h>
#include <Export.h>
#include <Entity.h>
#include <string>

class ENGINE_EXPORT SceneNode : public Node{
public:
	SceneNode(const std::string &name);
	SceneNode *CreateChildSceneNode(const std::string &name);

	void AttachEntity(Entity *pEntity);
	void DetachEntity(void);
	Entity *GetAttachedEntity(void);	
	const AxisAlignedBox& GetWorldBoundingBox() const
	{
		return m_worldAABB;
	}
	
	virtual void _Update(bool updateChildren, bool parentHasChanged);	
	/** Tells the SceneNode to update the world bound info it stores.
	*/
	virtual void _UpdateBounds(void);
	
private:
	virtual SceneNode* CreateChildImpl(const std::string &name);
protected:
	AxisAlignedBox m_worldAABB;
	Entity *mp_attachedEntity;
};

#endif
