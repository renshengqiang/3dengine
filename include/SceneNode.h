#ifndef _SCENE_NODE_H
#define _SCENE_NODE_H
#include <MovableObject.h>
#include <SceneManager.h>
#include <map>
#include <string>

class ENGINE_EXPORT SceneNode{
	public:
		/** Enumeration denoting the spaces which a transform can be relative to.
        */
		enum TransformSpace
				{
					/// Transform is relative to the local space
					TS_LOCAL,
					/// Transform is relative to the space of the parent node
					TS_PARENT,
					/// Transform is relative to world space
					TS_WORLD
				};

		typedef std::map<std::string, SceneNode*> ChildNodeMap;
		typedef ChildNodeMap::iterator ChildNodeMapIterator;
		typedef ChildNodeMap::const_iterator ChildNodeMapConstIterator;
		typedef std::map<std::string, MovableObject*> ObjectMap;
	
		SceneNode();
		~SceneNode();
		virtual void AttachObject(MovableObject *o);
		virtual void DetachObject(MovableObject *o);
		virtual void _update();
		virtual SceneNode* CreateChildSceneNode();
	private:
		SceneNode *m_parent;
		SceneManager *m_creator;
		ChildNodeMap m_children;
		ObjectMap m_object;
		//own transform
		Vector3f m_pos;
		Vector3f m_scale;
		Quaternion m_orientation;		
		//derived transform:防备更换父亲的情况
		//所有的transform都按照scale, rotate,translate的顺序进行的，是对原有的局部坐标系的改变
		//这样做的好处是:物体的位置能够比较清楚的得到
		Vector3f m_derived_pos;
		Vector3f m_derived_scale;
		Quaternion m_derived_orientation;
};

#endif
