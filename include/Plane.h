#ifndef __Plane_H__
#define __Plane_H__

#include <math_3d.h>
#include <Export.h>
#include <vector>
/** Defines a plane in 3D space.
    @remarks
        A plane is defined in 3D space by the equation
        Ax + By + Cz + D = 0
    @par
        This equates to a vector (the normal of the plane, whose x, y
        and z components equate to the coefficients A, B and C
        respectively), and a constant (D) which is the distance along
        the normal you have to go to move the plane back to the origin.
 */
class AxisAlignedBox;
class ENGINE_EXPORT Plane
{
public:
    /** Default constructor - sets everything to 0.
    */
    Plane ();
    Plane (const Plane& rhs);
    /** Construct a plane through a normal, and a distance to move the plane along the normal.*/
    Plane (const Vector3f& rkNormal, float fConstant);
	/** Construct a plane using the 4 constants directly **/
	Plane (float a, float b, float c, float d);
    Plane (const Vector3f& rkNormal, const Vector3f& rkPoint);
    Plane (const Vector3f& rkPoint0, const Vector3f& rkPoint1,
        const Vector3f& rkPoint2);

    /** The "positive side" of the plane is the half space to which the
        plane normal points. The "negative side" is the other half
        space. The flag "no side" indicates the plane itself.
    */
    enum Side
    {
        NO_SIDE,
        POSITIVE_SIDE,
        NEGATIVE_SIDE,
        BOTH_SIDE
    };

    Side getSide (const Vector3f& rkPoint) const;

    /**
    Returns the side where the alignedBox is. The flag BOTH_SIDE indicates an intersecting box.
    One corner ON the plane is sufficient to consider the box and the plane intersecting.
    */
    Side getSide (const AxisAlignedBox& rkBox) const;

    /** Returns which side of the plane that the given box lies on.
        The box is defined as centre/half-size pairs for effectively.
    @param centre The centre of the box.
    @param halfSize The half-size of the box.
    @return
        POSITIVE_SIDE if the box complete lies on the "positive side" of the plane,
        NEGATIVE_SIDE if the box complete lies on the "negative side" of the plane,
        and BOTH_SIDE if the box intersects the plane.
    */
    Side getSide (const Vector3f& centre, const Vector3f& halfSize) const;

    /** This is a pseudodistance. The sign of the return value is
        positive if the point is on the positive side of the plane,
        negative if the point is on the negative side, and zero if the
        point is on the plane.
        @par
        The absolute value of the return value is the true distance only
        when the plane normal is a unit length vector.
    */
    float getDistance (const Vector3f& rkPoint) const;

    /** Redefine this plane based on 3 points. */
    void redefine(const Vector3f& rkPoint0, const Vector3f& rkPoint1,
        const Vector3f& rkPoint2);

/** Redefine this plane based on a normal and a point. */
void redefine(const Vector3f& rkNormal, const Vector3f& rkPoint);

/** Project a vector onto the plane. 
@remarks This gives you the element of the input vector that is perpendicular 
	to the normal of the plane. You can get the element which is parallel
	to the normal of the plane by subtracting the result of this method
	from the original vector, since parallel + perpendicular = original.
@param v The input vector
*/
Vector3f projectVector(const Vector3f& v) const;

    /** Normalises the plane.
        @remarks
            This method normalises the plane's normal and the length scale of d
            is as well.
        @note
            This function will not crash for zero-sized vectors, but there
            will be no changes made to their components.
        @return The previous length of the plane's normal.
    */
    float normalise(void);

Vector3f normal;
    float d;

    /// Comparison operator
    bool operator==(const Plane& rhs) const
    {
        return (rhs.d == d && rhs.normal == normal);
    }
    bool operator!=(const Plane& rhs) const
    {
        return (rhs.d != d || rhs.normal != normal);
    }

    //ENGINE_EXPORT friend std::ostream& operator<< (std::ostream& o, const Plane& p);
};

typedef std::vector<Plane> PlaneList;

#endif
