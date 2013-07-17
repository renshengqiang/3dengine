#ifndef _RENDERABLE_H
#define _RENDERABLE_H

#include <Effect.h>
class Renderable
{
public:
	virtual void Render(Effect *pEffect) = 0;
};

#endif