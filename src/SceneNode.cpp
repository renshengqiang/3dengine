#include <SceneNode.h>

SceneNode::SceneNode()
{
	m_parent = NULL;
	m_creator = NULL;
	m_scale = Vector3f(1.0f, 1.0f, 1.0f);
	m_pos = Vector3f(0.0f, 0.0f, 0.0f);
	m_orientation = Quaternion(0.0f,0.0f,0.0f,1.0f);
	m_derived_scale = Vector3f(1.0f, 1.0f, 1.0f);
	m_derived_pos = Vector3f(0.0f, 0.0f, 0.0f);
	m_derived_orientation = Quaternion(0.0f,0.0f,0.0f,1.0f);
}
SceneNode::~SceneNode()
{
	//��ɾ�����ӣ����ɾ���Լ�
}
void SceneNode::AttachObject(MovableObject * o)
{
}
void SceneNode::DetachObject(MovableObject * o)
{
}
void SceneNode::_update()
{
}
SceneNode *SceneNode::CreateChildSceneNode()
{
	return NULL;
}