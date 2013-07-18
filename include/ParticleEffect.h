#ifndef _PARTICLE_EFFECT_H
#define _PARTICLE_EFFECT_H
#include <Effect.h>
#include <math_3d.h>
#include <string>

class ParticleEffect : public Effect
{
public:
	ParticleEffect(const char *vertexShaderFile, const char *fragmentShaderFile);
	~ParticleEffect();

	virtual bool Init(void);
	
	void EnablePositionAttrib(void);
	void EnableVelocityAttrib(void);
	void EnableTextureCoordAtrrib(void);
	void EnableVertexArray(void);
	void DisableVertexArray(void);
	
	void SetWVP(const Matrix4f& WVP);
	void SetTextureUnit(int textureUnit);
	void SetTime(float time);
	
	void SetPositionAttribPointer(int coordNum, int stride, int offset);
	void SetVelocityAttribPointer(int coordNum, int stride, int offset);
	void SetTextureCoordAttribPointer(int coordNum, int stride, int offset);	
	
private:
	std::string m_vertexShaderFile, m_fragmentShaderFile;
	GLuint m_positionLocation;
	GLuint m_velocityLocation;
	GLuint m_textureCoordLocation;
	GLuint m_timeLocation;
	GLuint m_WVPLocation;
	GLuint m_sampleLocation;
};
#endif
