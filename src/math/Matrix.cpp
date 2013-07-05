#include "math_3d.h"

//-----------------------------------------------------------------------
Vector3f Matrix3f::operator* (const Vector3f& rkPoint) const
{
	Vector3f kProd;
	for (size_t iRow = 0; iRow < 3; iRow++)
	{
		kProd[iRow] =
			m[iRow][0]*rkPoint[0] +
			m[iRow][1]*rkPoint[1] +
			m[iRow][2]*rkPoint[2];
	}
	return kProd;
}
void Matrix3f::FromAxes(const Vector3f& xAxis, const Vector3f& yAxis, const Vector3f& zAxis)
{
	m[0][0] = xAxis.x; m[1][0] = xAxis.y; m[2][0] = xAxis.z;
	m[0][1] = yAxis.x; m[1][1] = yAxis.y; m[2][1] = yAxis.z;
	m[0][2] = zAxis.x; m[1][2] = zAxis.y; m[2][2] = zAxis.z;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
Matrix4f::Matrix4f()
{
	InitIdentity();
}
//-----------------------------------------------------------------------
Matrix4f::Matrix4f(const aiMatrix4x4& AssimpMatrix)
{
	m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = AssimpMatrix.a4;
	m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = AssimpMatrix.b4;
	m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = AssimpMatrix.c4;
	m[3][0] = AssimpMatrix.d1; m[3][1] = AssimpMatrix.d2; m[3][2] = AssimpMatrix.d3; m[3][3] = AssimpMatrix.d4;
}
//-----------------------------------------------------------------------
Matrix4f::Matrix4f(const aiMatrix3x3& AssimpMatrix)
{
	m[0][0] = AssimpMatrix.a1; m[0][1] = AssimpMatrix.a2; m[0][2] = AssimpMatrix.a3; m[0][3] = 0.0f;
	m[1][0] = AssimpMatrix.b1; m[1][1] = AssimpMatrix.b2; m[1][2] = AssimpMatrix.b3; m[1][3] = 0.0f;
	m[2][0] = AssimpMatrix.c1; m[2][1] = AssimpMatrix.c2; m[2][2] = AssimpMatrix.c3; m[2][3] = 0.0f;
	m[3][0] = 0.0f			 ; m[3][1] = 0.0f			; m[3][2] = 0.0f		   ; m[3][3] = 1.0f;
}
//-----------------------------------------------------------------------
Matrix4f::Matrix4f(float a00, float a01, float a02, float a03,
		 float a10, float a11, float a12, float a13,
		 float a20, float a21, float a22, float a23,
		 float a30, float a31, float a32, float a33)
{
	m[0][0] = a00; m[0][1] = a01; m[0][2] = a02; m[0][3] = a03;
	m[1][0] = a10; m[1][1] = a11; m[1][2] = a12; m[1][3] = a13;
	m[2][0] = a20; m[2][1] = a21; m[2][2] = a22; m[2][3] = a23;
	m[3][0] = a30; m[3][1] = a31; m[3][2] = a32; m[3][3] = a33; 	   
}
//-----------------------------------------------------------------------
void Matrix4f::operator = ( const Matrix3f& mat3 )
{
	m[0][0] = mat3.m[0][0]; m[0][1] = mat3.m[0][1]; m[0][2] = mat3.m[0][2];
	m[1][0] = mat3.m[1][0]; m[1][1] = mat3.m[1][1]; m[1][2] = mat3.m[1][2];
	m[2][0] = mat3.m[2][0]; m[2][1] = mat3.m[2][1]; m[2][2] = mat3.m[2][2];
}
//-----------------------------------------------------------------------
Matrix4f &Matrix4f::operator+=(const Matrix4f &Right)
{
	for(int i=0;i<4;++i)
		for(int j=0;j<4;++j)
			m[i][j] += Right.m[i][j];
	return *this;
}
//-----------------------------------------------------------------------
Matrix4f &Matrix4f::operator*=(float weight)
{
	for(int i=0;i<4;++i){
		for(int j=0;j<4;++j)
			m[i][j] = m[i][j] * weight;
	}
	return *this;
}
//-----------------------------------------------------------------------
Matrix4f Matrix4f::operator*(const Matrix4f& Right) const
{
	Matrix4f Ret;

	for (unsigned int i = 0 ; i < 4 ; i++) {
		for (unsigned int j = 0 ; j < 4 ; j++) {
			Ret.m[i][j] = m[i][0] * Right.m[0][j] +
						  m[i][1] * Right.m[1][j] +
						  m[i][2] * Right.m[2][j] +
						  m[i][3] * Right.m[3][j];
		}
	}

	return Ret;
}
//-----------------------------------------------------------------------
void Matrix4f::InitIdentity()
{
	m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
	m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}
//-----------------------------------------------------------------------
void Matrix4f::InitZero()
{
	m[0][0] = 0.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
	m[1][0] = 0.0f; m[1][1] = 0.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 0.0f; m[2][3] = 0.0f;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 0.0f;	
}
//-----------------------------------------------------------------------
Matrix4f Matrix4f::Transpose() const
{
	Matrix4f n;
	
	for (unsigned int i = 0 ; i < 4 ; i++) {
		for (unsigned int j = 0 ; j < 4 ; j++) {
			n.m[i][j] = m[j][i];
		}
	}
	
	return n;
}
//-----------------------------------------------------------------------
float Matrix4f::Determinant() const
{
	return m[0][0]*m[1][1]*m[2][2]*m[3][3] - m[0][0]*m[1][1]*m[2][3]*m[3][2] + m[0][0]*m[1][2]*m[2][3]*m[3][1] - m[0][0]*m[1][2]*m[2][1]*m[3][3] 
		+ m[0][0]*m[1][3]*m[2][1]*m[3][2] - m[0][0]*m[1][3]*m[2][2]*m[3][1] - m[0][1]*m[1][2]*m[2][3]*m[3][0] + m[0][1]*m[1][2]*m[2][0]*m[3][3] 
		- m[0][1]*m[1][3]*m[2][0]*m[3][2] + m[0][1]*m[1][3]*m[2][2]*m[3][0] - m[0][1]*m[1][0]*m[2][2]*m[3][3] + m[0][1]*m[1][0]*m[2][3]*m[3][2] 
		+ m[0][2]*m[1][3]*m[2][0]*m[3][1] - m[0][2]*m[1][3]*m[2][1]*m[3][0] + m[0][2]*m[1][0]*m[2][1]*m[3][3] - m[0][2]*m[1][0]*m[2][3]*m[3][1] 
		+ m[0][2]*m[1][1]*m[2][3]*m[3][0] - m[0][2]*m[1][1]*m[2][0]*m[3][3] - m[0][3]*m[1][0]*m[2][1]*m[3][2] + m[0][3]*m[1][0]*m[2][2]*m[3][1]
		- m[0][3]*m[1][1]*m[2][2]*m[3][0] + m[0][3]*m[1][1]*m[2][0]*m[3][2] - m[0][3]*m[1][2]*m[2][0]*m[3][1] + m[0][3]*m[1][2]*m[2][1]*m[3][0];
}
//-----------------------------------------------------------------------
Matrix4f& Matrix4f::Inverse()
{
	// Compute the reciprocal determinant
	float det = Determinant();
	if(det == 0.0f) 
	{
		// Matrix not invertible. Setting all elements to nan is not really
		// correct in a mathematical sense but it is easy to debug for the
		// programmer.
		/*const float nan = std::numeric_limits<float>::quiet_NaN();
		*this = Matrix4f(
			nan,nan,nan,nan,
			nan,nan,nan,nan,
			nan,nan,nan,nan,
			nan,nan,nan,nan);*/
        assert(0);
		return *this;
	}

	float invdet = 1.0f / det;

	Matrix4f res;
	res.m[0][0] = invdet  * (m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[1][2] * (m[2][3] * m[3][1] - m[2][1] * m[3][3]) + m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
	res.m[0][1] = -invdet * (m[0][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[0][2] * (m[2][3] * m[3][1] - m[2][1] * m[3][3]) + m[0][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]));
	res.m[0][2] = invdet  * (m[0][1] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) + m[0][2] * (m[1][3] * m[3][1] - m[1][1] * m[3][3]) + m[0][3] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]));
	res.m[0][3] = -invdet * (m[0][1] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) + m[0][2] * (m[1][3] * m[2][1] - m[1][1] * m[2][3]) + m[0][3] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]));
	res.m[1][0] = -invdet * (m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[1][2] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
	res.m[1][1] = invdet  * (m[0][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[0][2] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[0][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]));
	res.m[1][2] = -invdet * (m[0][0] * (m[1][2] * m[3][3] - m[1][3] * m[3][2]) + m[0][2] * (m[1][3] * m[3][0] - m[1][0] * m[3][3]) + m[0][3] * (m[1][0] * m[3][2] - m[1][2] * m[3][0]));
	res.m[1][3] = invdet  * (m[0][0] * (m[1][2] * m[2][3] - m[1][3] * m[2][2]) + m[0][2] * (m[1][3] * m[2][0] - m[1][0] * m[2][3]) + m[0][3] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]));
	res.m[2][0] = invdet  * (m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) + m[1][1] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
	res.m[2][1] = -invdet * (m[0][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) + m[0][1] * (m[2][3] * m[3][0] - m[2][0] * m[3][3]) + m[0][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
	res.m[2][2] = invdet  * (m[0][0] * (m[1][1] * m[3][3] - m[1][3] * m[3][1]) + m[0][1] * (m[1][3] * m[3][0] - m[1][0] * m[3][3]) + m[0][3] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]));
	res.m[2][3] = -invdet * (m[0][0] * (m[1][1] * m[2][3] - m[1][3] * m[2][1]) + m[0][1] * (m[1][3] * m[2][0] - m[1][0] * m[2][3]) + m[0][3] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]));
	res.m[3][0] = -invdet * (m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) + m[1][1] * (m[2][2] * m[3][0] - m[2][0] * m[3][2]) + m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
	res.m[3][1] = invdet  * (m[0][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) + m[0][1] * (m[2][2] * m[3][0] - m[2][0] * m[3][2]) + m[0][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]));
	res.m[3][2] = -invdet * (m[0][0] * (m[1][1] * m[3][2] - m[1][2] * m[3][1]) + m[0][1] * (m[1][2] * m[3][0] - m[1][0] * m[3][2]) + m[0][2] * (m[1][0] * m[3][1] - m[1][1] * m[3][0]));
	res.m[3][3] = invdet  * (m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) + m[0][1] * (m[1][2] * m[2][0] - m[1][0] * m[2][2]) + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0])); 
	*this = res;

	return *this;
}
//-----------------------------------------------------------------------
void Matrix4f::InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ)
{
    m[0][0] = ScaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
    m[1][0] = 0.0f;   m[1][1] = ScaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
    m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = ScaleZ; m[2][3] = 0.0f;
    m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;
}
//-----------------------------------------------------------------------
void Matrix4f::InitRotateTransform(float RotateX, float RotateY, float RotateZ)
{
    Matrix4f rx, ry, rz;

    const float x = ToRadian(RotateX);
    const float y = ToRadian(RotateY);
    const float z = ToRadian(RotateZ);

    rx.m[0][0] = 1.0f; rx.m[0][1] = 0.0f   ; rx.m[0][2] = 0.0f    ; rx.m[0][3] = 0.0f;
    rx.m[1][0] = 0.0f; rx.m[1][1] = cosf(x); rx.m[1][2] = -sinf(x); rx.m[1][3] = 0.0f;
    rx.m[2][0] = 0.0f; rx.m[2][1] = sinf(x); rx.m[2][2] = cosf(x) ; rx.m[2][3] = 0.0f;
    rx.m[3][0] = 0.0f; rx.m[3][1] = 0.0f   ; rx.m[3][2] = 0.0f    ; rx.m[3][3] = 1.0f;

    ry.m[0][0] = cosf(y); ry.m[0][1] = 0.0f; ry.m[0][2] = sinf(y); ry.m[0][3] = 0.0f;
    ry.m[1][0] = 0.0f   ; ry.m[1][1] = 1.0f; ry.m[1][2] = 0.0f    ; ry.m[1][3] = 0.0f;
    ry.m[2][0] = -sinf(y); ry.m[2][1] = 0.0f; ry.m[2][2] = cosf(y) ; ry.m[2][3] = 0.0f;
    ry.m[3][0] = 0.0f   ; ry.m[3][1] = 0.0f; ry.m[3][2] = 0.0f    ; ry.m[3][3] = 1.0f;

    rz.m[0][0] = cosf(z); rz.m[0][1] = -sinf(z); rz.m[0][2] = 0.0f; rz.m[0][3] = 0.0f;
    rz.m[1][0] = sinf(z); rz.m[1][1] = cosf(z) ; rz.m[1][2] = 0.0f; rz.m[1][3] = 0.0f;
    rz.m[2][0] = 0.0f   ; rz.m[2][1] = 0.0f    ; rz.m[2][2] = 1.0f; rz.m[2][3] = 0.0f;
    rz.m[3][0] = 0.0f   ; rz.m[3][1] = 0.0f    ; rz.m[3][2] = 0.0f; rz.m[3][3] = 1.0f;

    *this = rz * ry * rx;
}
//-----------------------------------------------------------------------
void Matrix4f::InitTranslationTransform(float x, float y, float z)
{
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}
//-----------------------------------------------------------------------
void Matrix4f::InitTranslationTransform(Vector3f trans)
{
	InitTranslationTransform(trans.x, trans.y, trans.z);
}
//-----------------------------------------------------------------------
void Matrix4f::InitCameraTransform(const Vector3f& Target, const Vector3f& Up)
{
    Vector3f N = Target;
    N.Normalize();
    Vector3f U = Up;
    U.Normalize();
    U = U.Cross(N);
    Vector3f V = N.Cross(U);

    m[0][0] = U.x;   m[0][1] = U.y;   m[0][2] = U.z;   m[0][3] = 0.0f;
    m[1][0] = V.x;   m[1][1] = V.y;   m[1][2] = V.z;   m[1][3] = 0.0f;
    m[2][0] = N.x;   m[2][1] = N.y;   m[2][2] = N.z;   m[2][3] = 0.0f;
    m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f;
}
//-----------------------------------------------------------------------
void Matrix4f::InitCameraTransform(const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up)
{
	Vector3f N = Target;
    N.Normalize();
    Vector3f U = Up;
    U.Normalize();
    U = N.Cross(U);
	U.Normalize();
    Vector3f V = U.Cross(N);
	V.Normalize();
	
    m[0][0] = U.x;   m[0][1] = U.y;   m[0][2] = U.z;   m[0][3] = 0.0f;
    m[1][0] = V.x;   m[1][1] = V.y;   m[1][2] = V.z;   m[1][3] = 0.0f;
    m[2][0] = -N.x;   m[2][1] = -N.y;   m[2][2] = -N.z;   m[2][3] = 0.0f;
    m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f;
	//multiply pos
	m[0][3] = -(Pos.x*U.x + Pos.y*U.y + Pos.z*U.z);
	m[1][3] = -(Pos.x*V.x + Pos.y*V.y + Pos.z*V.z);
	m[2][3] = (Pos.x*N.x + Pos.y*N.y + Pos.z*N.z);
}
//-----------------------------------------------------------------------
void Matrix4f::InitPersProjTransform(float FOV, float Width, float Height, float zNear, float zFar)
{
    const float ar         = Width / Height;
    const float zRange     = zNear - zFar;
    const float tanHalfFOV = tanf(ToRadian(FOV / 2.0f));

    m[0][0] = 1.0f/(tanHalfFOV * ar); m[0][1] = 0.0f;            m[0][2] = 0.0f;          m[0][3] = 0.0;
    m[1][0] = 0.0f;                   m[1][1] = 1.0f/tanHalfFOV; m[1][2] = 0.0f;          m[1][3] = 0.0;
    m[2][0] = 0.0f;                   m[2][1] = 0.0f;            m[2][2] = (-zNear -zFar)/zRange ; m[2][3] = 2.0f * zFar*zNear/zRange;
    m[3][0] = 0.0f;                   m[3][1] = 0.0f;            m[3][2] = 1.0f;          m[3][3] = 0.0;
}
//-----------------------------------------------------------------------
void Matrix4f::InitPersProjTransform(float FOV, float AR, float zNear, float zFar)
{
	const float zRange	   = zNear - zFar;
	const float tanHalfFOV = tanf(ToRadian(FOV / 2.0f));

	m[0][0] = 1.0f/(tanHalfFOV * AR); m[0][1] = 0.0f;			 m[0][2] = 0.0f;		  m[0][3] = 0.0;
	m[1][0] = 0.0f; 				  m[1][1] = 1.0f/tanHalfFOV; m[1][2] = 0.0f;		  m[1][3] = 0.0;
	m[2][0] = 0.0f; 				  m[2][1] = 0.0f;			 m[2][2] = (zNear + zFar)/zRange ; m[2][3] = 2.0f * zFar*zNear/zRange;
	m[3][0] = 0.0f; 				  m[3][1] = 0.0f;			 m[3][2] = -1.0f;		  m[3][3] = 0.0;
	/*不知道为什么就是不行
	m[0][0] = -1.0f/(tanHalfFOV * ar);  m[0][1] = 0.0f;			 m[0][2] = 0.0f;		  m[0][3] = 0.0;
	m[1][0] = 0.0f; 				  m[1][1] = -1.0f/tanHalfFOV; m[1][2] = 0.0f;		  m[1][3] = 0.0;
	m[2][0] = 0.0f; 				  m[2][1] = 0.0f;			 m[2][2] = -(zNear + zFar)/zRange ; m[2][3] = -2.0f * zFar*zNear/zRange;
	m[3][0] = 0.0f; 				  m[3][1] = 0.0f;			 m[3][2] = 1.0f;		  m[3][3] = 0.0;
	*/
}
//-----------------------------------------------------------------------
void Matrix4f::MakeTransform(const Vector3f& position, const Vector3f& scale, const Quaternion& orientation)
{
    // Ordering:实际效果的顺序，非代码顺序
    //    1. Scale
    //    2. Rotate
    //    3. Translate

    Matrix3f rot3x3;
    orientation.ToRotationMatrix(rot3x3);

    // Set up final matrix with scale, rotation and translation
    m[0][0] = scale.x * rot3x3.m[0][0]; m[0][1] = scale.y * rot3x3.m[0][1]; m[0][2] = scale.z * rot3x3.m[0][2]; m[0][3] = position.x;
    m[1][0] = scale.x * rot3x3.m[1][0]; m[1][1] = scale.y * rot3x3.m[1][1]; m[1][2] = scale.z * rot3x3.m[1][2]; m[1][3] = position.y;
    m[2][0] = scale.x * rot3x3.m[2][0]; m[2][1] = scale.y * rot3x3.m[2][1]; m[2][2] = scale.z * rot3x3.m[2][2]; m[2][3] = position.z;

    // No projection term
    m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
}
//-----------------------------------------------------------------------
Matrix4f operator*(const Matrix4f &m,float weight)
{
	Matrix4f ret(m);

	ret*=weight;

	return ret;
}
//-----------------------------------------------------------------------
/*
	Vector transformation using '*'
	@remarks
	transforms the given 3-D vector by the matrix, projection the result back into w=1
	@note
	This meas that the initial w=1.0, and then all the three elements of the resulting 3-D vector are diveded by the resulting w
*/
Vector3f operator*(const Matrix4f &M,const Vector3f &V)
{
	Vector3f ret;
	float fInvW = 1.0f /(M.m[3][0] * V.x + M.m[3][1] * V.y + M.m[3][2] * V.z + M.m[3][3]);

	ret.x = (M.m[0][0]*V.x + M.m[0][1]*V.y + M.m[0][2]*V.z + M.m[0][3])*fInvW;
	ret.y = (M.m[1][0]*V.x + M.m[1][1]*V.y + M.m[1][2]*V.z + M.m[1][3])*fInvW;
	ret.z = (M.m[2][0]*V.x + M.m[2][1]*V.y + M.m[2][2]*V.z + M.m[2][3])*fInvW;
	return ret;
}
