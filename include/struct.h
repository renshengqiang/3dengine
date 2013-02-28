#ifndef _ENGINE_STRUCT_H
#define _ENGINE_STRUCT_H
#include "base.h"
#include <stdio.h>
/*==================================================*/
#define GdMemMove(dst, src, n) memmove(dst, src, n)
/*void GdMemMove(DUINT8 *dst, const DUINT8 *src, DINT n)
{
	if(((dst+n) <= src) || ((src+n) <= dst)) {
		memcpy(dst, src, n);
		return;
	}

	if (src < dst) {
		dst += n;
		src += n;
		while (n--)
			*--dst = *--src;
	}else{
		while (n--)
			*dst++ = *src++;
	}
}*/
/*==================================================*/

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
	{	/*��ֹ���ظ�ɾ��*/
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

/* ��phead ������뵽prev��next�ڵ�֮��*/
static inline void gd_list_insert_list(GD_LIST *phead, GD_LIST *prev, GD_LIST *next)
{
	if(phead->next != phead)
	{
		/*phead ������һ���ڵ�*/
		phead->next->prev = prev;
		phead->prev->next = next;
		prev->next = phead->next;
		next->prev = phead->prev;
		/*phead �ÿ�*/
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
		
/*************************************************************************/
/*        							array								 */
/*************************************************************************/
/*����һ���ɱ䳤�����飬�мǲ��ܷ�gd_list, gd_btree, gd_queue �Ȳ���realloc�ĳ�Ա*/
typedef struct gd_array {
	DINT max_num;
	DINT length;
	DINT data_size;
	void *array;/*(length*data_size),(max_num*data_size)*/
} GD_ARRAY;

#define GD_ARRAY_LENGTH(a) ((a).length)
#define GD_ARRAY_HEAD(a) ((a).array)
#ifdef __cplusplus
extern "C" {
#endif
void GdArrayInit(GD_ARRAY *parray, DINT size);
void GdArrayReset(GD_ARRAY *parray);
void *GdArrayGet(GD_ARRAY *parray, DINT i);
void *GdArrayAdd(GD_ARRAY *parray, DINT i, DINT n); /*i=-1��ʾ��β������*/
void GdArrayDelete(GD_ARRAY *parray, DINT i, DINT n); /*i=-1��ʾ��β������*/
void GdArrayClear(GD_ARRAY *parray, DINT i, DINT n);
/*ֱ�Ӱ�array�Ĵ�С����Ϊn�������ڲ�����������ռ�*/
void *GdArraySet(GD_ARRAY *parray, DINT n);
#ifdef __cplusplus
}
#endif

#endif/*_ENGINE_STRUCT_H*/
