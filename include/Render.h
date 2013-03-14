#ifndef _RENDER_H
#define _RENDER_H
#include "math_3d.h"
#include <GL/glew.h>

typedef struct IndexObj INDEX_OBJ;

INDEX_OBJ* CreateIndexObject(int n, const unsigned int *p);
INDEX_OBJ* CreateIndexObject2(int n, const unsigned short *p);
void DestroyIndexObject(INDEX_OBJ *indexObject);

/*-------------------------------------------------------*/
//下面的掩码表示不同的数据在elements这个数中的位置，用户创建VETTEX_OBJ
#define COORD_SHIFT		0
#define COLOR_SHIFT		3
#define COLOR2_SHIFT		6
#define NORMAL_SHIFT		9
#define TEXTURE_SHIFT		12
#define TEXTURE2_SHIFT	15
//bone weight的组合放在最后一个位置[id1,id2,id3,id4,weight1,weight2,weight3,weight4]
#define BONE_SHIFT		30


#define COORD_3 (3<<COORD_SHIFT)
#define COORD_4 (4<<COORD_SHIFT)
#define COORD_MASK (0x07<<COORD_SHIFT)
#define COORD_I(n) ((n&0x07)<<COORD_SHIFT)

#define COLOR_3	(3<<COLOR_SHIFT)
#define COLOR_4	(4<<COLOR_SHIFT)
#define COLOR_MASK (0x07<<COLOR_SHIFT)
#define COLOR_I(n) ((n&0x07)<<COLOR_SHIFT)

#define COLOR2_3  (3<<COLOR2_SHIFT)
#define COLOR2_4  (4<<COLOR2_SHIFT)
#define COLOR2_MASK (0x07<<COLOR2_SHIFT)
#define COLOR2_I(n)((n&0x07)<<COLOR_SHIFT)

#define NORMAL_3 (3<<NORMAL_SHIFT)
#define NORMAL_MASK (0x07<<NORMAL_SHIFT)
#define NORMAL_I(n) ((n&0x07)<<NORMAL_SHIFT)

#define TEXTURE_2 (3<<TEXTURE_SHIFT)
#define TEXTURE_3 (4<<TEXTURE_SHIFT)
#define TEXTURE_MASK (0x07<<TEXTURE_SHIFT)
#define TEXTURE_I(n) ((n&0x07)<<TEXTURE_SHIFT)

#define TEXTURE2_2 (3<<TEXTURE2_SHIFT)
#define TEXTURE2_3 (4<<TEXTURE2_SHIFT)
#define TEXTURE2_MASK (0x07<<TEXTURE2_SHIFT)
#define TEXTURE2_I(n) ((n&0x07)<<TEXTURE2_SHIFT)

#define BONE_4  (1<<BONE_SHIFT)
#define BONE_MASK (0x03<<BONE_SHIFT)
#define BONE_I(n) ((n&0x03)<<BONE_SHIFT)

typedef struct VertexObj VERTEX_OBJ;

VERTEX_OBJ *CreateVertexObject(int elements,  int n);
void VertexObjectPushElement(VERTEX_OBJ *vobj, int element, float *p);
void VertexObjectPushElementAll(VERTEX_OBJ *vobj, int element, float *p);
void VertexObjectEnd(VERTEX_OBJ *vobj);

void DestroyVertexObject(VERTEX_OBJ *vertexObject);

void VertexObjectReplace(VERTEX_OBJ * vobj,const float * p);
void VertexObjectReplace(VERTEX_OBJ *obj, VERTEX_OBJ *obj2);
void VertexObjectReset(VERTEX_OBJ *obj);

VERTEX_OBJ* CreateVertexObject2(int elements,  int n,int span, const double *p);

/*-------------------------------------------------------*/
typedef struct PixelObj PIXEL_OBJ;
#define PIXEL4_BGR8880
PIXEL_OBJ* CreatePixelObject(const char *file,GLenum textureUnit, GLenum textureTarget);
PIXEL_OBJ *CreatePixelObject2(GLuint object, GLenum textureUnit, GLenum textureTarget);
//int CreatePixelObject(pixel_type,width, height, depth, line_bytes,char *p);
/*-------------------------------------------------------*/
struct context {
	int mask;
	float alpha;
	int pixel2_obj;
};
bool InitGlew(void);
void GetGLInfo(void);
bool InitRenderState(void);
void ClearBuffer(void);
void DrawOverlay(int fps);
void DrawObject(int type,const INDEX_OBJ *indexObj,const VERTEX_OBJ *vertexObj,const PIXEL_OBJ *pixelObj, struct context *contx);
void DrawOjectUseShader(const INDEX_OBJ *indexObj,const VERTEX_OBJ *vertexObj,const PIXEL_OBJ *pixelObj);
void SetModelViewMatrix(const Matrix4f *modelviewMatrix);
void SetProjectMatrix(const Matrix4f *projectMatrix);
//for shader use
void SetTranslateMatrix(const GLuint matrixLocation, const Matrix4f *transMatrix);
void SetIntValue(const GLuint location, GLint value);

#endif

