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
struct Matrix4f;
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
	inline float operator [] ( unsigned i ) const
	{
		return *(&x+i);
	}
	inline float& operator [] ( unsigned i )
	{
		return *(&x+i);
	}
	inline float length () const
	{
		return (float)sqrt( x * x + y * y + z * z );
	}
	/** Returns a vector at a point half way between this and the passed
		  in vector.
	  */
	  inline Vector3f midPoint( const Vector3f& vec ) const
	  {
		  return Vector3f(
			  ( x + vec.x ) * 0.5f,
			  ( y + vec.y ) * 0.5f,
			  ( z + vec.z ) * 0.5f );
	  }

	Vector3f &operator+=(const Vector3f &r);
	Vector3f &operator-=(const Vector3f &r);
	Vector3f &operator*=(const Vector3f &r);
	Vector3f &operator/=(const Vector3f &r);
	Vector3f &operator/=(float scale);
	Vector3f& operator*=(float f);
	inline float dotProduct(const Vector3f& v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}
	 inline float absDotProduct(const Vector3f& vec) const
        {
            return fabs(x * vec.x) + fabs(y * vec.y) + fabs(z * vec.z);
        }
	Vector3f Cross(const Vector3f& v) const;
	float Normalize();
	void Rotate(float Angle, const Vector3f& Axis);
	inline void makeFloor( const Vector3f& v )
	{
		if( v.x < x ) x = v.x;
		if( v.y < y ) y = v.y;
		if( v.z < z ) z = v.z;
	}	
	inline void makeCeil( const Vector3f& v )
	{
		if( v.x > x ) x = v.x;
		if( v.y > y ) y = v.y;
		if( v.z > z ) z = v.z;
	}

	// special points
	static const Vector3f ZERO;
	static const Vector3f UNIT_X;
	static const Vector3f UNIT_Y;
	static const Vector3f UNIT_Z;
	static const Vector3f UNIT_SCALE;
};

Vector3f operator+(const Vector3f& l, const Vector3f& r);
Vector3f operator-(const Vector3f& l, const Vector3f& r);
Vector3f operator*(const Vector3f& l, float f);
Vector3f operator*(const Vector3f &l, const Vector3f &r);
Vector3f operator*(const Vector3f& v, const Quaternion &q);
Vector3f operator/(const Vector3f &l, const Vector3f &r);
Vector3f operator/(const Vector3f &l, float scale);
bool operator==(const Vector3f &l, const Vector3f &r);
bool operator!=(const Vector3f &l, const Vector3f &r);
bool operator>(const Vector3f &l, const Vector3f &r);
bool operator<(const Vector3f &l, const Vector3f &r);

struct Vector4f
{
	float x,y,z,w;
	Vector4f(float _x, float _y, float _z, float _w);

	Vector4f& operator*=(const Matrix4f& r);
};
Vector4f operator*(const Vector4f& l, const Matrix4f &r);
//------------------------------------------------------------------------------------------------------
struct Quaternion
{
	    float x, y, z, w;
		Quaternion();
	    Quaternion(float _x, float _y, float _z, float _w);
		Quaternion(const Vector3f &axis, float angle);

		bool operator==(const Quaternion &r) const;
		bool operator!=(const Quaternion &r) const;
		Quaternion &operator+=(const Quaternion &r);
		Quaternion &operator-=(const Quaternion &r);
		Quaternion &operator*=(const Quaternion &r);
		Quaternion &operator*=(const Vector3f &v);
		Quaternion &operator*=(float t);
		Quaternion operator-() const;
		
