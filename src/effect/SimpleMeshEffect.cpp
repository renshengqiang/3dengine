#include "SimpleMeshEffect.h"
#include <string.h>//memset

SimpleMeshEffect::SimpleMeshEffect(const char *vertexShaderFile, const char *fragmentShaderFile)
{
	m_vertexShaderFile = vertexShaderFile;
	m_fragmentShaderFile = fragmentShaderFile;
}
SimpleMeshEffect::~SimpleMeshEffect()
{
}
bool SimpleMeshEffect::Init()
{
	if(!Effect::Init()) return false;

	if(!AddShader(VERTEX_SHADER, m_vertexShaderFile.c_str())) return false;

	if(!AddShader(FRAGMENT_SHADER, m_fragmentShaderFile.c_str())) return false;

	if(!Finalize()) return false;

	m_positionLocation = GetAttribLocation("vertexPosition");
	m_textureCoordLocation = GetAttribLocation("vertexTexCoord");
	m_boneIdLocation = GetAttribLocation("boneIds");
	m_boneWeightLocation = GetAttribLocation("weights");
	
	m_hasBonesLocation = GetUniformLocation("hasBones");
	m_WVPLocation = GetUniformLocation("PVMMatrix");
	m_sampleLocation = GetUniformLocation("sampler");
	if(m_positionLocation == INVALID_LOCATION ||
		m_textureCoordLocation == INVALID_LOCATION ||
		m_boneIdLocation == INVALID_LOCATION ||
		m_boneWeightLocation == INVALID_LOCATION ||
		m_hasBonesLocation == INVALID_LOCATION || 
		m_WVPLocation == INVALID_LOCATION ||
		m_sampleLocation == INVALID_LOCATION)
		return false;

	for(unsigned int i=0; i < MAX_BONE_NUM; ++i){
		char name[128];
		memset(name, 0, sizeof(name));
		snprintf(name, sizeof(name), "boneTransform[%d]", i);
		m_boneTransMatrixLocation[i] = GetUniformLocation(name);
		if(m_boneTransMatrixLocation[i] == INVALID_LOCATION)
		{
			return false;
		}
	}
	return true;
}

void SimpleMeshEffect::EnablePositionAttrib()
{
	glEnableVertexAttribArray(m_positionLocation);
}
void SimpleMeshEffect::EnableTextureCoordAtrrib()
{
	glEnableVertexAttribArray(m_textureCoordLocation);
}
void SimpleMeshEffect::EnableBoneIdAttrib()
{
	glEnableVertexAttribArray(m_boneIdLocation);
}
void SimpleMeshEffect::EnableBoneWeightAttrib()
{
	glEnableVertexAttribArray(m_boneWeightLocation);
}
void SimpleMeshEffect::EnableVertexArray()
{
	EnablePositionAttrib();
	EnableTextureCoordAtrrib();
	EnableBoneIdAttrib();
	EnableBoneWeightAttrib();
}
void SimpleMeshEffect::DisableVertexArray()
{
	glDisableVertexAttribArray(m_positionLocation);
	glDisableVertexAttribArray(m_textureCoordLocation);
	glDisableVertexAttribArray(m_boneIdLocation);
	glDisableVertexAttribArray(m_boneWeightLocation);
}
void SimpleMeshEffect::SetWVP(const Matrix4f& WVP)
{
	glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, &(WVP.m[0][0]));
}
void SimpleMeshEffect::SetTextureUnit(int textureUnit)
{
	glUniform1i(m_sampleLocation, textureUnit);
}
void SimpleMeshEffect::SetHasBones(bool hasBone)
{
	glUniform1i(m_hasBonesLocation, hasBone);
}
void SimpleMeshEffect::SetBoneTransMatrix(const Matrix4f& trans, unsigned boneId)
{
	glUniformMatrix4fv(m_boneTransMatrixLocation[boneId], 1, GL_TRUE, &(trans.m[0][0]));
}
void SimpleMeshEffect::SetPositionAttribPointer(int coordNum, int stride, int offset)
{
	glVertexAttribPointer(m_positionLocation, coordNum, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)offset);
}
void SimpleMeshEffect::SetTextureCoordAttribPointer(int coordNum, int stride, int offset)
{
	glVertexAttribPointer(m_textureCoordLocation, coordNum, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)offset);
}
void SimpleMeshEffect::SetBoneIdAttribPointer(int coordNum, int stride, int offset)
{
	glVertexAttribPointer(m_boneIdLocation, coordNum, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)offset);
}
void SimpleMeshEffect::SetBoneWeightAttribPointer(int coordNum, int stride, int offset)
{
	glVertexAttribPointer(m_boneWeightLocation, coordNum, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)offset);
}
