#include "ParticleEffect.h"
#include <string.h>//memset

ParticleEffect::ParticleEffect(const char *vertexShaderFile, const char *fragmentShaderFile)
{
	m_vertexShaderFile = vertexShaderFile;
	m_fragmentShaderFile = fragmentShaderFile;
}
ParticleEffect::~ParticleEffect()
{
}
bool ParticleEffect::Init()
{
	if(!Effect::Init()) return false;

	if(!AddShader(VERTEX_SHADER, m_vertexShaderFile.c_str())) return false;

	if(!AddShader(FRAGMENT_SHADER, m_fragmentShaderFile.c_str())) return false;

	if(!Finalize()) return false;

	m_positionLocation = GetAttribLocation("pos");
	m_textureCoordLocation = GetAttribLocation("texCoord");
	m_velocityLocation = GetAttribLocation("velocity");
	m_timeLocation = GetUniformLocation("time");
	m_WVPLocation = GetUniformLocation("wvpMatrix");
	m_sampleLocation = GetUniformLocation("sampler");
	if(m_positionLocation == INVALID_LOCATION ||
		m_textureCoordLocation == INVALID_LOCATION ||
		m_velocityLocation== INVALID_LOCATION ||
		m_timeLocation== INVALID_LOCATION ||
		m_WVPLocation == INVALID_LOCATION ||
		m_sampleLocation == INVALID_LOCATION)
		return false;
	return true;
}

void ParticleEffect::EnablePositionAttrib()
{
	glEnableVertexAttribArray(m_positionLocation);
}
void ParticleEffect::EnableTextureCoordAtrrib()
{
	glEnableVertexAttribArray(m_textureCoordLocation);
}
void ParticleEffect::EnableVelocityAttrib()
{
	glEnableVertexAttribArray(m_velocityLocation);
}
void ParticleEffect::EnableVertexArray()
{
	EnablePositionAttrib();
	EnableTextureCoordAtrrib();
	EnableVelocityAttrib();
}
void ParticleEffect::DisableVertexArray()
{
	glDisableVertexAttribArray(m_positionLocation);
	glDisableVertexAttribArray(m_textureCoordLocation);
	glDisableVertexAttribArray(m_velocityLocation);
}
void ParticleEffect::SetWVP(const Matrix4f& WVP)
{
	glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, &(WVP.m[0][0]));
}
void ParticleEffect::SetTextureUnit(int textureUnit)
{
	glUniform1i(m_sampleLocation, textureUnit);
}
void ParticleEffect::SetTime(float time)
{
	glUniform1f(m_timeLocation, time);
}
void ParticleEffect::SetPositionAttribPointer(int coordNum, int stride, int offset)
{
	glVertexAttribPointer(m_positionLocation, coordNum, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)offset);
}
void ParticleEffect::SetTextureCoordAttribPointer(int coordNum, int stride, int offset)
{
	glVertexAttribPointer(m_textureCoordLocation, coordNum, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)offset);
}
void ParticleEffect::SetVelocityAttribPointer(int coordNum, int stride, int offset)
{
	glVertexAttribPointer(m_velocityLocation, coordNum, GL_FLOAT, GL_FALSE, stride, (const GLvoid *)offset);
}
