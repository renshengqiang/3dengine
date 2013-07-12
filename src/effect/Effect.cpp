#include "Effect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-----------------------------------------------------------------------
const char* _Shader_type_2_string(int type)
{
	if(type == GL_VERTEX_SHADER)
		return "Vertex shader";
	else if(type == GL_FRAGMENT_SHADER)
		return "Fragment shader";
	else
		return "Unknown";
}
//-----------------------------------------------------------------------
char *_ReadFile(const char *fileName)
{
	if(fileName == NULL)
	{
		fprintf(stderr, "_ReadFile: invalid file name\n");
		return NULL;
	}
	FILE *fp = fopen(fileName, "rb");
	if(fp == NULL)
	{
		fprintf(stderr, "_ReadFile: Error opening file %s, file not exist?\n", fileName);
		return NULL;
	}
	if(fseek(fp, 0, SEEK_END) == -1)
	{
		fprintf(stderr, "_ReadFile: Error seeking file %s\n", fileName);
		return NULL;
	}
	long fileSize = ftell(fp);
	if(fileSize == -1)
	{
		fprintf(stderr, "_ReadFile: Error reading file %s, file content invalid\n", fileName);
		return NULL;
	}
	if(fseek(fp, 0, SEEK_SET) == -1)
	{
		fprintf(stderr, "_ReadFile: Error seeking file %s\n", fileName);
		return NULL;
	}

	//allocate the file content into a string
	char *content = (char *)malloc((size_t)(fileSize + 1));
	fread(content, 1, (size_t)fileSize, fp);

	if(ferror(fp))
	{
		fprintf(stderr, "-ReadFile: Error reading file %s\n", fileName);
		free(content);
		return NULL;
	}
	fclose(fp);
	content[fileSize] = '\0';
	return content;
}
Effect::Effect()
{
	m_shaderProgram = 0;
}
Effect::~Effect()
{
	for(ShaderObjList::iterator iter = m_shaderObjList.begin(); iter != m_shaderObjList.end(); ++iter)
	{
		glDeleteShader(*iter);
	}
	if(m_shaderProgram != 0)
	{
		glDeleteProgram(m_shaderProgram);
	}
}
bool Effect::Init()
{
	m_shaderProgram = glCreateProgram();
	if(m_shaderProgram == 0)
	{
		fprintf(stderr, "Effect::Init: Error creating a shader glsl program\n");
		return false;
	}
	return true;
}
void Effect::Enable()
{
	glUseProgram(m_shaderProgram);
}
void Effect::Disable()
{
	glUseProgram(0);
}
bool Effect::AddShader(SHADER_TYPE shaderType, const char *fileName)
{
	char *source = _ReadFile(fileName);
	if(source == NULL)
	{
		fprintf(stderr, "Effect::AddShader: Error parsing shader file %s\n", fileName);
		return false;
	}
	
	GLenum type = shaderType == VERTEX_SHADER? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
	GLuint shaderObj = glCreateShader(type);
	if(shaderObj == 0)
	{
		fprintf(stderr, "Effect::AddShader: Error creating shader object\n");
		return false;
	}

	const GLchar *textArray[1];
	textArray[0] = source;
	GLint length[1];
	length[0] = strlen(source);
	glShaderSource(shaderObj, 1, textArray, length);

	glCompileShader(shaderObj);

	GLint success;
	glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
	if(success == 0)
	{
		GLchar log[1024];
		glGetShaderInfoLog(shaderObj, 1024, NULL, log);
		fprintf(stderr, "Effect::AddShader: Error compiling shader type %s : '%s'\n", _Shader_type_2_string(type), log);
		return false;
	}

	free(source);
	glAttachShader(m_shaderProgram, shaderObj);
	m_shaderObjList.push_back(shaderObj);
	return true;
}
bool Effect::Finalize()
{
	GLint success;
	GLchar log[1024];

	glLinkProgram(m_shaderProgram);
	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
	if(success == 0)
	{
		glGetProgramInfoLog(m_shaderProgram, 1024, NULL, log);
		fprintf(stderr, "Effect::Finalize: Error link glsl program: '%s'\n", log);
		return false;
	}

	glValidateProgram(m_shaderProgram);
	glGetProgramiv(m_shaderProgram, GL_VALIDATE_STATUS, &success);
	if(success == 0)
	{	
		glGetProgramInfoLog(m_shaderProgram, 1024, NULL, log);
		fprintf(stderr, "Effect::Finalize: Invalid shader program: '%s'\n", log);
		return false;
	}

	//delete the intermediate shader objects that have been added to the program
	for(ShaderObjList::iterator iter = m_shaderObjList.begin(); iter != m_shaderObjList.end(); ++iter)
	{	
		glDeleteShader(*iter);
	}
	m_shaderObjList.clear();

	return true;
}
GLint Effect::GetUniformLocation(const char *pUniformName)
{
	GLuint location = glGetUniformLocation(m_shaderProgram, pUniformName);

	if(location == INVALID_LOCATION)
	{
		fprintf(stderr, "Effect::GetUniformLocation: Error get uniform varivable %s location\n", pUniformName);
	}
	return location;
}
GLint Effect::GetAttribLocation(const char *pAttribName)
{
	GLuint location = glGetAttribLocation(m_shaderProgram, pAttribName);

	if(location == INVALID_LOCATION)
	{
		fprintf(stderr, "Effect::GetUniformLocation: Error get uniform varivable %s location\n", pAttribName);
	}
	return location;
}