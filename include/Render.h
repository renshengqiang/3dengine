#ifndef _RENDER_H
#define _RENDER_H
#include <GL/glew.h>
#include <math_3d.h>

typedef struct IndexObj INDEX_OBJ;

INDEX_OBJ* CreateIndexObject(int n, const unsigned int *p);
INDEX_OBJ* CreateIndexObject2(int n, const unsigned short *p);
void DestroyIndexObject(INDEX_OBJ *indexObject);

/*-------------------------------------------------------*/
//注意:vertex中的所有数据都是浮点类型的，所以不必记录数据类型
#define COORD_SHIFT		28
#define COLOR_SHIFT		24
#define NORMAL_SHIFT	20
#define TEXTURE_SHIFT	16
#define TEXTURE2_SHIFT	12

#define COORD_3 (3<<COORD_SHIFT)
#define COORD_4 (4<<COORD_SHIFT)
#define COORD_MASK (0x0f<<COORD_SHIFT)
#define COORD_I(n) ((n&0x0f)<<COORD_SHIFT)

#define COLOR_3	(3<<COLOR_SHIFT)
#define COLOR_4	(4<<COLOR_SHIFT)
#define COLOR_MASK (0x0f<<COLOR_SHIFT)
#define COLOR_I(n) ((n&0x0F)<<COLOR_SHIFT)

#define NORMAL_3 (3<<NORMAL_SHIFT)
#define NORMAL_MASK (0x0f<<NORMAL_SHIFT)
#define NORMAL_I(n) ((n&0x0f)<<NORMAL_SHIFT)

#define TEXTURE_2 (3<<TEXTURE_SHIFT)
#define TEXTURE_3 (4<<TEXTURE_SHIFT)
#define TEXTURE_MASK (0x0f<<TEXTURE_SHIFT)
#define TEXTURE_I(n) ((n&0x0f)<<TEXTURE_SHIFT)

#define TEXTURE2_2 (3<<TEXTURE2_SHIFT)
#define TEXTURE2_3 (4<<TEXTURE2_SHIFT)
#define TEXTURE2_MASK (0x0f<<TEXTURE2_SHIFT)
#define TEXTURE2_I(n) ((n&0x0f)<<TEXTURE2_SHIFT)

typedef struct VertexObj VERTEX_OBJ;

VERTEX_OBJ* CreateVertexObject(int elements,  int n,int span, const float *p);
VERTEX_OBJ* CreateVertexObject2(int elements,  int n,int span, const double *p);
void DestroyVertexObject(VERTEX_OBJ *vertexObject);

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
bool InitRender();
void ClearBuffer();
void DrawObject(int type,const INDEX_OBJ *indexObj,const VERTEX_OBJ *vertexObj,const PIXEL_OBJ *pixelObj, struct context *contx);
void DrawOjectUseShader(const INDEX_OBJ *indexObj,const VERTEX_OBJ *vertexObj,const PIXEL_OBJ *pixelObj);
void SetModelViewMatrix(const Matrix4f *modelviewMatrix);
void SetProjectMatrix(const Matrix4f *projectMatrix);
//for shader use
void SetTranslateMatrix(const GLuint matrixLocation, const Matrix4f *transMatrix);
#endif

