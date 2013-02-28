#include "Render.h"
#include "Texture.h"
#include "Shader.h"
#include "math_3d.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

typedef GLenum DataType;
struct IndexObj {
	GLsizei n;//number of index
	DataType dataType;
	GLsizei offset;
	GLuint object;
	IndexObj(){
		object = 0;
	}
};

typedef  short DataSize;
/*
	*Num:表示该数据的维数，为0表示没有
	*Offset:表示该数据在结构中的字节偏移量
*/
struct ElementInfo{
	DataSize coordNum;
	DataSize coordOffset;
	
	DataSize colorNum;
	DataSize colorOffset;
	
	DataSize color2Num;
	DataSize color2Offset;

	DataSize normalNum;//0 or 3
	DataSize normalOffset;

	DataSize textureCoordNum;
	DataSize textureCoordOffset;

	DataSize textureCoord2Num;
	DataSize textureCoord2Offset;

	DataSize boneSize;//0 or 1
	DataSize boneOffset;
};
struct MeshData{
	void *meshData;
	int pushedNum; 
};
struct VertexObj{
	GLuint object;
	GLuint replaceObject;
	//num of vertex
	GLsizei numVertex;
	DataSize stride;
	DataType dataType;/*GL_FLOAT OR GL_DOUBLE*/
	struct MeshData meshData;
	struct ElementInfo elementInfo;
};

