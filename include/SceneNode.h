#ifndef _SCENE_NODE_H
#define _SCENE_NODE_H
#include <Mesh.h>
#include <struct.h>

//递归包含，此处做前向声明
class SceneManager;
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
		struct ChildList{
			SceneNode *node;
			GD_LIST siblingList;
		};
		SceneNode();
		~SceneNode();
		SceneNode *GetParent(void) const;
		void SetParent(SceneNode *parent);
		const Quaternion &_GetDerivedOrientation(void);
		const Vector3f &_GetDerivedPosition(void);
		const Vector3f &_GetDerivedScale(void);
		const Matrix4f &_GetFullTransform(void);
		void _Update(bool updateChildren, bool parentHasChanged);
		void _UpdateFromParent(void);
		void needUpdate(void);
		void AddChild(SceneNode *child);
		SceneNode *CreateSceneNode(void);
		void RemoveChild(SceneNode *child);
		unsigned short NumChildren(void) const;
		SceneNode *GetChild(unsigned short index) const;
		const Quaternion &GetOrientation(void) const;
		void SetOrientation(const Quaternion &q);
		void SetOrientation(float w, float x, float y, float z);
		void ResetOrientation(void);
		const Vector3f &GetPosition(void);
		void SetPosition(const Vector3f &v);
		void SetPosition(float x, float y, float z);
		const Vector3f &GetScale(void);
		void SetScale(const Vector3f &v);
		void SetScale(float x, float y, float z);
		void Translate(const Vector3f &d, TransformSpace relativeTo=TS_PARENT);
		void Translate(float x, float y, float z, TransformSpace relativeTo=TS_PARENT);
		void Rotate(const Quaternion &q, TransformSpace relativeTo=TS_PARENT);
		void Rotate(const Vector3f &axis, float angle, TransformSpace relativeTo=TS_PARENT);
		void Roll(float angle, TransformSpace relativeTo=TS_PARENT);
		void Pitch(float angle, TransformSpace relativeTo=TS_PARENT);
		void Yaw(float angle, TransformSpace relativeTo=TS_PARENT);
		void Scale(const Vector3f &v);
		void Scale(float x, float y, float z);
		 /** Sets the current transform of this node to be the 'initial state' ie that
            position / orientation / scale to be used as a basis for delta values used
            in keyframe animation.
        @remarks
            You never need to call this method unless you plan to animate this node. If you do
            plan to animate it, call this method once you've loaded the node with it's base state,
            ie the state on which all keyframes are based.
        @par
            If you never call this method, the initial state is the identity transform, ie do nothing.
        */
		void SetInitialState(void);
		void ResetToInitialState(void);
		/////////////////////////////////////////////////////////////////////////
		void AttachMesh(Mesh *mesh);
		void DetachMesh(Mesh *mesh);
		Mesh *GetMesh(void);
	private:
		SceneManager *m_creator;
		SceneNode *m_parent;
		GD_LIST m_childHead;
		Mesh *m_mesh;	

		//Flag to indicate own transform from parent is out of date
		bool m_needSelfUpdate;
		// Flag indicating that all children need to be updated
		bool m_needChildUpdate;
		// Flag indicating that transform matrix need to be recompute
		bool m_cachedTransformOutOfDate;
		// Stores the orientation of the node relative to it's parent.
        Quaternion m_orientation;
        // Stores the position/translation of the node relative to its parent.
        Vector3f m_position;
        /// Stores the scaling factor applied to this node
        Vector3f m_scale;	
        // The position to use as a base for keyframe animation
        Vector3f m_initialPosition;
        // The orientation to use as a base for keyframe animation
        Quaternion m_initialOrientation;
        // The scale to use as a base for keyframe animation
        Vector3f m_initialScale;

		Matrix4f m_cachedTransform;
		/** Cached combined orientation.
            @par
                This member is the orientation derived by combining the
                local transformations and those of it's parents.
                This is updated when _UpdateFromParent is called by the
                SceneManager or the nodes parent
        */
        Quaternion m_derivedOrientation; 

        /** Cached combined position.
            @par
                This member is the position derived by combining the
                local transformations and those of it's parents.
                This is updated when _UpdateFromParent is called by the
                SceneManager or the nodes parent.
        */
        Vector3f m_derivedPosition;

        /** Cached combined scale.
            @par
                This member is the position derived by combining the
                local transformations and those of it's parents.
                This is updated when _UpdateFromParent is called by the
                SceneManager or the nodes parent.
        */
        Vector3f m_derivedScale;	
};
#endif
