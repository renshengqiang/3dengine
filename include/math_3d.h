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
struct Quaternion;
struct Matrix3f;
//---------------------------------------------------------------------
struct Vector2i
{
    int x;
    int y;
};

struct Vector2f
{
    float x;
    float y;

    Vector2f();
    Vector2f(float _x, float _y);
};

struct Vector3f
{
    float x;
    float y;
    float z;

    Vector3f();
    Vector3f(float _x, float _y, float _z);
	Vector3f(const Vector3f &vector);
	
    Vector3f &operator+=(const Vector3f &r);
    Vector3f &operator-=(const Vector3f &r);
	Vector3f &operator*=(const Vector3f &r);
	Vector3f &operator/=(const Vector3f &r);
    Vector3f& operator*=(float f);
    Vector3f Cross(const Vector3f& v) const;
    Vector3f& Normalize();
    void Rotate(float Angle, const Vector3f& Axis);
};

Vector3f operator+(const Vector3f& l, const Vector3f& r);
Vector3f operator-(const Vector3f& l, const Vector3f& r);
Vector3f operator*(const Vector3f& l, float f);
Vector3f operator*(const Vector3f &l, const Vector3f &r);
Vector3f operator*(const Vector3f& v, const Quaternion &q);
Vector3f operator/(const Vector3f &l, const Vector3f &r);
bool operator==(const Vector3f &l, const Vector3f &r);

//------------------------------------------------------------------------------------------------------
struct Quaternion
{
	    float x, y, z, w;
		Quaternion();
	    Quaternion(float _x, float _y, float _z, float _w);
		Quaternion(const Vector3f &axis, float angle);

		Quaternion &operator+=(const Quaternion &r);
		Quaternion &operator-=(const Quaternion &r);
		Quaternion &operator*=(const Quaternion &r);
		Quaternion &operator*=(const Vector3f &v);
		Quaternion &operator*=(float t);
		Quaternion operator-() const;

	    void Normalize();
		void ToRotationMatrix (Matrix3f& kRot) const;
		float Dot (const Quaternion& rkQ) const;
		static Quaternion nlerp(float fT, const Quaternion& rkP,const Quaternion& rkQ, bool shortestPath);
		Quaternion Inverse () const;
	    Quaternion Conjugate();  
		
 };

Quaternion operator*(const Quaternion& l, const Quaternion& r);
Quaternion operator*(const Quaternion& q, float t);
Quaternion operator*(const Quaternion& q, const Vector3f& v);
Quaternion operator+(const Quaternion& l, const Quaternion& r);
Quaternion operator-(const Quaternion& l, const Quaternion& r);

//--------------------------------------------------------------------------------------------------
struct Matrix3f
{
	float m[3][3];
};
struct Matrix4f
{
    float m[4][4];

    Matrix4f();
	Matrix4f(const aiMatrix4x4& assimpMatrix);    
    Matrix4f(const aiMatrix3x3& assimpMatrix);
	Matrix4f(float a00, float a01, float a02, float a03,
             float a10, float a11, float a12, float a13,
             float a20, float a21, float a22, float a23,
             float a30, float a31, float a32, float a33);

	Matrix4f &operator+=(const Matrix4f &r);
	Matrix4f &operator*=(float weight);

	//初始化为单位矩阵
    void InitIdentity();
	//初始化为零矩阵
	void InitZero();
	// 转置矩阵
	Matrix4f Transpose() const;
	// 逆矩阵
	Matrix4f& Inverse();
	//计算对应行列式的值
	float Determinant() const; 
	
    Matrix4f operator*(const Matrix4f& Right) const;
    void InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
    void InitRotateTransform(float RotateX, float RotateY, float RotateZ);
    void InitTranslationTransform(float x, float y, float z);
    void InitCameraTransform(const Vector3f& Target, const Vector3f& Up);	
	void InitCameraTransform(const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up);
    void InitPersProjTransform(float FOV, float Width, float Height, float zNear, float zFar);
	void InitPersProjTransform(float FOV, float ar, float zNear, float zFar);
	void MakeTransform(const Vector3f& position, const Vector3f& scale, const Quaternion& orientation);
	
	
};
Matrix4f operator*(const Matrix4f &m,float weight);
Vector3f operator*(const Matrix4f &matrix,const Vector3f &vector);
#endif	/* MATH_3D_H */

