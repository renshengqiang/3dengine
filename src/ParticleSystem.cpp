#include <Texture.h>
#include <stdlib.h>
#include <math_3d.h>

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------
GLuint g_textureID = -1;

const float PI = 3.141592654f;

float g_fSpinX = 0.0f;
float g_fSpinY = 0.0f;

struct Vertex
{
    float tu, tv;
    float x, y, z;
};

Vertex g_quadVertices[] =
{
    { 0.0f,0.0f, -1.0f,-1.0f, 0.0f },
    { 1.0f,0.0f,  1.0f,-1.0f, 0.0f },
    { 1.0f,1.0f,  1.0f, 1.0f, 0.0f },
    { 0.0f,1.0f, -1.0f, 1.0f, 0.0f }
};

struct Particle
{
    Vector3f m_vCurPos;
    Vector3f m_vCurVel;
	float    m_r, m_g, m_b, m_a;
};

const int MAX_PARTICLES = 200;
Particle g_particles[MAX_PARTICLES];

//-----------------------------------------------------------------------------
// PROTOTYPES
//-----------------------------------------------------------------------------
void initPointSprites(void);
void updatePointSprites(void);
void renderPointSprites(void);
float getRandomMinMax(float fMin, float fMax);
Vector3f getRandomVector(void);


//-----------------------------------------------------------------------------
// Name: getRandomMinMax()
// Desc: Gets a random number between min/max boundaries
//-----------------------------------------------------------------------------
float getRandomMinMax( float fMin, float fMax )
{
    float fRandNum = (float)rand () / RAND_MAX;
    return fMin + (fMax - fMin) * fRandNum;
}

//-----------------------------------------------------------------------------
// Name: getRandomVector()
// Desc: Generates a random vector where X,Y, and Z components are between
//       -1.0 and 1.0
//-----------------------------------------------------------------------------
Vector3f getRandomVector( void )
{
	Vector3f vVector;

    // Pick a random Z between -1.0f and 1.0f.
    vVector.z = getRandomMinMax( -1.0f, 1.0f );
       
    // Get radius of this circle
    float radius = (float)sqrt(1 - vVector.z * vVector.z);
    
    // Pick a random point on a circle.
    float t = getRandomMinMax( -PI, PI );

    // Compute matching X and Y for our Z.
    vVector.x = (float)cosf(t) * radius;
    vVector.y = (float)sinf(t) * radius;

vVector.z = 10.0f;

	return vVector;
}

//-----------------------------------------------------------------------------
// Name: initPointSprites()
// Desc: 
//-----------------------------------------------------------------------------
void initPointSprites( void )
{
	// Load up the point sprite's texture...
	Texture *pTexture = new Texture(GL_TEXTURE_2D, "./textures/particle.bmp");
	pTexture->Load();
	g_textureID = pTexture->GetTextureObj();


	// Query for the max point size supported by the hardware
	float maxSize = 0.0f;
	glGetFloatv( GL_POINT_SIZE_MAX_ARB, &maxSize );
	glPointSize( maxSize );

	// Initialize our particles so they'll start at the origin with some 
	// random direction and color.
	for( int i = 0; i < MAX_PARTICLES; ++i )
	{
		g_particles[i].m_vCurPos = Vector3f(0.0f,0.0f,-100.0f);
		g_particles[i].m_vCurVel = getRandomVector() ;

		g_particles[i].m_r = getRandomMinMax( 0.0f, 1.0f );
		g_particles[i].m_g = getRandomMinMax( 0.0f, 1.0f );
		g_particles[i].m_b = getRandomMinMax( 0.0f, 1.0f );
	}
}

//-----------------------------------------------------------------------------
// Name: updatePointSprites()
// Desc: 
//-----------------------------------------------------------------------------
void updatePointSprites( float dElpasedFrameTime)
{
	// To repeat the sample automatically, keep track of the overall app time.
	for( int i = 0; i < MAX_PARTICLES; ++i )
		if(g_particles[i].m_vCurPos.z > 0)
			g_particles[i].m_vCurPos = Vector3f(0.0f,0.0f,-100.0f);

	// Move each particle via its velocity and elapsed frame time.	
	for( int i = 0; i < MAX_PARTICLES; ++i )
		g_particles[i].m_vCurPos += g_particles[i].m_vCurVel * (float)dElpasedFrameTime/1000;
}

//-----------------------------------------------------------------------------
// Name: renderPointSprites()
// Desc: 
//-----------------------------------------------------------------------------
void renderPointSprites( void )
{
	//set up mvp matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 800.0/600.0, 0.1, 1000);

	
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	//setup texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_textureID);
	
	// Set up for blending...
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE);

	// Set up the OpenGL state machine for using point sprites...

	// This is how will our point sprite's size will be modified by 
	// distance from the viewer
	//1/(a+b*d+c*d^2)来计算大小，d是距离，a,b,c是下面给出的参数
	float quadratic[] =  { 1.0f, 0.0f, 0.01f };
	glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );

	// Query for the max point size supported by the hardware
	float maxSize = 0.0f;
	glGetFloatv( GL_POINT_SIZE_MAX_ARB, &maxSize );

	// Clamp size to 100.0f or the sprites could get a little too big on some  
	// of the newer graphic cards. My ATI card at home supports a max point 
	// size of 1024.0f!
	if( maxSize > 100.0f )
	    maxSize = 100.0f;

	glPointSize( maxSize );

	// The alpha of a point is calculated to allow the fading of points 
	// instead of shrinking them past a defined threshold size. The threshold 
	// is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to 
	// the minimum and maximum point sizes.
	//淡出极限，重采用时有用
	glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f );
	//点的尺寸范围
	glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
	glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, maxSize );

	// Specify point sprite texture coordinate replacement mode for each 
	// texture unit
	//使用局部点坐标代替纹理坐标
	glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );

	// Render point sprites...
	glEnable( GL_POINT_SPRITE_ARB );

	glBegin( GL_POINTS );
	{
		for( int i = 0; i < MAX_PARTICLES; ++i )
		{
			glColor4f( g_particles[i].m_r, 
			           g_particles[i].m_g, 
			           g_particles[i].m_b, 
			           1.0f );

			glVertex3f( g_particles[i].m_vCurPos.x,
			            g_particles[i].m_vCurPos.y,
			            g_particles[i].m_vCurPos.z );
		}
	}
	glEnd();

	glDisable( GL_POINT_SPRITE_ARB );
	glDisable(GL_TEXTURE_2D);
	glDisable( GL_BLEND );

	//clear depth
	glClear(GL_DEPTH_BUFFER_BIT);
}
