#include "SimpleSpline.h"

//-----------------------------------------------------------------------------
SimpleSpline::SimpleSpline()
{
	// Set up matrix
	// Hermite polynomial
	mCoeffs[0][0] = 2;
	mCoeffs[0][1] = -2;
	mCoeffs[0][2] = 1;
	mCoeffs[0][3] = 1;
	mCoeffs[1][0] = -3;
	mCoeffs[1][1] = 3;
	mCoeffs[1][2] = -2;
	mCoeffs[1][3] = -1;
	mCoeffs[2][0] = 0;
	mCoeffs[2][1] = 0;
	mCoeffs[2][2] = 1;
	mCoeffs[2][3] = 0;
	mCoeffs[3][0] = 1;
	mCoeffs[3][1] = 0;
	mCoeffs[3][2] = 0;
	mCoeffs[3][3] = 0;

	mAutoCalc = true;
}
//-----------------------------------------------------------------------------
SimpleSpline::~SimpleSpline()
{
}
//-----------------------------------------------------------------------------
void SimpleSpline::addPoint(const Vector3f& p)
{
	mPoints.push_back(p);
	if (mAutoCalc)
	{
		recalcTangents();
	}
}
//-----------------------------------------------------------------------------
Vector3f SimpleSpline::interpolate(float t) const
{
	// Currently assumes points are evenly spaced, will cause velocity
	// change where this is not the case
	// TODO: base on arclength?

	
	// Work out which segment this is in
	float fSeg = t * (mPoints.size() - 1);
	unsigned int segIdx = (unsigned int)fSeg;
	// Apportion t 
	t = fSeg - segIdx;

	return interpolate(segIdx, t);

}
//-----------------------------------------------------------------------------
Vector3f SimpleSpline::interpolate(unsigned int fromIndex, float t) const
{
	// Bounds check
	assert (fromIndex < mPoints.size() &&
		"fromIndex out of bounds");

	if ((fromIndex + 1) == mPoints.size())
	{
		// Duff request, cannot blend to nothing
		// Just return source
		return mPoints[fromIndex];

	}

	// Fast special cases
	if (t == 0.0f)
	{
		return mPoints[fromIndex];
	}
	else if(t == 1.0f)
	{
		return mPoints[fromIndex + 1];
	}

	// Real interpolation
	// Form a vector of powers of t
	float t2, t3;
	t2 = t * t;
	t3 = t2 * t;
	Vector4f powers(t3, t2, t, 1);


	// Algorithm is ret = powers * mCoeffs * Matrix4(point1, point2, tangent1, tangent2)
	const Vector3f& point1 = mPoints[fromIndex];
	const Vector3f& point2 = mPoints[fromIndex+1];
	const Vector3f& tan1 = mTangents[fromIndex];
	const Vector3f& tan2 = mTangents[fromIndex+1];
	Matrix4f pt;

	pt[0][0] = point1.x;
	pt[0][1] = point1.y;
	pt[0][2] = point1.z;
	pt[0][3] = 1.0f;
	pt[1][0] = point2.x;
	pt[1][1] = point2.y;
	pt[1][2] = point2.z;
	pt[1][3] = 1.0f;
	pt[2][0] = tan1.x;
	pt[2][1] = tan1.y;
	pt[2][2] = tan1.z;
	pt[2][3] = 1.0f;
	pt[3][0] = tan2.x;
	pt[3][1] = tan2.y;
	pt[3][2] = tan2.z;
	pt[3][3] = 1.0f;

	Vector4f ret = powers * mCoeffs * pt;


	return Vector3f(ret.x, ret.y, ret.z);
}
//-----------------------------------------------------------------------------
void SimpleSpline::recalcTangents(void)
{
	// Catmull-Rom approach
	// 
	// tangent[i] = 0.5 * (point[i+1] - point[i-1])
	//
	// Assume endpoint tangents are parallel with line with neighbour

	size_t i, numPoints;
	bool isClosed;

	numPoints = mPoints.size();
	if (numPoints < 2)
	{
		// Can't do anything yet
		return;
	}

	// Closed or open?
	if (mPoints[0] == mPoints[numPoints-1])
	{
		isClosed = true;
	}
	else
	{
		isClosed = false;
	}

	mTangents.resize(numPoints);



	for(i = 0; i < numPoints; ++i)
	{
		if (i ==0)
		{
			// Special case start
			if (isClosed)
			{
				// Use numPoints-2 since numPoints-1 is the last point and == [0]
				mTangents[i] = (mPoints[1] - mPoints[numPoints-2]) *0.5;
			}
			else
			{
				mTangents[i] =(mPoints[1] - mPoints[0])*0.5;
			}
		}
		else if (i == numPoints-1)
		{
			// Special case end
			if (isClosed)
			{
				// Use same tangent as already calculated for [0]
				mTangents[i] = mTangents[0];
			}
			else
			{
				mTangents[i] = (mPoints[i] - mPoints[i-1])*0.5;
			}
		}
		else
		{
			mTangents[i] = (mPoints[i+1] - mPoints[i-1])*0.5;
		}
		
	}



}
//-----------------------------------------------------------------------------
const Vector3f& SimpleSpline::getPoint(unsigned short index) const
{
	assert (index < mPoints.size() && "Point index is out of bounds!!");

	return mPoints[index];
}
//-----------------------------------------------------------------------------
unsigned short SimpleSpline::getNumPoints(void) const
{
	return (unsigned short)mPoints.size();
}
//-----------------------------------------------------------------------------
void SimpleSpline::clear(void)
{
	mPoints.clear();
	mTangents.clear();
}
//-----------------------------------------------------------------------------
void SimpleSpline::updatePoint(unsigned short index, const Vector3f& value)
{
	assert (index < mPoints.size() && "Point index is out of bounds!!");

	mPoints[index] = value;
	if (mAutoCalc)
	{
		recalcTangents();
	}
}
//-----------------------------------------------------------------------------
void SimpleSpline::setAutoCalculate(bool autoCalc)
{
	mAutoCalc = autoCalc;
}
