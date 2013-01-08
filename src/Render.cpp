#include <Render.h>
#include <Texture.h>
#include <Shader.h>
#include <math_3d.h>
#include <util.h>

typedef GLenum DataType;
struct IndexObj {
	GLsizei n;//num of index
	DataType dataType;
	GLsizei offset;
	GLuint object;
	IndexObj(){
		object = 0;
		}
};
typedef unsigned short DataSize;
struct VertexObj{
	GLsizei n;//num of vertex
	int elements;
	DataSize span;
	DataSize coordSize;
	DataSize coordOffset;
	DataSize colorSize;
	DataSize colorOffset;
	DataSize normalSize;
	DataSize normalOffset;
	DataSize textureCoordSize;
	DataSize textureOffset;
	DataSize texture2CoordSize;
	DataSize texture2Offset;
	GLuint object;
	DataType dataType;
};
struct PixelObj{
	GLenum textureTarget;
	GLenum textureUnit;
	GLuint object;	
};

INDEX_OBJ* CreateIndexObject(int n, const unsigned int *p)
{
	struct IndexObj *obj = (struct IndexObj*)malloc(sizeof(struct IndexObj));	
	if(obj!=NULL){
		glGenBuffers(1, &(obj->object));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->object);
		obj->n = n;
		obj->dataType = GL_UNSIGNED_INT;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, n*sizeof(unsigned int), p, GL_STATIC_DRAW);
	}
	return obj;
}

INDEX_OBJ* CreateIndexObject2(int n,const unsigned short *p)
{
	struct IndexObj *obj = (struct IndexObj *)malloc(sizeof(struct IndexObj));	
	if(obj!=NULL){
		glGenBuffers(1, &(obj->object));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->object);
		obj->n = n;
		obj->dataType= GL_UNSIGNED_SHORT;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, n*sizeof(unsigned short), p, GL_STATIC_DRAW);
	}
	return obj;
}
void DestroyIndexObject(INDEX_OBJ *indexObject)
{
	if(indexObject && indexObject->object!=INVALID_OBJECT_VALUE){
		glDeleteBuffers(1, &(indexObject->object));
	}
}

VERTEX_OBJ* CreateVertexObject(int elements, int n, int span, const float *p)
{
	int dataOffset = 0;
	struct VertexObj *obj = (struct VertexObj *)malloc(sizeof(struct VertexObj));	
	if(obj!=NULL){
		glGenBuffers(1, &(obj->object));
		glBindBuffer(GL_ARRAY_BUFFER, obj->object);
		glBufferData(GL_ARRAY_BUFFER, n*span, p, GL_STATIC_DRAW);
		obj->span = span;
		obj->n = n;
		obj->dataType = GL_FLOAT;
		obj->elements = elements;

		obj->coordOffset = 0;
		switch(elements&COORD_MASK){
			case COORD_3:
				dataOffset += 3;
				obj->coordSize = 3;
				break;
			case COORD_4:
				dataOffset += 4;
				obj->coordSize = 4;
				break;
			default:
				obj->coordSize = 0;
				break;
		}
		obj->colorOffset = dataOffset * sizeof(float);
		switch(elements&COLOR_MASK){
			case COLOR_3:
				dataOffset+=3;
				obj->colorSize = 3;
				break;
			case COLOR_4:
				dataOffset+=4;
				obj->colorSize = 4;
				break;
			default:
				obj->colorSize = 0;
				break;
		}
		obj->normalOffset = dataOffset * sizeof(float);
		switch(elements&NORMAL_MASK){
			case NORMAL_3:
				dataOffset+=3;
				obj->normalSize = 3;
				break;
			default:
				obj->normalSize = 0;
				break;
		}
		obj->textureOffset = dataOffset*sizeof(float);
		switch(elements&TEXTURE_MASK){
			case TEXTURE_2:
				dataOffset+=2;
				obj->textureCoordSize = 2;
				break;
			case TEXTURE_3:
				dataOffset+=3;
				obj->textureCoordSize = 3;
				break;
			default:
				obj->textureCoordSize = 0;
				break;
		}
		obj->texture2Offset = dataOffset * sizeof(float);
		switch(elements&TEXTURE2_MASK){
			case TEXTURE2_2:
				dataOffset+=2;
				obj->texture2CoordSize = 2;
				break;
			case TEXTURE2_3:
				dataOffset+=3;
				obj->textureCoordSize = 3;
				break;
			default:
				obj->texture2CoordSize = 0;
				break;
		}
		//.............. other texture units
	}
	return obj;
}
void DestroyVertexObject(VERTEX_OBJ *vertexObject)
{
	if(vertexObject && vertexObject->object != INVALID_OBJECT_VALUE){
		glDeleteBuffers(1, &(vertexObject->object));
	}
	return;
}


