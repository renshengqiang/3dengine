#include <Shader.h>
#include <string.h>
#include <stdio.h>
#define INVALID_HANDLER 0xffffffff

GLuint g_vertexPostionLocation;
GLuint g_vertexTexCoordLocation;
GLuint g_PVMMatrixLocation;
GLuint g_samplerLocation;
static const char *pVS = "											\n\
#version 130														\n\
																	\n\
in vec3 vertexPosition;												\n\
in vec2 vertexTexCoord;												\n\
																	\n\
uniform mat4 PVMatrix;												\n\
uniform mat4 MMatrix;												\n\
																	\n\
out vec2 texCoord;													\n\
void main()															\n\
{																	\n\
	gl_Position = PVMatrix*vec4(vertexPosition, 1.0f);				\n\
	texCoord=vertexTexCoord;										\n\
}";
static const char *pFS = "									\n\
#version 130												\n\
															\n\
in vec2 texCoord;											\n\
uniform sampler2D sampler;									\n\
															\n\
out vec4 FragColor;											\n\
void main()													\n\
{															\n\
	FragColor = texture2D(sampler, texCoord.st);			\n\
}";
const char* _shader_type_2_string(int type)
{
	if(type == GL_VERTEX_SHADER)
		return "Vertex shader";
	else if(type == GL_FRAGMENT_SHADER)
		return "Fragment shader";
	else
		return "Unknown";
}
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
bool CreateShaders()
{
	GLuint shader_handler = glCreateProgram();
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
	g_PVMMatrixLocation = glGetUniformLocation(shader_handler, "PVMatrix");
	g_samplerLocation = glGetUniformLocation(shader_handler, "sampler");
	if( g_vertexPostionLocation == INVALID_HANDLER || 
		g_vertexTexCoordLocation == INVALID_HANDLER ||
		g_PVMMatrixLocation == INVALID_HANDLER||
		g_samplerLocation == INVALID_HANDLER) return false;
	//printf("Position:%d\n", glGetAttribLocation(shader_handler, "Position"));
	return true;
}

