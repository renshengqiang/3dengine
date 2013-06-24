#include "math_3d.h"

//---------- vector2f ----------------------------------
Vector2f::Vector2f()
{
	x = 0;
	y = 0;
}
//-----------------------------------------------------------------------------
Vector2f::Vector2f(float _x, float _y)
{
	x = _x;
	y = _y;
}

//-------------vector3f --------------------------------
const Vector3f Vector3f::ZERO( 0, 0, 0 );
const Vector3f Vector3f::UNIT_X( 1, 0, 0 );
const Vector3f Vector3f::UNIT_Y( 0, 1, 0 );
const Vector3f Vector3f::UNIT_Z( 0, 0, 1 );
const Vector3f Vector3f::UNIT_SCALE(1, 1, 1);

Vector3f::Vector3f()
{
	x = 0;
	y = 0;
	z = 0;
}
//-----------------------------------------------------------------------------
Vector3f::Vector3f(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}
//-----------------------------------------------------------------------------
Vector3f::Vector3f(const Vector3f &vector)
{
	x = vector.x;
	y = vector.y;
	z = vector.z;
}
//-----------------------------------------------------------------------------
Vector3f& Vector3f::operator+=(const Vector3f& r)
{
	x += r.x;
	y += r.y;
	z += r.z;

	return *this;
}
//-----------------------------------------------------------------------------
Vector3f& Vector3f::operator-=(const Vector3f& r)
{
	x -= r.x;
	y -= r.y;
	z -= r.z;

	return *this;
}
//-----------------------------------------------------------------------------
Vector3f& Vector3f::operator*=(const Vector3f &r)
{
	x*=r.x;
	y*=r.y;
	z*=r.z;
	return *this;
}
//-----------------------------------------------------------------------------
Vector3f& Vector3f::operator/=(const Vector3f &r)
{
	x/=r.x;
	y/=r.y;
	z/=r.z;
	return *this;
}
//-----------------------------------------------------------------------------
Vector3f& Vector3f::operator/=(float scale)
{
	x/=scale;
	y/=scale;
	z/=scale;
	return *this;
}
//-----------------------------------------------------------------------------
Vector3f& Vector3f::operator*=(float f)
{
	x *= f;
	y *= f;
	z *= f;

	return *this;
}
//-----------------------------------------------------------------------------
Vector3f Vector3f::Cross(const Vector3f& v) const
{
	const float _x = y * v.z - z * v.y;
	const float _y = z * v.x - x * v.z;
	const float _z = x * v.y - y * v.x;

	return Vector3f(_x, _y, _z);
}
//-----------------------------------------------------------------------------
float Vector3f::Normalize()
{
	const float length = sqrtf(x * x + y * y + z * z);
	assert(length!=0);
	x /= length;
	y /= length;
	z /= length;

	return length;
}
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
Vector3f operator+(const Vector3f& l, const Vector3f& r)
{
	Vector3f ret(l);

	ret+=r;
	return ret;
}
//-----------------------------------------------------------------------------
Vector3f operator-(const Vector3f& l, const Vector3f& r)
{
	Vector3f ret(l);

	ret-=r;
	return ret;
}
//-----------------------------------------------------------------------------
Vector3f operator*(const Vector3f& l, float f)
{
	Vector3f ret(l);

	ret*=f;
	return ret;
}
//-----------------------------------------------------------------------------
Vector3f operator*(const Vector3f &l, const Vector3f &r)
{
	Vector3f ret(l);

	ret*=r;
	return ret;
}
//-----------------------------------------------------------------------------
Vector3f operator/(const Vector3f &l, const Vector3f &r)
{
	Vector3f ret(l);

	ret/=r;
	return ret;
}
//-----------------------------------------------------------------------------
Vector3f operator/(const Vector3f &l, float scale)
{
	Vector3f ret(l);

	ret/=scale;
	return ret;
}
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
bool operator==(const Vector3f &l, const Vector3f &r)
{
	return l.x==r.x && l.y==r.y && l.z==r.z;
}
bool operator!=(const Vector3f &l, const Vector3f &r)
{
	return l.x!=r.x || l.y !=r.y || l.z != r.z;
}
bool operator>(const Vector3f &l, const Vector3f &r)
{
	return l.x > r.x && l.y > r.y && l.z >r.z;
}
bool operator<(const Vector3f &l, const Vector3f &r)
{
	return l.x < r.x && l.y < r.y && l.z < r.z;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Vector4f::Vector4f(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}
//-----------------------------------------------------------------------------
Vector4f& Vector4f::operator*=(const Matrix4f& r)
{
	x = x*r[0][0] + y*r[1][0] + z*r[2][0] + w*r[3][0];
	y = x*r[0][1] + y*r[1][1] + z*r[2][1] + w*r[3][1];
	z = x*r[0][2] + y*r[1][2] + z*r[2][2] + w*r[3][2];
	w= x*r[0][3] + y*r[1][3] + z*r[2][3] + w*r[3][3];
	return *this;
}
//-----------------------------------------------------------------------------
Vector4f operator*(const Vector4f& l, const Matrix4f &r)
{
	Vector4f ret(l);
	ret*=r;
	return ret;
}