		/// Normalises this quaternion
		void Normalize();
		Quaternion Inverse () const;// apply to non-zero quaternion
		Quaternion Exp () const;
		Quaternion Log () const;
	    	Quaternion Conjugate();  
		/// translate the quaternion to transform matrix
		void ToRotationMatrix (Matrix3f& kRot) const;
		// functions of a quaternion
        	/// Returns the dot product of the quaternion
		float Dot (const Quaternion& rkQ) const;
		/** Performs Normalised linear interpolation between two quaternions, and returns the result.
			nlerp ( 0.0f, A, B ) = A
			nlerp ( 1.0f, A, B ) = B
			@remarks
			Nlerp is faster than Slerp.
			Nlerp has the proprieties of being commutative (@see Slerp;
			commutativity is desired in certain places, like IK animation), and
			being torque-minimal (unless shortestPath=false). However, it's performing
			the interpolation at non-constant velocity; sometimes this is desired,
			sometimes it is not. Having a non-constant velocity can produce a more
			natural rotation feeling without the need of tweaking the weights; however
			if your scene relies on the timing of the rotation or assumes it will point
			at a specific angle at a specific weight value, Slerp is a better choice.
		*/
		static Quaternion nlerp(float fT, const Quaternion& rkP,const Quaternion& rkQ, bool shortestPath = false);
		/** Performs Spherical linear interpolation between two quaternions, and returns the result.
			Slerp ( 0.0f, A, B ) = A
			Slerp ( 1.0f, A, B ) = B
			@return Interpolated quaternion
			@remarks
			Slerp has the proprieties of performing the interpolation at constant
			velocity, and being torque-minimal (unless shortestPath=false).
			However, it's NOT commutative, which means
			Slerp ( 0.75f, A, B ) != Slerp ( 0.25f, B, A );
			therefore be careful if your code relies in the order of the operands.
			This is specially important in IK animation.
		*/
		static Quaternion slerp(float fT, const Quaternion& rkP,const Quaternion& rkQ, bool shortestPath = false);
		static Quaternion squad (float fT,	const Quaternion& rkP, const Quaternion& rkA,
								const Quaternion& rkB, const Quaternion& rkQ, bool shortestPath = false);
		
		/// Cutoff for sine near zero
        	static const float msEpsilon;
		
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
	inline float* operator [] (int  iRow)
        {
           return (float*)m[iRow];
        }
	/// Matrix * vector [3x3 * 3x1 = 3x1]
        Vector3f operator* (const Vector3f& rkVector) const;
	void FromAxes(const Vector3f& xAxis, const Vector3f& yAxis, const Vector3f& zAxis);
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
	void operator = ( const Matrix3f& mat3 );

	inline float* operator [] (int  iRow)
        {
            assert( iRow < 4 );
            return m[iRow];
        }
        inline const float *operator [] (int iRow ) const
        {
            assert( iRow < 4 );
            return m[iRow];
        }
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
	inline bool isAffine(void) const
        {
            return m[3][0] == 0 && m[3][1] == 0 && m[3][2] == 0 && m[3][3] == 1;
        }
	/** 3-D Vector transformation specially for an affine matrix.
            @remarks
                Transforms the given 3-D vector by the matrix, projecting the 
                result back into <i>w</i> = 1.
            @note
                The matrix must be an affine matrix. @see Matrix4::isAffine.
        */
        inline Vector3f transformAffine(const Vector3f& v) const
        {
            assert(isAffine());

            return Vector3f(
                    m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3], 
                    m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3],
                    m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]);
        }

        /** 4-D Vector transformation specially for an affine matrix.
            @note
                The matrix must be an affine matrix. @see Matrix4::isAffine.
        */
        inline Vector4f transformAffine(const Vector4f& v) const
        {
            assert(isAffine());

            return Vector4f(
                m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w, 
                m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
                m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
                v.w);
        }
	Matrix4f operator*(const Matrix4f& Right) const;
	void InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
	void InitRotateTransform(float RotateX, float RotateY, float RotateZ);
	void InitTranslationTransform(float x, float y, float z);
	void InitTranslationTransform(Vector3f trans);
	void InitCameraTransform(const Vector3f& Target, const Vector3f& Up);	
	void InitCameraTransform(const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up);
	void InitPersProjTransform(float FOV, float Width, float Height, float zNear, float zFar);
	void InitPersProjTransform(float FOV, float ar, float zNear, float zFar);
	void MakeTransform(const Vector3f& position, const Vector3f& scale, const Quaternion& orientation);
};
Matrix4f operator*(const Matrix4f &m,float weight);
Vector3f operator*(const Matrix4f &matrix,const Vector3f &vector);
#endif	/* MATH_3D_H */

