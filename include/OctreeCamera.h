#ifndef _CTREECAMERA_H
#define _CTREECAMERA_H

#include <AxisAlignedBox.h>
#include <Camera.h>
#include <string>

/** Specialized viewpoint from which an Octree can be rendered.
@remarks
This class contains several specializations of the Ogre::Camera class. It
implements the getRenderOperation method in order to return displayable geometry
for debugging purposes. It also implements a visibility function that is more granular
than the default.
*/

class   OctreeCamera : public Camera
{
public:

	/** Visibility types */
	enum Visibility
	{
	    NONE,
	    PARTIAL,
	    FULL
	};

	/* Standard constructor */
	OctreeCamera(Vector3f pos=Vector3f(0,0,0),Vector3f target=Vector3f(0,0,-1), Vector3f up=Vector3f(0,1,0));
	/* Standard destructor */
	~OctreeCamera();

	/** Returns the visiblity of the box
	*/
	OctreeCamera::Visibility GetVisibility( const AxisAlignedBox &bound );

};

#endif
