#include "EffectManager.h"
#include "SimpleMeshEffect.h"
#include "ParticleEffect.h"
#include <stdio.h>

EffectManager::EffectManager()
{
}
void EffectManager::AddEffect(EFFECT_TYPE type,const char * vsFile,const char * fsFile)
{
	Effect *pEffect = NULL;

	if(m_effectMap.count(type) >0)
	{
		fprintf(stderr, "EffectManager::AddEffect: effect type already exists, check\n");
		return;
	}
	switch(type)
	{
		case MESH_EFFECT:
			pEffect = new SimpleMeshEffect(vsFile, fsFile);
			break;
		case PARTICAL_EFFECT:
			pEffect = new ParticleEffect(vsFile, fsFile);
			break;
		default:
			fprintf(stderr, "EffectManager::AddEffect: Invalid Effect type\n");
			break;
	}
	m_effectMap.insert(std::make_pair(type, pEffect));
}
Effect* EffectManager::GetEffect(EFFECT_TYPE type)
{
	EffectMapIterator it = m_effectMap.find(type);

	if(it != m_effectMap.end()) return it->second;
	else return NULL;
}
void EffectManager::Finalize(void)
{
	for(EffectMapIterator it = m_effectMap.begin(); it != m_effectMap.end(); ++it)
	{
		it->second->Init();
	}
}
void EffectManager::FFPRendering(void)
{
	Effect::Disable();
}