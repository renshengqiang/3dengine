#ifndef _ENGINE_STRUCT_H
#define _ENGINE_STRUCT_H
#include <stdio.h>
#include "base.h"

/****************** gerneral double linked list structure *************/
typedef struct gd_list {
	struct gd_list *next, *prev;
} GD_LIST;

#define gd_list_entry(ptr,type,member) GD_ENTRY(ptr, type, member)

/*#define gd_list_head_init(name) {&(name), &(name)}*/
/*#define gd_define_list_head(name) GD_LIST name = {&(name), &(name)}*/

#define gd_list_init_head(ptr)	\
do {(ptr)->next = (ptr); (ptr)->prev = (ptr);} while(0);

#define gd_list_init_node(ptr)	\
do {(ptr)->next = NULL; (ptr)->prev = NULL;} while(0);

static inline void gd_list_insert(GD_LIST *pnew, GD_LIST *prev, GD_LIST *next)
{
//	if((pnew->next == NULL)&&(pnew->prev == NULL))
//	{
		pnew->next = next;
		pnew->prev = prev;
		prev->next = pnew;
		next->prev = pnew;
//	} else {
//		printf("gd_list_insert error\n");
//	}
}

static inline void gd_list_add(GD_LIST *pnew, GD_LIST *head)
{
	gd_list_insert(pnew, head, head->next);
}

static inline void gd_list_add_tail(GD_LIST *pnew, GD_LIST *head)
{
	gd_list_insert(pnew, head->prev, head);
}

static inline void gd_list_del(GD_LIST *entry)
{
	if((entry->next != NULL)&&(entry->prev != NULL))
	{	/*防止了重复删除*/
		entry->prev->next = entry->next;
		entry->next->prev = entry->prev;
		entry->next = NULL;
		entry->prev = NULL;
	}
}
static inline int gd_list_empty(const GD_LIST *head)
{
	return head->next == head;
}

/* 把phead 链表插入到prev和next节点之间*/
static inline void gd_list_insert_list(GD_LIST *phead, GD_LIST *prev, GD_LIST *next)
{
	if(phead->next != phead)
	{
		/*phead 至少有一个节点*/
		phead->next->prev = prev;
		phead->prev->next = next;
		prev->next = phead->next;
		next->prev = phead->prev;
		/*phead 置空*/
		phead->next = phead;
		phead->prev = phead;
	}
}

static inline void gd_list_add_list_tail(GD_LIST *src_head, GD_LIST *dst_head)
{
	gd_list_insert_list(src_head, dst_head->prev, dst_head);
}


#define gd_list_for_each(pos, head)				\
	for(pos=(head)->next;pos!=(head);pos=(pos)->next)

#define gd_list_for_each_prev(pos, head)			\
	for(pos=(head)->prev;pos!=(head);pos=(pos)->prev)

#define gd_list_for_each_safe(pos, n, head)			\
	for(pos=(head)->next,n=(pos)->next; pos!=(head); pos=n, n=(pos)->next)

#define gd_list_for_each_prev_safe(pos, n, head)		\
	for(pos=(head)->prev,n=(pos)->prev; pos!=(head); pos=n, n=(pos)->prev)

#endif

