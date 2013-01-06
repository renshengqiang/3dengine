#ifndef _SHADER_H
#define _SHADER_H
#include <GL/glew.h>

extern GLuint g_vertexPostionLocation;
extern GLuint g_vertexTexCoordLocation;
extern GLuint g_PVMatrixLocation;
extern GLuint g_MMatrixLocation;
extern GLuint g_samplerLocation;
bool CreateShaders();	
#endif
