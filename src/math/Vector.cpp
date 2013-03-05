#include "math_3d.h"

//---------- vector2f ----------------------------------
Vector2f::Vector2f()
{
	x = 0;
	y = 0;
}

Vector2f::Vector2f(float _x, float _y)
{
	x = _x;
	y = _y;
}

//-------------vector3f --------------------------------
Vector3f::Vector3f()
{
	x = 0;
	y = 0;
	z = 0;
}

Vector3f::Vector3f(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}
Vector3f::Vector3f(const Vector3f &vector)
{
	x = vector.x;
	y = vector.y;
	z = vector.z;
}
Vector3f& Vector3f::operator+=(const Vector3f& r)
{
	x += r.x;
	y += r.y;
	z += r.z;

	return *this;
}

Vector3f& Vector3f::operator-=(const Vector3f& r)
{
	x -= r.x;
	y -= r.y;
	z -= r.z;

	return *this;
}

Vector3f& Vector3f::operator*=(const Vector3f &r)
{
	x*=r.x;
	y*=r.y;
	z*=r.z;
	return *this;
}

Vector3f& Vector3f::operator/=(const Vector3f &r)
{
	x/=r.x;
	y/=r.y;
	z/=r.z;
	return *this;
}
Vector3f& Vector3f::operator*=(float f)
{
	x *= f;
	y *= f;
	z *= f;

	return *this;
}

Vector3f Vector3f::Cross(const Vector3f& v) const
{
	const float _x = y * v.z - z * v.y;
	const float _y = z * v.x - x * v.z;
	const float _z = x * v.y - y * v.x;

	return Vector3f(_x, _y, _z);
}

Vector3f& Vector3f::Normalize()
{
	const float length = sqrtf(x * x + y * y + z * z);
	assert(length!=0);
	x /= length;
	y /= length;
	z /= length;

	return *this;
}

void Vector3f::Rotate(float Angle, const Vector3f& Axe)
{
	const float SinHalfAngle = sinf(ToRadian(Angle/2));
	const float CosHalfAngle = cosf(ToRadian(Angle/2));

	const float Rx = Axe.x * SinHalfAngle;
	const float Ry = Axe.y * SinHalfAngle;
	const float Rz = Axe.z * SinHalfAngle;
	const float Rw = CosHalfAngle;
	Quaternion RotationQ(Rx, Ry, Rz, Rw);

	Quaternion ConjugateQ = RotationQ.Conjugate();
	//ConjugateQ.Normalize();
	Quaternion W = RotationQ * (*this) * ConjugateQ;

	x = W.x;
	y = W.y;
	z = W.z;
}

Vector3f operator+(const Vector3f& l, const Vector3f& r)
{
	Vector3f ret(l);

	ret+=r;
	return ret;
}

Vector3f operator-(const Vector3f& l, const Vector3f& r)
{
	Vector3f ret(l);

	ret-=r;
	return ret;
}

Vector3f operator*(const Vector3f& l, float f)
{
	Vector3f ret(l);

	ret*=f;
	return ret;
}

Vector3f operator*(const Vector3f &l, const Vector3f &r)
{
	Vector3f ret(l);

	ret*=r;
	return ret;
}

Vector3f operator/(const Vector3f &l, const Vector3f &r)
{
	Vector3f ret(l);

	ret/=r;
	return ret;
}

Vector3f operator*(const Vector3f& v, const Quaternion &q)
{
	// nVidia SDK implementation
	Vector3f uv, uuv;
	Vector3f qvec(q.x, q.y, q.z);
	uv = qvec.Cross(v);
	uuv = qvec.Cross(uv);
	uv *= (2.0f * q.w);
	uuv *= 2.0f;

	return v + uv + uuv;
}

bool operator==(const Vector3f &l, const Vector3f &r)
{
	return l.x==r.x && l.y==r.y && l.z==r.z;
}

