#include <SceneNode.h>
#include <stdio.h>

SceneNode::SceneNode():
	m_creator(0),
	m_parent(0),
	m_mesh(0),
	m_needSelfUpdate(false),
	m_needChildUpdate(false),
	m_cachedTransformOutOfDate(false),
	m_orientation(0,0,0,1),
	m_position(0,0,0),
	m_scale(1,1,1),
	m_initialPosition(0,0,0),
	m_initialOrientation(0,0,0,1),
	m_initialScale(1,1,1),
	m_derivedOrientation(0,0,0,1),
	m_derivedPosition(0,0,0),
	m_derivedScale(1,1,1)
{
	gd_list_init_head(&m_childHead);
	needUpdate();
}
SceneNode::~SceneNode()
{
	
}
SceneNode *SceneNode::GetParent(void) const
{
	return m_parent;
}
void SceneNode::SetParent(SceneNode * parent)
{
	if(m_parent != parent){
		m_parent = parent;
		needUpdate();
	}
	return;
}
const Quaternion &SceneNode::_GetDerivedOrientation(void)
{
	if(m_needSelfUpdate){
		_UpdateFromParent();
	}
	return m_derivedOrientation;
}
const Vector3f &SceneNode::_GetDerivedPosition(void)
{
	if(m_needSelfUpdate){
		_UpdateFromParent();
	}
	return m_derivedPosition;
}
const Vector3f &SceneNode::_GetDerivedScale(void)
{
	if(m_needSelfUpdate){
		_UpdateFromParent();
	}
	return m_derivedScale;
}
const Matrix4f& SceneNode::_GetFullTransform(void)
{
	//矩阵运算计算量较大，使用下面的条件以避免无谓的计算
	if (m_cachedTransformOutOfDate)
	{
	    m_cachedTransform.MakeTransform(
	        _GetDerivedPosition(),
	        _GetDerivedScale(),
	        _GetDerivedOrientation());
		m_needSelfUpdate = false;
	    m_cachedTransformOutOfDate = false;
	}
	return m_cachedTransform;
}
void SceneNode::_Update(bool updateChildren,bool parentHasChanged)
{
	// See if we should process everyone
	if (m_needSelfUpdate || parentHasChanged)
	{
	    // Update transforms from parent
	    _UpdateFromParent();
	}

	if(updateChildren)
	{
	    if (m_needChildUpdate || parentHasChanged)
	    {
	    	GD_LIST *pos;
			struct ChildList *listNode;
			
	    	gd_list_for_each(pos,&(m_childHead)){
				listNode = GD_ENTRY(pos,struct ChildList,siblingList);
				listNode->node->_Update(true,true);
	    	}
	    }
	    m_needChildUpdate = false;
	}
}
void SceneNode::_UpdateFromParent(void)
{
	if (m_parent)
	{
	    // Update orientation
	    const Quaternion& parentOrientation = m_parent->_GetDerivedOrientation();
	    // Combine orientation with that of parent
	    m_derivedOrientation = parentOrientation * m_orientation;
	    // Update scale
	    const Vector3f& parentScale = m_parent->_GetDerivedScale();
        // Scale own position by parent scale, NB just combine
        // as equivalent axes, no shearing
        m_derivedScale = parentScale * m_scale;
	    // Change position vector based on parent's orientation & scale
	    m_derivedPosition = (parentScale * m_position) * parentOrientation;
	    // Add altered position vector to parents
	    m_derivedPosition += m_parent->_GetDerivedPosition();
	}
	else
	{
	    // Root node, no parent
	    m_derivedOrientation = m_orientation;
	    m_derivedPosition = m_position;
	    m_derivedScale = m_scale;
	}
	m_cachedTransformOutOfDate = true;
	m_needSelfUpdate = false;
}
void SceneNode::needUpdate(void)
{
	m_needSelfUpdate = true;
	m_needChildUpdate = true;
	m_cachedTransformOutOfDate = true;
}
void SceneNode::AddChild(SceneNode * child)
{
	if(child->GetParent()){
		fprintf(stderr, "SceneNode::AddChild: child already has a parent, ADD FAILURE\n");
		return;
	}
	struct ChildList *childNode = (struct ChildList *)malloc(sizeof(struct ChildList));
	if(childNode == NULL){
		fprintf(stderr, "SceneNode::AddChild: memory error, ADD FAILURE\n");
		return;
	}
	childNode->node = child;
	gd_list_add_tail(&(childNode->siblingList),&(m_childHead));
	child->SetParent(this);
}
SceneNode *SceneNode::CreateSceneNode(void)
{
	SceneNode *childNode = new SceneNode();
	
	if(childNode!=NULL){
		AddChild(childNode);
	}	
	return childNode;
}
void SceneNode::RemoveChild(SceneNode *node)
{
	GD_LIST *pos;
	struct ChildList *child;

	gd_list_for_each(pos,&(m_childHead)){
		child = GD_ENTRY(pos,struct ChildList,siblingList);
		if(child->node == node){
			gd_list_del(pos);
			free(child);
			return;
		}		
	}
	return;
}
unsigned short SceneNode::NumChildren(void) const
{
	GD_LIST *pos;
	unsigned short n=0;

	gd_list_for_each(pos,&(m_childHead)){
		++n;
	}
	return n;
}
SceneNode *SceneNode::GetChild(unsigned short index) const
{
	GD_LIST *pos;
	struct ChildList *child=NULL;
	unsigned short n=0;
	gd_list_for_each(pos,&(m_childHead)){
		if(index==n){
			child = gd_list_entry(pos,struct ChildList,siblingList);
			return child->node;
		}
		++n;
	}
	return NULL;
}
const Quaternion &SceneNode::GetOrientation(void) const
{
	return m_orientation;
}
void SceneNode::SetOrientation(const Quaternion &q)
{
	m_orientation = q;
	m_orientation.Normalize();
	needUpdate();
	return;
}
void SceneNode::SetOrientation(float w, float x, float y, float z)
{
	SetOrientation(Quaternion(x,y,z,w));
	return;
}
void SceneNode::ResetOrientation(void)
{
	m_orientation = Quaternion(0,0,0,1);
	needUpdate();
	return;
}
const Vector3f &SceneNode::GetPosition(void)
{
	return m_position;
}
void SceneNode::SetPosition(const Vector3f &v)
{
	m_position = v;
	needUpdate();
	return;
}
void SceneNode::SetPosition(float x, float y, float z)
{
	SetPosition(Vector3f(x,y,z));
	return;
}
const Vector3f &SceneNode::GetScale(void)
{
	return m_scale;
}
void SceneNode::SetScale(const Vector3f &v)
{
	m_scale = v;
	needUpdate();
	return;
}
void SceneNode::SetScale(float x, float y, float z)
{
	SetScale(Vector3f(x,y,z));
	return;
}
void SceneNode::Translate(const Vector3f &d, TransformSpace relativeTo)
{
    switch(relativeTo)
    {
    case TS_LOCAL:
        // position is relative to parent so transform downwards
        m_position += d * m_orientation;
    	break;
    case TS_WORLD:
        // position is relative to parent so transform upwards
        if (m_parent)
        {
            m_position += (d * m_parent->_GetDerivedOrientation().Inverse())
                / m_parent->_GetDerivedScale();
        }
        else
        {
            m_position += d;
        }
    	break;
    case TS_PARENT:
        m_position += d;
        break;
    }
    needUpdate();
}
void SceneNode::Translate(float x, float y, float z, TransformSpace relativeTo)
{
	Translate(Vector3f(x,y,z),relativeTo);
	return;
}
void SceneNode::Rotate(const Quaternion &q, TransformSpace relativeTo)
{
	// Normalise quaternion to avoid drift
	Quaternion qnorm = q;
	qnorm.Normalize();

    switch(relativeTo)
    {
    case TS_PARENT:
        // Rotations are normally relative to local axes, transform up
        m_orientation = qnorm * m_orientation;
        break;
    case TS_WORLD:
        // Rotations are normally relative to local axes, transform up
        m_orientation = m_orientation * _GetDerivedOrientation().Inverse()
            * qnorm * _GetDerivedOrientation();
        break;
    case TS_LOCAL:
        // Note the order of the mult, i.e. q comes after
        m_orientation = m_orientation * qnorm;
        break;
    }
    needUpdate();
}
void SceneNode::Rotate(const Vector3f &axis, float angle, TransformSpace relativeTo)
{
	Rotate(Quaternion(axis, angle), relativeTo);
	return;
}
void SceneNode::Roll(float angle, TransformSpace relativeTo)
{
	Rotate(Vector3f(0.0f, 0.0f, 1.0f),angle, relativeTo);
	return;
}
void SceneNode::Pitch(float angle, TransformSpace relativeTo)
{
	Rotate(Vector3f(1.0f, 0.0f, 0.0f), angle, relativeTo);
}
void SceneNode::Yaw(float angle, TransformSpace relativeTo)
{
	Rotate(Vector3f(0.0f,1.0f,0.0f),angle,relativeTo);
}
void SceneNode::Scale(const Vector3f &v)
{
	m_scale *= v;
	needUpdate();
}
void SceneNode::Scale(float x, float y, float z)
{
	Scale(Vector3f(x, y, z));
	return;
}
void SceneNode::SetInitialState(void)
{
	m_initialPosition = m_position;
	m_initialOrientation = m_orientation;
	m_initialScale = m_scale;
}
void SceneNode::ResetToInitialState(void)
{
	m_position = m_initialPosition;
	m_orientation = m_initialOrientation;
	m_scale = m_initialScale;

	needUpdate();
}

////////////////////////////////////////////////////////////////
void SceneNode::AttachMesh(Mesh * mesh)
{
	m_mesh = mesh;
	return;
}
void SceneNode::DetachMesh(Mesh * mesh)
{
	if(m_mesh == mesh){
		m_mesh=NULL;
	}
	return;
}
Mesh *SceneNode::GetMesh(void)
{
	return m_mesh;
}
