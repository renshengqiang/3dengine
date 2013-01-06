#include <RenderWindow.h>
#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
RenderWindow::RenderWindow(int windowWidth,int windowHeight)
{
	/* Information about the current video settings. */    
	const SDL_VideoInfo* info = NULL;    
	/* Color depth in bits of our window. */    
	int bpp = 0;    
	/* Flags we will pass into SDL_SetVideoMode. */
	int flags = 0;    
	/* First, initialize SDL's video subsystem. */    
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {        
		/* Failed, exit. */        
		fprintf( stderr, "Video initialization failed: %s\n",SDL_GetError());    
		Quit( 1 );    
	}   
	/* Let's get some video information. */    
	info = SDL_GetVideoInfo( );    
	if( !info ) {        
		/* This should probably never happen. */        
		fprintf( stderr, "Video query failed: %s\n",SDL_GetError());        
		Quit(1);    
	}    
	bpp = info->vfmt->BitsPerPixel;    
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );    
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );    
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );    
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );    
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );    
	SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 0);
	
	flags = SDL_OPENGL;    
	/*Set the video mode     */    
	if( SDL_SetVideoMode( windowWidth, windowHeight, bpp, flags ) == 0 ) {        
		fprintf( stderr, "Video mode set failed: %s\n",SDL_GetError());        
		Quit( 1 );    
	}
	m_WindowHeight=windowHeight;
	m_WindowWidth=windowWidth;
}
void RenderWindow::Quit(int exit_code)
{
	SDL_Quit( );
	exit(exit_code);
}
void RenderWindow::SwapBuffer()
{
	SDL_GL_SwapBuffers();
	return;
}