#include "Node.h"
#include <stdio.h>

//----------------------------------------------------------
Node::Node():
	mp_parent(0),
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
	_needUpdate();
}
//----------------------------------------------------------
Node::Node(const std::string  &name):
		mp_parent(0),
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
	m_name = name;
	_needUpdate();
}
//----------------------------------------------------------
Node::~Node()
{
	for(ChildNodeIterator iter=m_childVec.begin(); iter!=m_childVec.end(); ++iter){
		delete (*iter);
	}
	m_childVec.clear();
}
 //-----------------------------------------------------------------------
const std::string& Node::GetName(void) const
{
	return m_name;
}
//----------------------------------------------------------
Node *Node::GetParent(void) const
{
	return mp_parent;
}
//----------------------------------------------------------
void Node::SetParent(Node * parent)
{
	if(mp_parent != parent){
		mp_parent = parent;
		_needUpdate();
	}
	return;
}
//----------------------------------------------------------
void Node::Reset(void)
{
	m_position = Vector3f(0.0f, 0.0f, 0.0f);
	m_scale = Vector3f(1.0f, 1.0f, 1.0f);
	m_orientation = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

	for(ChildNodeIterator iter = m_childVec.begin(); iter != m_childVec.end(); ++iter){
		(*iter)->Reset();
	 }
}
//----------------------------------------------------------
const Quaternion &Node::_GetDerivedOrientation(void)
{
	if(m_needSelfUpdate){
		_UpdateFromParent();
	}
	return m_derivedOrientation;
}
//----------------------------------------------------------
const Vector3f &Node::_GetDerivedPosition(void)
{
	if(m_needSelfUpdate){
		_UpdateFromParent();
	}
	return m_derivedPosition;
}
//----------------------------------------------------------
const Vector3f &Node::_GetDerivedScale(void)
{
	if(m_needSelfUpdate){
		_UpdateFromParent();
	}
	return m_derivedScale;
}
//----------------------------------------------------------
const Matrix4f& Node::_GetFullTransform(void)
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
//----------------------------------------------------------
void Node::_Update(bool updateChildren,bool parentHasChanged)
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
	    	for(ChildNodeIterator iter = m_childVec.begin(); iter != m_childVec.end(); ++iter){
				(*iter)->_Update(true, true);
	    	}
	    }
	    m_needChildUpdate = false;
	}
}
//----------------------------------------------------------
void Node::_UpdateFromParent(void)
{
	updateFromParentImpl();
}
void Node::updateFromParentImpl(void)
{
	if (mp_parent)
	{
		// Update orientation
		const Quaternion& parentOrientation = mp_parent->_GetDerivedOrientation();
		// Combine orientation with that of parent
		m_derivedOrientation = parentOrientation * m_orientation;
		// Update scale
		const Vector3f& parentScale = mp_parent->_GetDerivedScale();
		// Scale own position by parent scale, NB just combine
		// as equivalent axes, no shearing
		m_derivedScale = parentScale * m_scale;
		// Change position vector based on parent's orientation & scale
		m_derivedPosition = (parentScale * m_position) * parentOrientation;
		// Add altered position vector to parents
		m_derivedPosition += mp_parent->_GetDerivedPosition();
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
//----------------------------------------------------------
void Node::_needUpdate(void)
{
	m_needSelfUpdate = true;
	m_needChildUpdate = true;
	m_cachedTransformOutOfDate = true;
}
//----------------------------------------------------------
void Node::AddChild(Node * child)
{
	if(child->GetParent()){
		fprintf(stderr, "Node::AddChild: child already has a parent, ADD FAILURE\n");
		return;
	}
	m_childVec.push_back(child);
	child->SetParent(this);
}
//----------------------------------------------------------
Node *Node::CreateChild(const std::string& name)
{
	Node* newNode = CreateChildImpl(name);
	this->AddChild(newNode);
	
	return newNode;
}
//----------------------------------------------------------
Node *Node::CreateChildImpl(const  std::string& name)
{
	Node *newNode = new Node(name);
	return newNode;
}
//----------------------------------------------------------
void Node::RemoveChild(Node *child)
{
	if (child){
		for(ChildNodeIterator iter = m_childVec.begin(); iter != m_childVec.end(); ++iter){
			if((*iter) == child){
				m_childVec.erase(iter);
				break;
			}//if
		}//for
	}//child
}
//----------------------------------------------------------
unsigned short Node::NumChildren(void) const
{
	return static_cast< unsigned short >(m_childVec.size());
}
//----------------------------------------------------------
Node *Node::GetChild(unsigned short index) const
{
	if( index < m_childVec.size() )
		return m_childVec[index];
	else
		return NULL;
}
//----------------------------------------------------------
Node *Node::GetChild(const std::string &name) const
{
	Node *p_childNode;
	for(ConstChildNodeIterator iter = m_childVec.begin(); iter != m_childVec.end(); ++iter){
		if((*iter)->GetName() == name) return *iter;
		else if((p_childNode = (*iter)->GetChild(name)) != NULL )  return p_childNode;
	}
	return NULL;
}
//----------------------------------------------------------
const Quaternion &Node::GetOrientation(void) const
{
	return m_orientation;
}
//----------------------------------------------------------
void Node::SetOrientation(const Quaternion &q)
{
	m_orientation = q;
	m_orientation.Normalize();
	_needUpdate();
	return;
}
//----------------------------------------------------------
void Node::SetOrientation(float w, float x, float y, float z)
{
	SetOrientation(Quaternion(x,y,z,w));
	return;
}
//----------------------------------------------------------
void Node::ResetOrientation(void)
{
	m_orientation = Quaternion(0,0,0,1);
	_needUpdate();
	return;
}
//----------------------------------------------------------
const Vector3f &Node::GetPosition(void)
{
	return m_position;
}
//----------------------------------------------------------
void Node::SetPosition(const Vector3f &v)
{
	m_position = v;
	_needUpdate();
	return;
}
//----------------------------------------------------------
void Node::SetPosition(float x, float y, float z)
{
	SetPosition(Vector3f(x,y,z));
	return;
}
//----------------------------------------------------------
const Vector3f &Node::GetScale(void)
{
	return m_scale;
}
//----------------------------------------------------------
void Node::SetScale(const Vector3f &v)
{
	m_scale = v;
	_needUpdate();
	return;
}
//----------------------------------------------------------
void Node::SetScale(float x, float y, float z)
{
	SetScale(Vector3f(x,y,z));
	return;
}
//----------------------------------------------------------
void Node::Translate(const Vector3f &d, TransformSpace relativeTo)
{
    switch(relativeTo)
    {
    case TS_LOCAL:
        // position is relative to parent so transform downwards
        m_position += d * m_orientation;
    	break;
    case TS_WORLD:
        // position is relative to parent so transform upwards
        if (mp_parent)
        {
            m_position += (d * mp_parent->_GetDerivedOrientation().Inverse())
                / mp_parent->_GetDerivedScale();
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
    _needUpdate();
}
//----------------------------------------------------------
void Node::Translate(float x, float y, float z, TransformSpace relativeTo)
{
	Translate(Vector3f(x,y,z),relativeTo);
	return;
}
//----------------------------------------------------------
void Node::Rotate(const Quaternion &q, TransformSpace relativeTo)
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
    _needUpdate();
}
//----------------------------------------------------------
void Node::Rotate(const Vector3f &axis, float angle, TransformSpace relativeTo)
{
	Rotate(Quaternion(axis, angle), relativeTo);
	return;
}
//----------------------------------------------------------
void Node::Roll(float angle, TransformSpace relativeTo)
{
	Rotate(Vector3f(0.0f, 0.0f, 1.0f),angle, relativeTo);
	return;
}
//----------------------------------------------------------
void Node::Pitch(float angle, TransformSpace relativeTo)
{
	Rotate(Vector3f(1.0f, 0.0f, 0.0f), angle, relativeTo);
}
//----------------------------------------------------------
void Node::Yaw(float angle, TransformSpace relativeTo)
{
	Rotate(Vector3f(0.0f,1.0f,0.0f),angle,relativeTo);
}
//----------------------------------------------------------
void Node::Scale(const Vector3f &v)
{
	m_scale *= v;
	_needUpdate();
}
//----------------------------------------------------------
void Node::Scale(float x, float y, float z)
{
	Scale(Vector3f(x, y, z));
	return;
}
//----------------------------------------------------------
void Node::SetInitialState(void)
{
	m_initialPosition = m_position;
	m_initialOrientation = m_orientation;
	m_initialScale = m_scale;
}
//----------------------------------------------------------
void Node::ResetToInitialState(void)
{
	m_position = m_initialPosition;
	m_orientation = m_initialOrientation;
	m_scale = m_initialScale;

	_needUpdate();
}
