#include "Animation.h"
#include <stdlib.h>

//-----------------------------------------------------------------------
Animation::Animation(const std::string &name, float length)
	:m_name(name),
	m_length(length),
	m_interpolationMode(IM_LINEAR),
	m_rotateInterpolationMode(RIM_LINEAR)
{
	gd_list_init_head(&m_nodeTrackListHead);
}
//-----------------------------------------------------------------------
Animation:: ~Animation()
{
	DestroyAllNodeTracks();
}
//-----------------------------------------------------------------------
const std::string &Animation::GetName(void) const
{
	return m_name;
}
//-----------------------------------------------------------------------
float Animation::GetLength(void) const
{
	return m_length;
}
//-----------------------------------------------------------------------
void Animation::SetLength(float len)
{
	m_length = len;
}
//-----------------------------------------------------------------------
Animation::InterpolationMode Animation::GetInterpolationMode(void)
{
	return m_interpolationMode;
}
void  Animation::SetRotationInterpolationMode(RotationInterpolationMode rim)
{
	m_rotateInterpolationMode = rim;
}
//-----------------------------------------------------------------------
Animation::RotationInterpolationMode Animation::GetRotationInterpolationMode(void)
{
	return m_rotateInterpolationMode;
}
//-----------------------------------------------------------------------
NodeAnimationTrack *Animation::CreateNodeTrack(unsigned short handle, const std::string &name)
{
	GD_LIST *pos,*addPos;
	struct NodeTrackListNode *listNode;

	//寻找插入的位置，链表中按照handle大小顺序排列，循环结束，addPos即为插入位置
	addPos = &m_nodeTrackListHead;
	gd_list_for_each(pos,&m_nodeTrackListHead){
		listNode = GD_ENTRY(pos,struct NodeTrackListNode,siblingList);
		if(listNode->handle == handle){
			fprintf(stderr, "Animation::CreateNodeTrack : Node track with the specified hangle %d already exists\n",handle);
			return NULL;
		}
		else if(listNode->handle < handle)
			addPos = &(listNode->siblingList);
		else
			break;
	}//gd_list_for_each
	NodeAnimationTrack *ret = new NodeAnimationTrack(this, name);
	listNode = (struct NodeTrackListNode *)malloc(sizeof(struct NodeTrackListNode));
	listNode->handle = handle;
	listNode->track = ret;
	gd_list_insert(&(listNode->siblingList),addPos,addPos->next);
	return ret;
}
//-----------------------------------------------------------------------
NodeAnimationTrack *Animation::CreateNodeTrack(unsigned short handle, Node *node)
{
	NodeAnimationTrack *ret = CreateNodeTrack(handle, node->GetName());
	ret->SetAssociatedNode(node);
	return ret;
}
//-----------------------------------------------------------------------
unsigned short Animation::GetNumNodeTracks(void) const
{
	GD_LIST *pos;
	int n = 0;

	gd_list_for_each(pos,&m_nodeTrackListHead) ++n;
	return n;
}
//-----------------------------------------------------------------------
bool Animation::HasNodeTrack(unsigned short handle) const
{
	GD_LIST *pos;
	struct NodeTrackListNode *listNode;

	gd_list_for_each(pos, &m_nodeTrackListHead){
		listNode = GD_ENTRY(pos,struct NodeTrackListNode,siblingList);
		if(listNode->handle == handle) return true;
	}
	return false;
}
//-----------------------------------------------------------------------
NodeAnimationTrack *Animation::GetNodeTrack(unsigned short handle) const
{
	GD_LIST *pos;
	struct NodeTrackListNode *listNode;

	gd_list_for_each(pos, &m_nodeTrackListHead){
		listNode = GD_ENTRY(pos,struct NodeTrackListNode,siblingList);
		if(listNode->handle == handle) return listNode->track;
	}
	return NULL;
}
//-----------------------------------------------------------------------
void Animation::DestoryNodeTrack(unsigned short handle)
{
	GD_LIST *pos;
	struct NodeTrackListNode *listNode;

	gd_list_for_each(pos,&m_nodeTrackListHead){
		listNode = GD_ENTRY(pos,struct NodeTrackListNode,siblingList);
		if(listNode->handle == handle){
			gd_list_del(pos);
			delete listNode->track;
			free(listNode);
			return;
		}
	}
	return;
}
//-----------------------------------------------------------------------
void Animation::DestroyAllNodeTracks(void)
{
	GD_LIST *pos,*temp;
	struct NodeTrackListNode *listNode;

	gd_list_for_each_safe(pos,temp,&m_nodeTrackListHead){
		listNode = GD_ENTRY(pos,struct NodeTrackListNode,siblingList);
		gd_list_del(pos);
		delete listNode->track;
		free(listNode);
	}
	return;
}
//-----------------------------------------------------------------------
void Animation::Apply(float timePos, float weight, float scale)
{
	GD_LIST *pos;
	struct NodeTrackListNode *listNode;

	gd_list_for_each(pos,&m_nodeTrackListHead){
		listNode = GD_ENTRY(pos, struct NodeTrackListNode, siblingList);
		listNode->track->Apply(timePos,weight,scale);
	}
	return;
}
//-----------------------------------------------------------------------
void Animation::ApplyToNode(Node *node, float timePos, float weight, float scale)
{
	GD_LIST *pos;
	struct NodeTrackListNode *listNode;

	gd_list_for_each(pos,&m_nodeTrackListHead){
		listNode = GD_ENTRY(pos, struct NodeTrackListNode, siblingList);
		listNode->track->ApplyToNode(node,timePos,weight,scale);
	}
	return;
}
