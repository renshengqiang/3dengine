#include "Animation.h"
#include <stdlib.h>

//-----------------------------------------------------------------------------
Animation::Animation(const std::string &name, float length)
	:m_name(name),
	m_length(length),
	m_interpolationMode(IM_LINEAR),
	m_rotateInterpolationMode(RIM_LINEAR)
{
}
//-----------------------------------------------------------------------------
Animation:: ~Animation()
{
	DestroyAllNodeTracks();
}
//-----------------------------------------------------------------------------
const std::string &Animation::GetName(void) const
{
	return m_name;
}
//-----------------------------------------------------------------------------
float Animation::GetLength(void) const
{
	return m_length;
}
//-----------------------------------------------------------------------------
void Animation::SetLength(float len)
{
	m_length = len;
}
//-----------------------------------------------------------------------------
Animation::InterpolationMode Animation::GetInterpolationMode(void)
{
	return m_interpolationMode;
}
void  Animation::SetRotationInterpolationMode(RotationInterpolationMode rim)
{
	m_rotateInterpolationMode = rim;
}
//-----------------------------------------------------------------------------
Animation::RotationInterpolationMode Animation::GetRotationInterpolationMode(void)
{
	return m_rotateInterpolationMode;
}
//-----------------------------------------------------------------------------
NodeAnimationTrack *Animation::CreateNodeTrack(unsigned short handle, const std::string &name)
{
	if(m_nodeTrackList.find(handle) != m_nodeTrackList.end()){
		fprintf(stderr, "Animation::CreateNodeTrack : Node track with the specified hangle %d already exists\n",handle);
			return NULL;
	}
	NodeAnimationTrack *ret = new NodeAnimationTrack(this, name);
	m_nodeTrackList.insert(std::make_pair(handle, ret));
	TransformKeyFrame*pKeyFrame = ret->CreateNodeKeyFrame(0);//默认添加第一帧
	return ret;
}
//-----------------------------------------------------------------------------
NodeAnimationTrack *Animation::CreateNodeTrack(unsigned short handle, Node *node)
{
	NodeAnimationTrack *ret = CreateNodeTrack(handle, node->GetName());
	ret->SetAssociatedNode(node);
	return ret;
}
//-----------------------------------------------------------------------------
unsigned short Animation::GetNumNodeTracks(void) const
{
	return m_nodeTrackList.size();
}
//-----------------------------------------------------------------------------
bool Animation::HasNodeTrack(unsigned short handle) const
{
	return m_nodeTrackList.find(handle)!=m_nodeTrackList.end();
}
//-----------------------------------------------------------------------------
NodeAnimationTrack *Animation::GetNodeTrack(unsigned short handle) const
{
	NodeTrackConstIterator iter = m_nodeTrackList.find(handle);
	if(iter != m_nodeTrackList.end()) return iter->second;
	return NULL;
}
//-----------------------------------------------------------------------------
Animation::NodeTrackIterator Animation::GetNodeTrackBegin(void)
{
	return m_nodeTrackList.begin();
}
//-----------------------------------------------------------------------------
const Animation::NodeTrackIterator Animation::GetNodeTrackEnd(void)
{
	return m_nodeTrackList.end();
}
//-----------------------------------------------------------------------------
void Animation::DestoryNodeTrack(unsigned short handle)
{
	NodeTrackIterator iter = m_nodeTrackList.find(handle);
	if(iter != m_nodeTrackList.end())
	{
		delete iter->second;
		m_nodeTrackList.erase(iter);
	}
	return;
}
//-----------------------------------------------------------------------------
void Animation::DestroyAllNodeTracks(void)
{
	for(NodeTrackIterator iter = m_nodeTrackList.begin(); iter != m_nodeTrackList.end(); ++iter)
	{
		delete iter->second;
	}
	m_nodeTrackList.clear();
	return;
}
//-----------------------------------------------------------------------------
void Animation::Apply(float timePos, float weight, float scale)
{
	for(NodeTrackIterator iter = m_nodeTrackList.begin(); iter!= m_nodeTrackList.end(); ++iter)
	{
		iter->second->Apply(timePos, weight, scale);
	}
	return;
}
//-----------------------------------------------------------------------------
void Animation::ApplyToNode(Node *node, float timePos, float weight, float scale)
{
	for(NodeTrackIterator iter = m_nodeTrackList.begin(); iter!= m_nodeTrackList.end(); ++iter)
	{
		iter->second->ApplyToNode(node, timePos, weight, scale);
	}
	return;
}
