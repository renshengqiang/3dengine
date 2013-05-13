#include "math_3d.h"


const float Quaternion::msEpsilon = 1e-03;

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
/*
	为什么下面是-rkQ?
	因为两者角度大于180度时，需要把第二个角度减去360度，表示从反方向变换到rkQ
	令本来rkQ的角度是r, 变过以后就是r-360
	而四元数采取的是cos(r/2), sin(r/2)*xxx
	于是r/2 变成r/2 -180
	而cos(r/2-180) =-cos(r/2), sin(r/2-180) =-sin(r/2)
	所以得到的是-rkQ
*/
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
/*
	球形插值: 以恒定的速率描述两四元数之间的曲线
	即得到的效果是角度的线性插值θ=(1-t)*θ1+t*θ2
	结论是: q(t) = q1*sin(1-t)(θ)/sin(θ) +q2*sin(tθ)/sin(θ)
	因为涉及到tan的计算，所以需要对cos的值进行一个范围的判断，防止越界
*/
Quaternion Quaternion::slerp (float fT, const Quaternion& rkP,
	  const Quaternion& rkQ, bool shortestPath)
  {
	  float fCos = rkP.Dot(rkQ);
	  Quaternion rkT;

	  // Do we need to invert rotation?
	  if (fCos < 0.0f && shortestPath)
	  {
		  fCos = -fCos;
		  rkT = -rkQ;
	  }
	  else
	  {
		  rkT = rkQ;
	  }

	  if (fabs(fCos) < 1 - msEpsilon)
	  {
		  // Standard case (slerp)
		  float fSin = sqrtf(1 - fCos*fCos);
		  float fAngle = atan2f(fSin, fCos);
		  float fInvSin = 1.0f / fSin;
		  float fCoeff0 = sinf((1.0f - fT) * fAngle) * fInvSin;
		  float fCoeff1 = sinf(fT * fAngle) * fInvSin;
		  return  rkP *fCoeff0 + rkT*fCoeff1;
	  }
	  else
	  {
		  // There are two situations:
		  // 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
		  //	interpolation safely.
		  // 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
		  //	are an infinite number of possibilities interpolation. but we haven't
		  //	have method to fix this case, so just use linear interpolation here.
		  Quaternion t =  rkP  * (1.0f - fT)+ rkT * fT ;
		  // taking the complement requires renormalisation
		  t.Normalize();
		  return t;
	  }
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
