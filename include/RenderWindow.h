#ifndef _RENDER_WINDOW_H
#define _RENDER_WINDOW_H
#include "Export.h"

class ENGINE_EXPORT RenderWindow{
	public:
		RenderWindow(int width, int height);
		void Quit(int exit_code);
		void SwapBuffer();
	private:
		int m_WindowWidth,m_WindowHeight;
};

#endif