PIXEL_OBJ *CreatePixelObject(const char *file,GLenum textureUnit, GLenum textureTarget)
{
	struct PixelObj *obj  = (struct PixelObj *)malloc(sizeof(struct PixelObj));
	if(obj!=NULL){
		Texture *texture = new Texture(textureTarget, file);
		texture->Load();
		obj->object = texture->GetTextureObj();
		obj->textureUnit = textureUnit;
		obj->textureTarget = textureTarget;
	}
	return obj;
}
PIXEL_OBJ *CreatePixelObject2(GLuint object, GLenum textureUnit, GLenum textureTarget)
{
	struct PixelObj *pixelObject = (struct PixelObj *)malloc(sizeof(struct PixelObj));
	if(pixelObject!=NULL){
		pixelObject->object = object;
		pixelObject->textureUnit = textureUnit;
		pixelObject->textureTarget = textureTarget;
	}
	return pixelObject;
}
/*
 绘制步骤:
 * 1. 使能相关顶点数据相关客户端状态
 * 2. 初始化顶点数组数据位置
 * 3. 使用顶点数组函数绘制渲染
 * 4. 禁止使能顶点数组相关客户端状态
 */
void DrawObject(int type, const INDEX_OBJ *indexObj, const VERTEX_OBJ *vertexObj, const PIXEL_OBJ *pixelObj, struct context *contx)
{	
	if(pixelObj){
		glEnable(pixelObj->textureTarget);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glActiveTexture(pixelObj->textureUnit);
		glBindTexture(pixelObj->textureTarget,pixelObj->object);
	}
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertexObj->object);
	glVertexPointer(vertexObj->coordSize, vertexObj->dataType,vertexObj->span, (const GLvoid *)vertexObj->coordOffset);
	if(vertexObj->colorSize > 0){
		glEnableClientState(GL_COLOR_ARRAY);
	}
	if(vertexObj->normalSize > 0){
		glEnableClientState(GL_NORMAL_ARRAY);
	}
	if(vertexObj->textureCoordSize > 0){
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(vertexObj->textureCoordSize,vertexObj->dataType,vertexObj->span,(const GLvoid *)vertexObj->textureOffset);
	}
	glEnableClientState(GL_INDEX_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexObj->object);

	//Rendering action
	glDrawElements(GL_TRIANGLES, indexObj->n,indexObj->dataType,0);
	
	if(pixelObj){
		glDisable(pixelObj->textureTarget);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	if(vertexObj->colorSize > 0){
		glDisableClientState(GL_COLOR_ARRAY);
	}
	if(vertexObj->normalSize > 0){
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	if(vertexObj->textureCoordSize > 0){
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}	
	return ;
}
void DrawOjectUseShader(const INDEX_OBJ *indexObj, const VERTEX_OBJ *vertexObj, const PIXEL_OBJ *pixelObj)
{
	glEnableVertexAttribArray(g_vertexPostionLocation);
	glEnableVertexAttribArray(g_vertexTexCoordLocation);
	glBindBuffer(GL_ARRAY_BUFFER, vertexObj->object);
	glVertexAttribPointer(g_vertexPostionLocation, vertexObj->coordSize, 
		vertexObj->dataType, GL_FALSE, vertexObj->span, (const GLvoid *)vertexObj->coordOffset);
	glVertexAttribPointer(g_vertexTexCoordLocation, vertexObj->textureCoordSize, 
		vertexObj->dataType, GL_FALSE, vertexObj->span,(const GLvoid *)vertexObj->textureOffset);
	glUniform1i(g_samplerLocation, 0);
	glActiveTexture(pixelObj->textureUnit);
    glBindTexture(pixelObj->textureTarget, pixelObj->object);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexObj->object);
	glDrawElements(GL_TRIANGLES, indexObj->n,indexObj->dataType,0);
	glDisableVertexAttribArray(g_vertexPostionLocation);
	glDisableVertexAttribArray(g_vertexTexCoordLocation);
}
void SetModelViewMatrix(const Matrix4f * modelviewMatrix)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadTransposeMatrixf(&(modelviewMatrix->m[0][0]));
	return;
}
void SetProjectMatrix(const Matrix4f * projectMatrix)
{
	glMatrixMode(GL_PROJECTION);
	glLoadTransposeMatrixf(&(projectMatrix->m[0][0]));
	return;
}
void SetTranslateMatrix(const GLuint matrixLocation,const Matrix4f * transMatrix)
{
	glUniformMatrix4fv(matrixLocation, 1, GL_TRUE, &(transMatrix->m[0][0]));
	return;
}
bool InitRender()
{
	//init glew
	GLenum res = glewInit();
	if(res != GLEW_OK){
		fprintf(stderr, "SceneManager::InitRendering: glew init failure\n");
		return false;
	}
	//init state
	glClearColor(0,0,0,0);
	glEnable(GL_DEPTH_TEST);	
	return true;
}
void ClearBuffer()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}