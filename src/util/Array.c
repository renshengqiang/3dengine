#include <struct.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_MIN_NUM 4
#define ARRAY_MAX_NUM 512

void GdArrayInit(GD_ARRAY *parray, DINT size)
{
	if(size < 0) size = 0;
	parray->max_num = 0;
	parray->length = 0;
	parray->data_size = size;
	parray->array = NULL;
	return;
}

void GdArrayReset(GD_ARRAY *parray)
{
	if(parray->data_size == 0) return;
	if(parray->array != NULL) {
		free(parray->array);
		parray->array = NULL;
	}
	parray->length = 0;
	parray->max_num = 0;
	return;	
}

void *GdArrayGet(GD_ARRAY *parray, DINT i)
{
	DUINT8 *p;
	if(parray->data_size == 0) return NULL;
	if((i < 0)||(i >= parray->length)) return NULL;
	p = (DUINT8 *)(parray->array);
	return (p+i*parray->data_size);
}

void *GdArrayAdd(GD_ARRAY *parray, DINT i, DINT n)
{
	DINT k, m, s;
	DUINT8 *p;
	if((s=parray->data_size) == 0) return NULL;
	k = parray->length;
	if(i == -1) i = k;
	if((i < 0)||(i > k)||(n <= 0)) return NULL;
	m = k+n;
	p = (DUINT8 *)(parray->array);
	if(m > parray->max_num)
	{
		if(m < ARRAY_MIN_NUM)
			m = ARRAY_MIN_NUM;
		else if(m < ARRAY_MAX_NUM)
			m <<= 1;
		else
			m += ARRAY_MAX_NUM;
		p = (DUINT8 *)realloc(p, m*s);
		if(p == NULL) return NULL;
		parray->array = (void *)p;
		parray->max_num = m;
		parray->length = k+n;
	} else {
		parray->length = m;
	}
	p += i*s;
	if((m=k-i) > 0) GdMemMove(p+n*s, p, m*s);
	memset(p, 0, n*s);
	return p;
}

void *GdArraySet(GD_ARRAY *parray, DINT n)
{
	DINT k, m, s;
	DUINT8 *p;

	if(n == 0) {
		GdArrayReset(parray);
		return NULL;
	}

	if((s=parray->data_size) == 0) return NULL;

	m = parray->max_num;
	k = parray->length;
	p = (DUINT8 *)(parray->array);

	if(n == -1) n = k;
	if(n <= 0) return NULL;

	if(n != m)
	{
		p = (DUINT8 *)realloc(p, n*s);
		if(p == NULL) return NULL;
		parray->max_num = n;
		parray->length = n;
		parray->array = (void *)p;
	}

	if((m=(n-k))>0) {
		memset(p+k*s, 0, m*s);
	}
	return p;
}

void GdArrayClear(GD_ARRAY *parray, DINT i, DINT n)
{
	DINT k, m, s;
	DUINT8 *p;
	if((s=parray->data_size) == 0) return;
	k = parray->length;
	if(i == -1) i = k-n;
	if((i < 0)||(i >= k)||(n <= 0)) return;
	m = k-i;
	if(m > n) m = n;
	p = (DUINT8 *)(parray->array)+i*s;
	memset(p, 0, m*s);
	return;
}

void GdArrayDelete(GD_ARRAY *parray, DINT i, DINT n)
{
	DINT k, m, s;
	DUINT8 *p;
	if((s=parray->data_size) == 0) return;
	k = parray->length;
	if(i == -1) i = k-n;
	if((i < 0)||(i >= k)||(n <= 0)) return;
	if((m=k-(i+n)) > 0) {
		p = (DUINT8 *)(parray->array)+i*s;
		GdMemMove(p, p+n*s, m*s);
		i += m;
	}
	parray->length = i;
	if(i < (parray->max_num>>1))
	{
		if(i > ARRAY_MIN_NUM) m = i;
		else m = ARRAY_MIN_NUM;
		if(m != parray->max_num) {
			parray->array = realloc(parray->array, m*s);
			parray->max_num = m;
		}
	}
	return;
}

