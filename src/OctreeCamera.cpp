#include <OctreeCamera.h>

//-----------------------------------------------------------------------
OctreeCamera::OctreeCamera(Vector3f pos,Vector3f target, Vector3f up) : Camera(pos, target, up)
{
                                                                      
}
//-----------------------------------------------------------------------
OctreeCamera::~OctreeCamera()
{
}
//-----------------------------------------------------------------------
OctreeCamera::Visibility OctreeCamera::GetVisibility( const AxisAlignedBox &bound )
{
	// Null boxes always invisible
	if (bound.isNull()) return NONE;

	// Infinite boxes always visible
	if (bound.isInfinite()) return FULL;

	// Make any pending updates to the calculated frustum planes
	_UpdateFrustumPlanes();

	// Get centre of the box
	Vector3f centre = bound.getCenter();
	// Get the half-size of the box
	Vector3f halfSize = bound.getHalfSize();

	bool all_inside = true;

	for ( int plane = 0; plane < 6; ++plane )
	{
		// Skip far plane if infinite view frustum
		if (plane == FRUSTUM_PLANE_FAR && m_farDist == 0)
		    continue;

		// This updates frustum planes and deals with cull frustum
		Plane::Side side = m_frustumPlanes[plane].getSide(centre, halfSize);
		if(side == Plane::NEGATIVE_SIDE) return NONE;
		// We can't return now as the box could be later on the negative side of a plane.
		if(side == Plane::BOTH_SIDE) 
		        all_inside = false;
	}

	if ( all_inside )
		return FULL;
	else
		return PARTIAL;
}
