/*

	Copyright 2010 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MATH_3D_H
#define	MATH_3D_H
#include <vector3.h>
#include <matrix3x3.h>
#include <matrix4x4.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#define ToRadian(x) ((x) * M_PI / 180.0f)
#define ToDegree(x) ((x) * 180.0f / M_PI)

struct Vector2i
{
    int x;
    int y;
};

struct Vector2f
{
    float x;
    float y;

    Vector2f()
    {
    }

    Vector2f(float _x, float _y)
    {
        x = _x;
        y = _y;
    }
};


struct Vector3f
{
    float x;
    float y;
    float z;

    Vector3f()
    {
    }

    Vector3f(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    Vector3f& operator+=(const Vector3f& r)
    {
        x += r.x;
        y += r.y;
        z += r.z;

        return *this;
    }

    Vector3f& operator-=(const Vector3f& r)
    {
        x -= r.x;
        y -= r.y;
        z -= r.z;

        return *this;
    }

    Vector3f& operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;

        return *this;
    }
	Vector3f& operator*=(const Vector3f &r)
	{
		x*=r.x;
		y*=r.y;
		z*=r.z;
		return *this;
	}

    Vector3f Cross(const Vector3f& v) const;

    Vector3f& Normalize();

    void Rotate(float Angle, const Vector3f& Axis);
};

inline Vector3f operator+(const Vector3f& l, const Vector3f& r)
{
    Vector3f Ret(l.x + r.x,
                 l.y + r.y,
                 l.z + r.z);

    return Ret;
}

inline Vector3f operator-(const Vector3f& l, const Vector3f& r)
{
    Vector3f Ret(l.x - r.x,
                 l.y - r.y,
                 l.z - r.z);

    return Ret;
}

inline Vector3f operator*(const Vector3f& l, float f)
{
    Vector3f Ret(l.x * f,
                 l.y * f,
                 l.z * f);

    return Ret;
}
inline Vector3f operator*(const Vector3f &l, const Vector3f &r)
{
	Vector3f ret;
	ret.x = l.x * r.x;
	ret.y = l.y * r.y;
	ret.z = l.z * r.z;
	return ret;
}
inline Vector3f operator/(const Vector3f &l, const Vector3f &r)
{
	Vector3f ret;
	ret.x = l.x/r.x;
	ret.y = l.y/r.y;
	ret.z = l.z/r.z;
	return ret;
}
inline bool operator==(const Vector3f &l, const Vector3f &r)
{
	return l.x==r.x && l.y==r.y && l.z==r.z;
}

struct Matrix3f;
struct Quaternion
{
	    float x, y, z, w;
		Quaternion();
	    Quaternion(float _x, float _y, float _z, float _w);
		Quaternion(const Vector3f &axis, float angle);

	    void Normalize();
		void ToRotationMatrix (Matrix3f& kRot) const;
		float Dot (const Quaternion& rkQ) const;
		static Quaternion nlerp(float fT, const Quaternion& rkP,const Quaternion& rkQ, bool shortestPath);
		Quaternion Inverse () const;
	    Quaternion Conjugate();  
		Quaternion operator-() const;
 };

Quaternion operator*(const Quaternion& l, const Quaternion& r);
Quaternion operator*(const Quaternion& q, float t);
Quaternion operator*(const Quaternion& q, const Vector3f& v);
Quaternion operator+(const Quaternion& l, const Quaternion& r);
Quaternion operator-(const Quaternion& l, const Quaternion& r);
Vector3f operator*(const Vector3f& v, const Quaternion &q);/*其实应该是将四元数放在前面的，但是为了和上面的函数进行区分，调换了参数的顺序*/

struct Matrix3f
{
	float m[3][3];
};
struct Matrix4f
{
    float m[4][4];

    inline Matrix4f(){};
	Matrix4f(const aiMatrix4x4& AssimpMatrix);
    
    Matrix4f(const aiMatrix3x3& AssimpMatrix);
	Matrix4f(float a00, float a01, float a02, float a03,
             float a10, float a11, float a12, float a13,
             float a20, float a21, float a22, float a23,
             float a30, float a31, float a32, float a33);
    void InitIdentity();
	void InitZero();
	Matrix4f Transpose() const;
    Matrix4f operator*(const Matrix4f& Right) const;
	Matrix4f operator*(float weight) const;
	Matrix4f &operator+=(const Matrix4f &Right);
    void InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
    void InitRotateTransform(float RotateX, float RotateY, float RotateZ);
    void InitTranslationTransform(float x, float y, float z);
    void InitCameraTransform(const Vector3f& Target, const Vector3f& Up);	
	void InitCameraTransform(const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up);
    void InitPersProjTransform(float FOV, float Width, float Height, float zNear, float zFar);
	void InitPersProjTransform(float FOV, float ar, float zNear, float zFar);
	void MakeTransform(const Vector3f& position, const Vector3f& scale, const Quaternion& orientation);
	float Determinant() const; //计算对应行列式的值
	Matrix4f& Inverse();//计算逆矩阵
};
Vector3f operator*(const Matrix4f &matrix,const Vector3f &vector);
#endif	/* MATH_3D_H */

