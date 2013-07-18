#ifndef _PARTICLE_SYSTEM_H
#define _PARTICLE_SYSTEM_H
#include <MoveableObject.h>
#include <Renderable.h>
#include <Texture.h>
#include <Render.h>
#include <vector>

class ParticleSystem : public MoveableObject, public Renderable
{
public:
	ParticleSystem();

	float GetTime(void);
	void AddTime(float seconds);
	void SetTexture(const char *filename);
	virtual const AxisAlignedBox& GetBoundingBox(void) const { return m_alignedBox;}
	virtual void Render(Effect *effect);

protected:
	void Finalize();

private:
	std::vector<Vector3f> m_posVec;
	std::vector<Vector3f> m_velocityVec;
	std::vector<Vector2f> m_texCoordVec;
	std::vector<int> m_indexVec;
	Texture *mp_texture;
	
	float m_time;
	int m_maxParticles;
	AxisAlignedBox m_alignedBox;

	VERTEX_OBJ *vertexObject;
	INDEX_OBJ *indexObject;
	PIXEL_OBJ *pixelObject;
	bool m_finalized;
};

#endif
