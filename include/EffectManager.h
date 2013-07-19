#ifndef _EFFECT_MANAGER_H
#define _EFFECT_MANAGER_H

#include <map>
#include <Effect.h>

class EffectManager
{
public:
	enum EFFECT_TYPE
	{
		MESH_EFFECT,
		PARTICAL_EFFECT
	};
	typedef std::map<EFFECT_TYPE, Effect*> EffectMap;
	typedef EffectMap::iterator EffectMapIterator;
	
	EffectManager();
	void AddEffect(EFFECT_TYPE type, const char *vsFile, const char *fsFile);
	Effect* GetEffect(EFFECT_TYPE type);
	void Finalize(void);

	static void FFPRendering();
private:
	EffectMap m_effectMap;
};


#endif
