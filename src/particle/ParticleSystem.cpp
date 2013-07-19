#include "ParticleSystem.h"
#include "ParticleEffect.h"

#define PARTICLE_SIZE 100
#define MAX_LIVE_TIME 10

float _GetRandomMinMax(float fMin, float fMax)
{
	float fRandNum = (float)rand() /RAND_MAX;
	return fMin + (fMax - fMin) * fRandNum;
}
Vector3f _GetRandomVector(void)
{
	Vector3f velcity;

	//// Pick a random Z between -1.0f and 1.0f
	velcity.z = _GetRandomMinMax(-1.0f, 1.0f);

	//// Get radius of this circle
	float radius = (float)sqrt(1 - velcity.z * velcity.z);

	//// Pick a random point on a circle
	float t = _GetRandomMinMax(-M_PI, M_PI);

	//// compute matching X and Y
	velcity.x = 10*(float)cosf(t) * radius;
	velcity.y = 10*(float)sinf(t)* radius;
	velcity.z = 100.0f;

	return velcity;
}


ParticleSystem::ParticleSystem():
	m_time(0),
	m_maxParticles(PARTICLE_SIZE),
	m_finalized(false)
{
	m_posVec.resize(4*m_maxParticles);
	m_velocityVec.resize(4*m_maxParticles);
	m_texCoordVec.resize(4*m_maxParticles);
	m_indexVec.resize(6 * m_maxParticles);
	
	for(int i=0; i<m_maxParticles; ++i)
	{
		Vector3f velocity = _GetRandomVector();
		m_posVec[4*i+0] = Vector3f(-1, -1, -1000);/*100(m/s) * 10(s)*/
		m_posVec[4*i+1] = Vector3f(1, -1, -1000);
		m_posVec[4*i+2] = Vector3f(1, 1, -1000);
		m_posVec[4*i+3] = Vector3f(-1, 1, -1000);
		
		m_texCoordVec[4*i+0] = Vector2f(0, 0);
		m_texCoordVec[4*i +1] = Vector2f(1, 0);
		m_texCoordVec[4*i +2] = Vector2f(1, 1);
		m_texCoordVec[4*i +3] = Vector2f(0, 1);
		
		for(int j=0; j<4; ++j)
		{
			m_velocityVec[4*i+j] = velocity;
		}

		printf("velocity (%f %f %f)\n", velocity.x, velocity.y, velocity.z);
		m_indexVec[6*i +0] = 4*i +0;
		m_indexVec[6*i +1] = 4*i +1;
		m_indexVec[6*i +2] = 4*i +2;
		m_indexVec[6*i +3] = 4*i +0;
		m_indexVec[6*i +4] = 4*i +2;
		m_indexVec[6*i +5] = 4*i +3;
	}
}
void ParticleSystem::AddTime(float seconds)
{
	m_time += seconds;
	if(m_time > MAX_LIVE_TIME)
		m_time -= MAX_LIVE_TIME;
}
float ParticleSystem::GetTime(void)
{
	return m_time;
}
void ParticleSystem::SetTexture(const char *filename)
{
	mp_texture = new Texture(GL_TEXTURE_2D, filename);
}
void ParticleSystem::Render(Effect * effect)
{
	ParticleEffect *pParticleEffect = static_cast<ParticleEffect*>(effect);
	
	if(!m_finalized)
	{
		Finalize();
		m_finalized = true;
	}

	pParticleEffect->SetTime(m_time);
	DrawParticle(indexObject, vertexObject, pixelObject, *pParticleEffect);
}
void ParticleSystem::Finalize(void)
{
	vertexObject = CreateVertexObject(COORD_3|COLOR_3|TEXTURE_2, 4*m_maxParticles);	
	VertexObjectPushElementAll(vertexObject, COORD_3, (float*)&m_posVec[0]);
	VertexObjectPushElementAll(vertexObject, COLOR_3, (float*)&m_velocityVec[0]);
	VertexObjectPushElementAll(vertexObject, TEXTURE_2, (float *)&m_texCoordVec[0]);
	VertexObjectEnd(vertexObject);
	
	indexObject = CreateIndexObject(6*m_maxParticles, (unsigned int *)&m_indexVec[0]);
	mp_texture->Load();
	pixelObject = CreatePixelObject2(mp_texture->GetTextureObj(),GL_TEXTURE0,GL_TEXTURE_2D);
}
