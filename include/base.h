#ifndef _ENGINE_BASE_H
#define _ENGINE_BASE_H

/*===========================================*/

#define GD_LSB	0x21
#define GD_MSB	0x12

/*DINT, DUINT表示符合体系结构, 能和指针类型互转的整数类型*/
/*DINT_SIZE must == sizeof(DINT)*/
/*GD_ENDIAN: LSB 或MSB */

#if (ARCH_X86 == 1)||(ARCH_ARM == 1)||(ARCH_MIPS == 1)||(ARCH_UNI == 1)

typedef int DINT32;
typedef short DINT16;
typedef signed char DINT8;
typedef unsigned int DUINT32;
typedef unsigned short DUINT16;
typedef unsigned char DUINT8;

typedef int DINT;
typedef unsigned int DUINT;

#define GD_ENDIAN	GD_LSB
#define DINT_SIZE	4	/* must == sizeof(DINT) */

#elif ARCH_X86_64 == 1

#include <sys/types.h>

typedef int32_t DINT32;
typedef int16_t DINT16;
typedef int8_t DINT8;
typedef u_int32_t DUINT32;
typedef u_int16_t DUINT16;
typedef u_int8_t DUINT8;

typedef int64_t DINT;
typedef u_int64_t DUINT;

#define GD_ENDIAN	GD_LSB
#define DINT_SIZE	8	/* must == sizeof(DINT) */

#endif

/*===========================================*/

#define GD_BYTE_PAD	DINT_SIZE
#define GD_BIT_PAD		(8*DINT_SIZE)

#define DINT_MAX	(~(1<<(DINT_SIZE*8-1)))
#define DINT_MIN	(1<<(DINT_SIZE*8-1))

/*===========================================*/

#ifndef None
#define None 0
#endif

#ifndef NULL
#define NULL (void *)0
#endif

#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE	0
#endif

/*boolean value*/
typedef DINT8 GD_BOOL;

typedef float GD_FLOAT;

/*ID类型: 必须能和指针互转, None表示空ID*/
typedef DUINT GD_ID;

/*坐标, 距离和大小的类型, 必须有符号*/

/*===========================================*/

/*设备无关的颜色类型*/

typedef DUINT32 GD_COLOR;

/*全透的颜色*/
#define NULL_COLOR	0

#define GD_B_MASK		0x000000ff
#define GD_G_MASK		0x0000ff00
#define GD_R_MASK		0x00ff0000
#define GD_A_MASK		0xff000000
#define GD_RGB_MASK	0x00ffffff

#define GD_B_SHIFT		0
#define GD_G_SHIFT		8
#define GD_R_SHIFT		16
#define GD_A_SHIFT		24

#define GDBGRA(b,g,r,a) ((GD_COLOR)\
		((((DUINT32)((DUINT8)(b)))<<GD_B_SHIFT)|\
		  (((DUINT32)((DUINT8)(g)))<<GD_G_SHIFT)|\
		  (((DUINT32)((DUINT8)(r)))<<GD_R_SHIFT)|\
		  (((DUINT32)((DUINT8)(a)))<<GD_A_SHIFT)))

#define GD_ALPHA(cv)	((DUINT8)((cv)>>GD_A_SHIFT))
#define GD_RED(cv)		((DUINT8)((cv)>>GD_R_SHIFT))
#define GD_GREEN(cv)	((DUINT8)((cv)>>GD_G_SHIFT))
#define GD_BLUE(cv)		((DUINT8)((cv)>>GD_B_SHIFT))

#define GDBGR(b,g,r)		GDBGRA(b,g,r,0xff)
#define GDRGB(r,g,b)		GDBGRA(b,g,r,0xff)

#define GDYUV(Cb,Y,Cr)  ((DUINT32)((DUINT8)(Cb)|\
	(((DUINT8)(Y))<<8)|\
	(((DUINT8)(Cr))<<16)|\
	(((DUINT8)(Y))<<24)))

#define GD_Cb(yuv) ((DUINT8)(yuv))
#define GD_Y(yuv) ((DUINT8)((yuv)>>8))
#define GD_Cr(yuv) ((DUINT8)((yuv)>>16))

/*===========================================*/

#ifdef __cplusplus
#define GD_EXTERN_C extern "C" {
#define GD_EXTERN_C_END }
#else
#define GD_EXTERN_C
#define GD_EXTERN_C_END
#endif

#define DPRINTF printf
#define EPRINTF printf

#define GD_MIN(a,b)	((a) < (b) ? (a) : (b))
#define GD_MAX(a,b)	((a) > (b) ? (a) : (b))

#define	GD_ABS(a)	((a) >= 0 ? (a) : -(a))

#define UNOVERLAP(x1, y1, w1, h1, x2, y2, w2, h2) \
	((x1)>=(x2)+(w2) || (x2)>=(x1)+(w1) || (y1)>=(y2)+(h2) || (y2)>=(y1)+(h1))

#define OVERLAP(x1, y1, w1, h1, x2, y2, w2, h2) \
	(!UNOVERLAP(x1, y1, w1, h1, x2, y2, w2, h2))

#define MODIFY_RECT(x, y, w, h, maxw, maxh) \
do {\
	if(x < 0) {w+=x; x=0;}\
	if(y < 0) {h+=y; y=0;}\
	if((x+w) > maxw)\
		w = maxw - x;\
	if((y+h) > maxh)\
		h = maxh - y;\
} while(0)

#define GD_LOWBIT(x) ((x) & (~(x) + 1))

/*#define GD_SWAP(a,b) do { a ^= b; b ^= a; a ^= b; } while(0)*/
#define GD_SWAP(a,b) do { typeof(a) t; t=a; a=b; b=t; } while(0)

#define GD_ROUNDUP(n, pad) ((((n)+(pad)-1)/(pad))*(pad))

#define GD_LIKELY(x) __builtin_expect(!!(x), 1)
#define GD_UNLIKELY(x) __builtin_expect(!!(x), 0)

/*#define GD_OFFSETOF(type, member) ((int)&(((type*)0)->member))*/
#define GD_OFFSETOF(type, member) __builtin_offsetof(type, member)

#define GD_ENTRY(p, type, member) ((type *)((char *)(p) - GD_OFFSETOF(type, member)))

#define GD_ARRAY_NUM(a) (sizeof(a)/sizeof(a[0]))

#define _GD_STR_CONJOIN(a,b) a##b
#define GD_STR_CONJOIN(a,b)	_GD_STR_CONJOIN(a,b)

#endif

