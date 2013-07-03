#ifndef _Ray_H_
#define _Ray_H_

#include <math_3d.h>
#include <AxisAlignedBox.h>
#include <utility>

/** Representation of a ray in space, i.e. a line with an origin and direction. */
class  Ray
{
protected:
    Vector3f  mOrigin;
    Vector3f mDirection;
public:
    Ray():mOrigin(Vector3f::ZERO), mDirection(Vector3f::UNIT_Z) {}
    Ray(const Vector3f& origin, const Vector3f& direction)
        :mOrigin(origin), mDirection(direction) {}

    /** Sets the origin of the ray. */
    void setOrigin(const Vector3f& origin) {mOrigin = origin;} 
    /** Gets the origin of the ray. */
    const Vector3f& getOrigin(void) const {return mOrigin;} 

    /** Sets the direction of the ray. */
    void setDirection(const Vector3f& dir) {mDirection = dir;} 
    /** Gets the direction of the ray. */
    const Vector3f& getDirection(void) const {return mDirection;} 

	/** Gets the position of a point t units along the ray. */
	Vector3f getPoint(float t) const { 
		return Vector3f(mOrigin + (mDirection * t));
	}
	
	/** Gets the position of a point t units along the ray. */
	Vector3f operator*(float t) const { 
		return getPoint(t);
	}
	/** Tests whether this ray intersects the given box. 
	@return A pair structure where the first element indicates whether
		an intersection occurs, and if true, the second element will
		indicate the distance along the ray at which it intersects. 
		This can be converted to a point in space by calling getPoint().
	*/
	std::pair<bool, float> intersects(const AxisAlignedBox& box) const;
};
#endif
