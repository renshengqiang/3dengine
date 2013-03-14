#include "math_3d.h"

//-----------------------------------------------------------------------
Quaternion::Quaternion()
{
	x=0;y=0;z=0;w=1;
}
//-----------------------------------------------------------------------
Quaternion::Quaternion(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}
//-----------------------------------------------------------------------
Quaternion::Quaternion(const Vector3f &axis, float angle)
{
	float fHalfAngle ( 0.5*ToRadian(angle));
	float fSin = sinf(fHalfAngle);
	w = cosf(fHalfAngle);
	x = fSin*axis.x;
	y = fSin*axis.y;
	z = fSin*axis.z;
}
//-----------------------------------------------------------------------
Quaternion& Quaternion::operator+=(const Quaternion &r)
{
	w += r.w;
	x += r.x;
	y += r.y;
	z += r.z;

	return *this;
}
//-----------------------------------------------------------------------
Quaternion& Quaternion::operator-=(const Quaternion &r)
{
	w -= r.w;
	x -= r.x;
	y -= r.y;
	z -= r.z;
	
	return *this;
}
//-----------------------------------------------------------------------
Quaternion& Quaternion::operator*=(const Quaternion &r)
{
    const float tw = (w * r.w) - (x * r.x) - (y * r.y) - (z * r.z);
    const float tx = (x * r.w) + (w * r.x) + (y * r.z) - (z * r.y);
    const float ty = (y * r.w) + (w * r.y) + (z * r.x) - (x * r.z);
    const float tz = (z * r.w) + (w * r.z) + (x * r.y) - (y * r.x);

	w = tw;
	x = tx;
	y = ty;
	z = tz;
	
    return *this;
}
//-----------------------------------------------------------------------
Quaternion& Quaternion::operator*=(const Vector3f &v)
{
    const float tw = - (x * v.x) - (y * v.y) - (z * v.z);
    const float tx =   (w * v.x) + (y * v.z) - (z * v.y);
    const float ty =   (w * v.y) + (z * v.x) - (x * v.z);
    const float tz =   (w * v.z) + (x * v.y) - (y * v.x);

	w = tw;
	x = tx;
	y = ty;
	z = tz;

    return *this;
}
//-----------------------------------------------------------------------
Quaternion& Quaternion::operator*=(float t)
{
	w *= t;
	x *= t;
	y *= t;
	z *= t;

	return *this;
}
//-----------------------------------------------------------------------
Quaternion Quaternion::operator-() const
{
	return Quaternion(-x,-y,-z,-w);
}
//-----------------------------------------------------------------------
void Quaternion::Normalize()
{
    float Length = sqrtf(x * x + y * y + z * z + w * w);

    x /= Length;
    y /= Length;
    z /= Length;
    w /= Length;
}
//-----------------------------------------------------------------------
Quaternion Quaternion::Conjugate()
{
    Quaternion ret(-x, -y, -z, w);
    return ret;
}
//-----------------------------------------------------------------------
Quaternion Quaternion::Inverse () const
{
  float fNorm = w*w+x*x+y*y+z*z;
  if ( fNorm > 0.0 )
  {
	  float fInvNorm = 1.0f/fNorm;
	  return Quaternion(w*fInvNorm,-x*fInvNorm,-y*fInvNorm,-z*fInvNorm);
  }
  else
  {
	  // return an invalid result to flag the error
	  return Quaternion(0,0,0,0);
  }
}
//-----------------------------------------------------------------------
float Quaternion::Dot (const Quaternion& rkQ) const
{
   return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
}
//-----------------------------------------------------------------------
Quaternion Quaternion::nlerp(float fT, const Quaternion& rkP,
    const Quaternion& rkQ, bool shortestPath)
{
	Quaternion result;
    float fCos = rkP.Dot(rkQ);
	if (fCos < 0.0f && shortestPath)
	{
		result = rkP + ((-rkQ) - rkP)*fT;
	}
	else
	{
		result = rkP +(rkQ - rkP)*fT;
	}
    result.Normalize();
    return result;
}
//-----------------------------------------------------------------------
void Quaternion::ToRotationMatrix (Matrix3f& kRot) const
{
    float fTx  = x+x;
    float fTy  = y+y;
    float fTz  = z+z;
    float fTwx = fTx*w;
    float fTwy = fTy*w;
    float fTwz = fTz*w;
    float fTxx = fTx*x;
    float fTxy = fTy*x;
    float fTxz = fTz*x;
    float fTyy = fTy*y;
    float fTyz = fTz*y;
    float fTzz = fTz*z;

    kRot.m[0][0] = 1.0f-(fTyy+fTzz);
    kRot.m[0][1] = fTxy-fTwz;
    kRot.m[0][2] = fTxz+fTwy;
    kRot.m[1][0] = fTxy+fTwz;
    kRot.m[1][1] = 1.0f-(fTxx+fTzz);
    kRot.m[1][2] = fTyz-fTwx;
    kRot.m[2][0] = fTxz-fTwy;
    kRot.m[2][1] = fTyz+fTwx;
    kRot.m[2][2] = 1.0f-(fTxx+fTyy);
}
//-----------------------------------------------------------------------
Quaternion operator+(const Quaternion& l, const Quaternion& r)
{
	Quaternion ret(l);

	ret+=r;

	return ret;
}
//-----------------------------------------------------------------------
Quaternion operator-(const Quaternion& l, const Quaternion& r)
{
	Quaternion ret(l);

	ret-=r;

	return ret;

}
//-----------------------------------------------------------------------
Quaternion operator*(const Quaternion& l, const Quaternion& r)
{
	Quaternion ret(l);

	ret*=r;

	return ret;
}
//-----------------------------------------------------------------------
Quaternion operator*(const Quaternion& q, float t)
{
	Quaternion ret(q);

	ret*=t;

	return ret;

}
//-----------------------------------------------------------------------
Quaternion operator*(const Quaternion& q, const Vector3f& v)
{
	Quaternion ret(q);

	ret*=v;

	return ret;

}