struct PixelObj{
	GLuint object;
	GLenum textureTarget;
	GLenum textureUnit;
};
void _GetGLErrorInfo()
{
	int errorId = glGetError();
	switch (errorId)
	{
		case GL_NO_ERROR:
			return;
		case GL_INVALID_ENUM:
			printf ("GL Invalid Enum\n");
			return;
		case GL_INVALID_VALUE:
			printf ("GL Invalid Value\n");
			return;
		case GL_INVALID_OPERATION:
			printf ("GL Invalid Operation\n");
			return;
		case GL_OUT_OF_MEMORY:
			printf ("GL Out Of Memory\n");
			return;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			printf ("GL Invalid FrameBuffer Operation\n");
			return;
		case  GL_STACK_OVERFLOW:
			printf ("GL Stack Overflow\n");
			return;
		case GL_STACK_UNDERFLOW:
			printf ("GL Stack Underflow\n");
			return;
		case GL_TABLE_TOO_LARGE:
			printf ("GL Table Too Large\n");
			return;
		default:
			printf("GL Undefined Error\n");
			return;
	};
}
//--------------------------------------------------------------------------
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
//--------------------------------------------------------------------------
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
//--------------------------------------------------------------------------
void DestroyIndexObject(INDEX_OBJ *indexObject)
{
	if(indexObject && indexObject->object!=INVALID_OBJECT_VALUE){
		glDeleteBuffers(1, &(indexObject->object));
	}
}
//--------------------------------------------------------------------------
/**
	@remarks:
		resolve the element mask user give, and fill them in elementInfo 
	@return:   
		the size in bytes of all elements
*/
int  _ResolveElement(struct ElementInfo *elementInfo, int elementMask)
{
	int dataOffset = 0, stride ;
	
	elementInfo->coordOffset = 0;
	switch(elementMask&COORD_MASK){
		case COORD_3:
			dataOffset += 3;
			elementInfo->coordNum = 3;
			break;
		case COORD_4:
			dataOffset += 4;
			elementInfo->coordNum= 4;
			break;
		default:
			elementInfo->coordNum = 0;
			break;
	}
	
	elementInfo->colorOffset = dataOffset * sizeof(float);
	switch(elementMask&COLOR_MASK){
		case COLOR_3:
			dataOffset+=3;
			elementInfo->colorNum = 3;
			break;
		case COLOR_4:
			dataOffset+=4;
			elementInfo->colorNum = 4;
			break;
		default:
			elementInfo->colorNum = 0;
			break;
	}
	
	elementInfo->color2Offset = dataOffset * sizeof(float);
	switch(elementMask&COLOR2_MASK){
		case COLOR2_3:
			dataOffset+=3;
			elementInfo->color2Num = 3;
			break;
		case COLOR2_4:
			dataOffset+=4;
			elementInfo->color2Num = 4;
			break;
		default:
			elementInfo->color2Num = 0;
			break;
	}

	elementInfo->normalOffset = dataOffset * sizeof(float);
	switch(elementMask&NORMAL_MASK){
		case NORMAL_3:
			dataOffset+=3;
			elementInfo->normalNum = 3;
			break;
		default:
			elementInfo->normalNum = 0;
			break;
	}

	elementInfo->textureCoordOffset = dataOffset*sizeof(float);
	switch(elementMask&TEXTURE_MASK){
		case TEXTURE_2:
			dataOffset+=2;
			elementInfo->textureCoordNum = 2;
			break;
		case TEXTURE_3:
			dataOffset+=3;
			elementInfo->textureCoordNum = 3;
			break;
		default:
			elementInfo->textureCoordNum = 0;
			break;
	}
	
	elementInfo->textureCoord2Offset = dataOffset*sizeof(float);
	switch(elementMask&TEXTURE2_MASK){
		case TEXTURE2_2:
			dataOffset+=2;
			elementInfo->textureCoord2Num = 2;
			break;
		case TEXTURE2_3:
			dataOffset+=3;
			elementInfo->textureCoord2Num = 3;
			break;
		default:
			elementInfo->textureCoord2Num = 0;
			break;
	}
	elementInfo->boneOffset = dataOffset*sizeof(float);
	if((elementMask&BONE_MASK) == BONE_4){
		elementInfo->boneSize  = 1;
		stride = (dataOffset+4) * sizeof(float) + 4*sizeof(float);//boneid是float类型的
	}else{
		elementInfo->boneSize  = 0;
		stride = dataOffset*sizeof(float);
	}
	return stride;
}
//--------------------------------------------------------------------------
/**
	@remarks: create a VertexOject using the given elements and number of vertices
	@return value: NULL for memery error
*/
VERTEX_OBJ* CreateVertexObject(int elements,  int n)
{
	int stride;
	struct VertexObj *obj = (struct VertexObj *)malloc(sizeof(struct VertexObj));
	
	if(obj!=NULL){
		obj->numVertex = n;
		obj->dataType = GL_FLOAT;
		stride = _ResolveElement(&(obj->elementInfo), elements);
		obj->stride = stride;
		obj->replaceObject = INVALID_OBJECT_VALUE;
		obj->object = INVALID_OBJECT_VALUE;
		obj->meshData.meshData = malloc(stride * n);
		obj->meshData.pushedNum = -1;
		if(obj->meshData.meshData== NULL){
			fprintf(stderr, "CreateVertexObject : memery allocation error\n");
			free(obj);
			return NULL;
		}
		return obj;
	}
	else{
		fprintf(stderr, "CreateVertexObject : memory allocation error\n");
		return NULL;
	}
}
//-----------------------------------------------------------------------------
/**
	@remarks : 
		check if vobj is a valid VertexObject
	@return value:
		1 for success
		0 for failure
*/
int _CheckVertexObject(VERTEX_OBJ *vobj)
{
	if(vobj == NULL || vobj->meshData.meshData == NULL){
		return 0;
	}
	return 1;
}
//-----------------------------------------------------------------------------
void _GetElementSizeOffset(VERTEX_OBJ *vobj, int element, int *offset, int *size)
{
	switch(element){
		case COORD_3:
			*offset = vobj->elementInfo.coordOffset;
			*size = 3 * sizeof(float);
			break;
		case COORD_4:
			*offset = vobj->elementInfo.coordOffset;
			*size = 4 * sizeof(float);
			break;
		case COLOR_3:
			*offset = vobj->elementInfo.colorOffset;
			*size = 3 * sizeof(float);
			break;
		case COLOR_4:
			*offset = vobj->elementInfo.colorOffset;
			*size = 4 * sizeof(float);
			break;
		case COLOR2_3:
			*offset = vobj->elementInfo.color2Offset;
			*size = 3 * sizeof(float);
			break;
		case COLOR2_4:
			*offset = vobj->elementInfo.color2Offset;
			*size = 4 * sizeof(float);
			break;
		case NORMAL_3:
			*offset = vobj->elementInfo.normalOffset;
			*size = 3 * sizeof(float);
			break;
		case TEXTURE_2:
			*offset = vobj->elementInfo.textureCoordOffset;
			*size = 2 * sizeof(float);
			break;
		case TEXTURE_3:
			*offset = vobj->elementInfo.textureCoordOffset;
			*size = 3 * sizeof(float);
			break;
		case TEXTURE2_2:
			*offset = vobj->elementInfo.textureCoord2Offset;
			*size = 2 * sizeof(float);
			break;
		case TEXTURE2_3:
			*offset = vobj->elementInfo.textureCoord2Offset;
			*size = 3 * sizeof(float);
			break;
		case BONE_4:
			*offset = vobj->elementInfo.boneOffset;
			*size = 4*(sizeof(float) + sizeof(float));
			break;
	}
	return;
}
//-----------------------------------------------------------------------------
void VertexObjectPushElement(VERTEX_OBJ *vobj, int element, float *p)
{
	void *destPos;
	int offset,size;
	if(_CheckVertexObject(vobj)==0){
		fprintf(stderr, "VertexObjectPushElement : Not a valid VERTEX_OBJ\n");
		return;
	}
	_GetElementSizeOffset(vobj, element,&offset,&size);
	if(element&COORD_MASK)
		++vobj->meshData.pushedNum;
	destPos = (char *)vobj->meshData.meshData + vobj->stride * vobj->meshData.pushedNum + offset;
	memcpy(destPos , p, size);
	return;
}
//---------------------------------------------------------------------------------------
void VertexObjectPushElementAll(VERTEX_OBJ *vobj, int element, float *p)
{
	void *destPos,*srcPos;
	int offset, size, i;
	if(_CheckVertexObject(vobj)==0){
		fprintf(stderr, "VertexObjectPushElement : Not a valid VERTEX_OBJ\n");
		return;
	}
	_GetElementSizeOffset(vobj, element, &offset, &size);
	for(i=0; i<vobj->numVertex; ++i){
		destPos = (char *)vobj->meshData.meshData + vobj->stride * i + offset;
		srcPos = (char *)p + size * i;
		memcpy(destPos, srcPos, size);
	}
	return;
}
//---------------------------------------------------------------------------------------
void VertexObjectEnd(VERTEX_OBJ *vobj)
{
	if(_CheckVertexObject(vobj)==0){
		fprintf(stderr, "VertexObjectPushElement : Not a valid VERTEX_OBJ\n");
		return;
	}
	glGenBuffers(1, &(vobj->object));
	glBindBuffer(GL_ARRAY_BUFFER, vobj->object);	
	glBufferData(GL_ARRAY_BUFFER, vobj->numVertex*vobj->stride,  vobj->meshData.meshData, GL_DYNAMIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, vobj->numVertex*vobj->stride,  vobj->meshData.meshData, GL_STATIC_DRAW);
	free(vobj->meshData.meshData);
	vobj->meshData.meshData = NULL;
	vobj->meshData.pushedNum = 0;
	return;
}
//-----------------------------------------------------------------------------
/*
	@remarks:
		replace the vertex coord & normal with a new buffer object
*/
void VertexObjectReplace(VERTEX_OBJ *vobj, const float *p)
{
	if(vobj){
		glGenBuffers(1, &(vobj->replaceObject));
		glBindBuffer(GL_ARRAY_BUFFER, vobj->replaceObject);
		glBufferData(GL_ARRAY_BUFFER, 
			vobj->numVertex * (vobj->elementInfo.coordNum + vobj->elementInfo.normalNum) * sizeof(float),
			p, GL_DYNAMIC_DRAW);
	}else{
		fprintf(stderr, "VertexObjectReplace : Not a valid VERTEX_OBJ\n");
	}
	return;
}
//-----------------------------------------------------------------------------
void VertexObjectReset(VERTEX_OBJ *vobj)
{
	if(vobj){
		if(vobj->replaceObject != INVALID_OBJECT_VALUE)
			glDeleteBuffers(1, &(vobj->replaceObject));			
	}else{
		fprintf(stderr, "VertexObjectReset : Not a valid VERTEX_OBJ\n");
	}
	return;
}
//-----------------------------------------------------------------------------
void DestroyVertexObject(VERTEX_OBJ *vertexObject)
{
	if(vertexObject ){
		if(vertexObject->object != INVALID_OBJECT_VALUE)
			glDeleteBuffers(1, &(vertexObject->object));
		if(vertexObject->object != INVALID_OBJECT_VALUE)
			glDeleteBuffers(1, &(vertexObject->replaceObject));
		if(vertexObject->meshData.meshData != NULL)
			free(vertexObject->meshData.meshData);
	}else{
		fprintf(stderr, "DestroyVertexObject : Not a valid VERTEX_OBJ\n");
	}
	return;
}
//-----------------------------------------------------------------------------
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
	//texture 
	if(pixelObj){
		glEnable(pixelObj->textureTarget);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glActiveTexture(pixelObj->textureUnit);
		glBindTexture(pixelObj->textureTarget,pixelObj->object);
	}
	//vertex buffer
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertexObj->object);
	glVertexPointer(vertexObj->elementInfo.coordNum, vertexObj->dataType,vertexObj->stride, (const GLvoid *)vertexObj->elementInfo.coordOffset);
	if(vertexObj->elementInfo.colorNum > 0){
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(vertexObj->elementInfo.colorNum, vertexObj->dataType, vertexObj->stride, (const GLvoid *)vertexObj->elementInfo.colorOffset);
	}
	if(vertexObj->elementInfo.color2Num > 0){
		glEnableClientState(GL_SECONDARY_COLOR_ARRAY);
		glSecondaryColorPointer(vertexObj->elementInfo.color2Num, vertexObj->dataType, vertexObj->stride, (const GLvoid *)vertexObj->elementInfo.color2Offset);
	}
	if(vertexObj->elementInfo.normalNum > 0){
		glEnableClientState(GL_NORMAL_ARRAY); 
		glNormalPointer(vertexObj->dataType, vertexObj->stride, (const GLvoid * )vertexObj->elementInfo.normalOffset);
	}
	if(vertexObj->elementInfo.textureCoordNum > 0){
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(vertexObj->elementInfo.textureCoordNum, vertexObj->dataType, vertexObj->stride, (const GLvoid *)vertexObj->elementInfo.textureCoordOffset);
	}
	//index buffer
	glEnableClientState(GL_INDEX_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexObj->object);

	//Rendering action
	glDrawElements(GL_TRIANGLES, indexObj->n,indexObj->dataType,0);
	
	if(pixelObj){
		glDisable(pixelObj->textureTarget);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	if(vertexObj->elementInfo.colorNum > 0){
		glDisableClientState(GL_COLOR_ARRAY);
	}
	if(vertexObj->elementInfo.normalNum > 0){
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	if(vertexObj->elementInfo.textureCoordNum > 0){
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}	
	return ;
}
void DrawOjectUseShader(const INDEX_OBJ *indexObj, const VERTEX_OBJ *vertexObj, const PIXEL_OBJ *pixelObj)
{
	glEnableVertexAttribArray(g_vertexPostionLocation);
	glEnableVertexAttribArray(g_vertexTexCoordLocation);
	glEnableVertexAttribArray(g_vertexBoneIdLocation);
	glEnableVertexAttribArray(g_vertexBoneWeightLocation);
	glBindBuffer(GL_ARRAY_BUFFER, vertexObj->object);
	glVertexAttribPointer(g_vertexPostionLocation, vertexObj->elementInfo.coordNum, 
		vertexObj->dataType, GL_FALSE, vertexObj->stride, (const GLvoid *)vertexObj->elementInfo.coordOffset);
	glVertexAttribPointer(g_vertexTexCoordLocation, vertexObj->elementInfo.textureCoordNum, 
		vertexObj->dataType, GL_FALSE, vertexObj->stride, (const GLvoid *)vertexObj->elementInfo.textureCoordOffset);
	//glVertexAttribIPointer(g_vertexBoneIdLocation, 4, GL_INT, vertexObj->stride, (const GLvoid *)vertexObj->elementInfo.boneOffset);
	glVertexAttribPointer(g_vertexBoneIdLocation, 4, GL_FLOAT, GL_FALSE, vertexObj->stride, (const GLvoid *)(vertexObj->elementInfo.boneOffset));
	glVertexAttribPointer(g_vertexBoneWeightLocation, 4, GL_FLOAT, GL_FALSE, vertexObj->stride, (const GLvoid *)(vertexObj->elementInfo.boneOffset + 4*sizeof(float)));

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
void SetTranslateMatrix(const GLuint location, const Matrix4f * transMatrix)
{
	glUniformMatrix4fv(location, 1, GL_TRUE, &(transMatrix->m[0][0]));
	return;
}
void SetIntValue(const GLuint location, GLint value)
{
	glUniform1i(location, value);
	return;
}
bool InitGlew(void)
{
	//init glew
	GLenum res = glewInit();
	if(res != GLEW_OK){
		fprintf(stderr, "InitGlew: glew init failure\n");
		return false;
	}
	return true;
}
bool InitRenderState(void)
{	
	//init state
	glClearColor(0,0,0,0);
	glEnable(GL_DEPTH_TEST);	
	return true;
}
void ClearBuffer(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
