#ifndef _SHADER_H
#define _SHADER_H
#include <GL/glew.h>

#define MAX_BONE_NUM 100
extern GLuint g_vertexPostionLocation;
extern GLuint g_vertexTexCoordLocation;
extern GLuint g_vertexBoneIdLocation;
extern GLuint g_vertexBoneWeightLocation;
extern GLuint g_hasBonesLocation;
extern GLuint g_PVMMatrixLocation;
extern GLuint g_samplerLocation;
extern GLuint g_boneTransformLocation[MAX_BONE_NUM];
bool CreateShaders();	
#endif
