#include "Plane.h"
#include "AxisAlignedBox.h"

//-----------------------------------------------------------------------
Plane::Plane ()
{
	normal = Vector3f(0.0f, 0.0f, 0.0f);
	d = 0.0;
}
//-----------------------------------------------------------------------
Plane::Plane (const Plane& rhs)
{
	normal = rhs.normal;
	d = rhs.d;
}
//-----------------------------------------------------------------------
Plane::Plane (const Vector3f& rkNormal, float fConstant)
{
	normal = rkNormal;
	d = -fConstant;
}
//---------------------------------------------------------------------
Plane::Plane (float a, float b, float c, float _d)
	: normal(a, b, c), d(_d)
{
}
//-----------------------------------------------------------------------
Plane::Plane (const Vector3f& rkNormal, const Vector3f& rkPoint)
{
	redefine(rkNormal, rkPoint);
}
//-----------------------------------------------------------------------
Plane::Plane (const Vector3f& rkPoint0, const Vector3f& rkPoint1,
	const Vector3f& rkPoint2)
{
	redefine(rkPoint0, rkPoint1, rkPoint2);
}
//-----------------------------------------------------------------------
float Plane::getDistance (const Vector3f& rkPoint) const
{
	return normal.dotProduct(rkPoint) + d;
}
//-----------------------------------------------------------------------
Plane::Side Plane::getSide (const Vector3f& rkPoint) const
{
	float fDistance = getDistance(rkPoint);

	if ( fDistance < 0.0 )
		return Plane::NEGATIVE_SIDE;

	if ( fDistance > 0.0 )
		return Plane::POSITIVE_SIDE;

	return Plane::NO_SIDE;
}
//-----------------------------------------------------------------------
Plane::Side Plane::getSide (const AxisAlignedBox& box) const
{
	if (box.isNull()) 
		return NO_SIDE;
	if (box.isInfinite())
		return BOTH_SIDE;

    return getSide(box.getCenter(), box.getHalfSize());
}
//-----------------------------------------------------------------------
Plane::Side Plane::getSide (const Vector3f& centre, const Vector3f& halfSize) const
{
    // Calculate the distance between box centre and the plane
    float dist = getDistance(centre);

    // Calculate the maximise allows absolute distance for
    // the distance between box centre and plane
    float maxAbsDist = normal.absDotProduct(halfSize);

    if (dist < -maxAbsDist)
        return Plane::NEGATIVE_SIDE;

    if (dist > +maxAbsDist)
        return Plane::POSITIVE_SIDE;

    return Plane::BOTH_SIDE;
}
//-----------------------------------------------------------------------
void Plane::redefine(const Vector3f& rkPoint0, const Vector3f& rkPoint1,
	const Vector3f& rkPoint2)
{
	Vector3f kEdge1 = rkPoint1 - rkPoint0;
	Vector3f kEdge2 = rkPoint2 - rkPoint0;
	normal = kEdge1.Cross(kEdge2);
	normal.Normalize();
	d = -normal.dotProduct(rkPoint0);
}
//-----------------------------------------------------------------------
void Plane::redefine(const Vector3f& rkNormal, const Vector3f& rkPoint)
{
	normal = rkNormal;
	d = -rkNormal.dotProduct(rkPoint);
}
//-----------------------------------------------------------------------
Vector3f Plane::projectVector(const Vector3f& p) const
{
	// We know plane normal is unit length, so use simple method
	Matrix3f xform;
	xform[0][0] = 1.0f - normal.x * normal.x;
	xform[0][1] = -normal.x * normal.y;
	xform[0][2] = -normal.x * normal.z;
	xform[1][0] = -normal.y * normal.x;
	xform[1][1] = 1.0f - normal.y * normal.y;
	xform[1][2] = -normal.y * normal.z;
	xform[2][0] = -normal.z * normal.x;
	xform[2][1] = -normal.z * normal.y;
	xform[2][2] = 1.0f - normal.z * normal.z;
	return xform * p;
}
//-----------------------------------------------------------------------
float Plane::normalise(void)
{
    float fLength = normal.length();

    // Will also work for zero-sized vectors, but will change nothing
	// We're not using epsilons because we don't need to.
    // Read http://www.ogre3d.org/forums/viewtopic.php?f=4&t=61259
    if ( fLength > float(0.0f) )
    {
        float fInvLength = 1.0f / fLength;
        normal *= fInvLength;
        d *= fInvLength;
    }

    return fLength;
}
/*
//-----------------------------------------------------------------------
std::ostream& operator<< (std::ostream& o, const Plane& p)
{
	o << "Plane(normal=" << p.normal << ", d=" << p.d << ")";
	return o;
}
*/
