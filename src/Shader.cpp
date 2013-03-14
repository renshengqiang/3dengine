#include "Shader.h"
#include "util.h"
#include <string.h>
#include <stdio.h>

GLuint g_vertexPostionLocation;
GLuint g_vertexTexCoordLocation;
GLuint g_vertexBoneIdLocation;
GLuint g_vertexBoneWeightLocation;
GLuint g_hasBonesLocation;
GLuint g_PVMMatrixLocation;
GLuint g_samplerLocation;
GLuint g_boneTransformLocation[MAX_BONE_NUM];

GLuint shader_handler;

static const char *pVS = "											\n\
#version 120														\n\
const int MAX_BONE_NUM = 100;										\n\
																\n\
attribute vec3 vertexPosition;											\n\
attribute vec2 vertexTexCoord;										\n\
attribute vec4 boneIds;												\n\
attribute vec4  weights;												\n\
uniform  bool hasBones;												\n\
																\n\
uniform mat4 PVMMatrix;												\n\
uniform mat4 boneTransform[MAX_BONE_NUM];							\n\
																\n\
varying vec2 texCoord;												\n\
void main()														\n\
{																\n\
	mat4 boneTrans = mat4(1.0);										\n\
	if(hasBones){													\n\
		boneTrans = boneTransform[int(boneIds[0])] * weights[0];		\n\
		boneTrans += boneTransform[int(boneIds[1])] * weights[1];		\n\
		boneTrans += boneTransform[int(boneIds[2])] * weights[2];		\n\
		boneTrans += boneTransform[int(boneIds[3])] * weights[3];		\n\
	}															\n\
																\n\
	gl_Position = PVMMatrix*boneTrans*vec4(vertexPosition, 1.0f);			\n\
	texCoord = vertexTexCoord;										\n\
}";
static const char *pFS = "											\n\
#version 120														\n\
																\n\
varying vec2 texCoord;												\n\
uniform sampler2D sampler;											\n\
																\n\
void main()														\n\
{																\n\
	gl_FragColor = texture2D(sampler, texCoord.st);						\n\
}";
//-----------------------------------------------------------------------
const char* _shader_type_2_string(int type)
{
	if(type == GL_VERTEX_SHADER)
		return "Vertex shader";
	else if(type == GL_FRAGMENT_SHADER)
		return "Fragment shader";
	else
		return "Unknown";
}
//-----------------------------------------------------------------------
static bool AddShader(GLuint shader_handler, const char *shader_text, GLenum shader_type) 
{
	GLuint shader_obj = glCreateShader(shader_type);
	const GLchar *text_array[1];
	GLint length_array[1];
	
	if(shader_obj == 0){
		fprintf(stderr, "AddShader: create shader fail\n");
		return false;
	}
	text_array[0] = shader_text;
	length_array[0] = strlen(shader_text);
	glShaderSource(shader_obj, 1, text_array, length_array);
	glCompileShader(shader_obj);
	GLint success;
	glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);
	if(!success){
		GLchar log[1024];
		glGetShaderInfoLog(shader_obj, 1024, NULL, log);
		fprintf(stderr, "AddShader: error compile shader type %s : '%s'\n",_shader_type_2_string(shader_type), log);
		return false;
	}
	glAttachShader(shader_handler, shader_obj);
	return true;
}
//-----------------------------------------------------------------------
bool CreateShaders()
{
	shader_handler = glCreateProgram();
	if(shader_handler == 0){
		fprintf(stderr, "CreateShaders: create program fail\n");
		return false;
	}
	if(AddShader(shader_handler,pVS, GL_VERTEX_SHADER) == false)
		return false;
	if(AddShader(shader_handler, pFS, GL_FRAGMENT_SHADER) == false)
		return false;

	GLint success = 0;
	GLchar log[1024]={0};
	glLinkProgram(shader_handler);
	glGetProgramiv(shader_handler, GL_LINK_STATUS, &success);
	if(success == 0){
		glGetProgramInfoLog(shader_handler, sizeof(log), NULL, log);
		fprintf(stderr, "CreateShaders: error link program: '%s'\n", log);
		return false;
	}
	glValidateProgram(shader_handler);
	glGetProgramiv(shader_handler, GL_VALIDATE_STATUS, &success);
	if(!success){
		glGetProgramInfoLog(shader_handler, sizeof(log), NULL, log);
		fprintf(stderr, "CreateShaders: error validate shader program: '%s'\n", log);
		return false;
	}
	glUseProgram(shader_handler);

	g_vertexPostionLocation = glGetAttribLocation(shader_handler, "vertexPosition");
	g_vertexTexCoordLocation = glGetAttribLocation(shader_handler, "vertexTexCoord");
	g_vertexBoneIdLocation = glGetAttribLocation(shader_handler, "boneIds");
	g_vertexBoneWeightLocation = glGetAttribLocation(shader_handler, "weights");
	g_hasBonesLocation = glGetUniformLocation(shader_handler, "hasBones");
	g_PVMMatrixLocation = glGetUniformLocation(shader_handler, "PVMMatrix");
	g_samplerLocation = glGetUniformLocation(shader_handler, "sampler");
	if( g_vertexPostionLocation == INVALID_HANDLER || 
		g_vertexTexCoordLocation == INVALID_HANDLER ||
		g_vertexBoneIdLocation == INVALID_OBJECT_VALUE ||
		g_vertexBoneWeightLocation == INVALID_OBJECT_VALUE ||
		g_hasBonesLocation == INVALID_HANDLER ||
		g_PVMMatrixLocation == INVALID_HANDLER||
		g_samplerLocation == INVALID_HANDLER) {
		return false;
	}
	for(unsigned int i=0; i < MAX_BONE_NUM; ++i){
		char name[128];
		memset(name, 0, sizeof(name));
		snprintf(name, sizeof(name), "boneTransform[%d]", i);
		g_boneTransformLocation[i] = glGetUniformLocation(shader_handler, name);
	}
	//printf("Position:%d\n", glGetAttribLocation(shader_handler, "Position"));
	return true;
}
void UseShaderToRender(void)
{
	glUseProgram(shader_handler);
}
void UseFixedPipeline(void)
{
	glUseProgram(0);
}
