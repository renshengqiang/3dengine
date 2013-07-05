#include "MeshManager.h"

//-----------------------------------------------------------------------
template<> MeshManager* Singleton<MeshManager>::msSingleton = 0;
MeshManager& MeshManager::GetSingleton(void)
{  
	assert( msSingleton );	return ( *msSingleton );  
}
MeshManager* MeshManager::GetSingletonPtr(void)
{
	return msSingleton;
}
MeshManager::MeshManager()
{
}
MeshManager::~MeshManager()
{
}
//-----------------------------------------------------------------------
MeshPtr MeshManager::CreateMesh(const std::string& name)
{
	MeshPtrMapIterator it = mMeshPtrMap.find(name);
	if(it != mMeshPtrMap.end()) return it->second;
	Mesh *pMesh = new Mesh(name);
	MeshPtr ptr(pMesh);
	mMeshPtrMap.insert(MeshPtrMapValueType(name, ptr));
	return ptr;
}
//-----------------------------------------------------------------------
MeshPtr MeshManager::CreatePlane(const std::string& name, const Plane& plane, float width, float height, 
								int xsegments, int ysegments, bool normals, 
								float xTile, float yTile, const Vector3f& upVector)
{
	MeshPtrMapIterator it = mMeshPtrMap.find(name);
	if(it != mMeshPtrMap.end()) return it->second;
	
	// store parameters
        MeshBuildParams params;
        //params.type = MBT_PLANE;
        params.plane = plane;
        params.width = width;
        params.height = height;
        params.xsegments = xsegments;
        params.ysegments = ysegments;
        params.normals = normals;
        //params.numTexCoordSets = numTexCoordSets;
        params.xTile = xTile;
        params.yTile = yTile;
        params.upVector = upVector;
        //params.vertexBufferUsage = vertexBufferUsage;
        //params.indexBufferUsage = indexBufferUsage;
        //params.vertexShadowBuffer = vertexShadowBuffer;
        //params.indexShadowBuffer = indexShadowBuffer;
		
	Mesh *pMesh = new Mesh();
	loadManualPlane(pMesh, params);
	
	MeshPtr ptr(pMesh);
	mMeshPtrMap.insert(MeshPtrMapValueType(name, ptr));
	return ptr;
}
//-----------------------------------------------------------------------
void MeshManager::loadManualPlane(Mesh *pMesh, MeshBuildParams& params)
{
	SubMesh *pSubMesh = new SubMesh();
	
	// Work out the transform required
	// Default orientation of plane is normal along +z, distance 0
	Matrix4f xlate, xform, rot;
	Matrix3f rot3;
	xlate.InitIdentity();
	rot.InitIdentity();
	// Determine axes
	Vector3f zAxis, yAxis, xAxis;
	zAxis = params.plane.normal;
	zAxis.Normalize();
	yAxis = params.upVector;
	yAxis.Normalize();
	xAxis = yAxis.Cross(zAxis);
	
	if (xAxis.length() == 0)
	{
		//upVector must be wrong
		fprintf(stderr, "MeshManager::CreatePlane : The upVector you supplied is parallel to the plane normal, so is not valid.");
	}
	
	//从朝向观察者的方向来看，是正确的
	rot3.FromAxes(xAxis, yAxis, zAxis);
	rot = rot3;

	// Set up standard transform from origin 此处为什么是负的?
	xlate.InitTranslationTransform(params.plane.normal * -params.plane.d);

	// concatenate 最后的偏移矩阵
	xform = xlate * rot;

	// Generate vertex data
	float xSpace = params.width / params.xsegments;
	float ySpace = params.height / params.ysegments;
	float halfWidth = params.width / 2;
	float halfHeight = params.height / 2;
	float xTex = (1.0f * params.xTile) / params.xsegments;
	float yTex = (1.0f * params.yTile) / params.ysegments;
	Vector3f vec;
	Vector3f min = Vector3f::ZERO, max = Vector3f::UNIT_SCALE;
	bool firstTime = true;

	for (int y = 0; y < params.ysegments + 1; ++y)
	{
		for (int x = 0; x < params.xsegments + 1; ++x)
		{
			// Work out centered on origin
			vec.x = (x * xSpace) - halfWidth;
			vec.y = (y * ySpace) - halfHeight;
			vec.z = 0.0f;
			// Transform by orientation and distance
			vec = xform.transformAffine(vec);
			// Assign to geometry
			pSubMesh->AddCoord(vec);

			// Build bounds as we go
			if (firstTime)
			{
				min = vec;
				max = vec;
				firstTime = false;
			}
			else
			{
				min.makeFloor(vec);
				max.makeCeil(vec);
			}

			if (params.normals)
			{
				/*
				// Default normal is along unit Z
				vec = Vector3f::UNIT_Z;
				// Rotate
				vec = rot.transformAffine(vec);
				*/
			}
			pSubMesh->AddTextureCoord(Vector2f(x*xTex, 1-y*yTex));
		} // x
	} // y
	tesselate2DMesh(pSubMesh, params.xsegments + 1, params.ysegments + 1);
	pMesh->AddSubMesh(pSubMesh);
	pMesh->SetBoundingBox(min, max);
}
//-----------------------------------------------------------------------
void MeshManager::tesselate2DMesh(SubMesh* pSub, unsigned short meshWidth, unsigned short meshHeight)
{
	// The mesh is built, just make a list of indexes to spit out the triangles
	unsigned short vInc, uInc, v, u;
	unsigned short vCount, uCount;
	
	// Make tris in a zigzag pattern (compatible with strips)
	vInc = 1;
	v = 0;
	u = 0;
	uInc = 1; // Start with moving +u

	unsigned short v1, v2, v3;		

	vCount = meshHeight - 1;
	while (vCount--)
	{
		uCount = meshWidth - 1;
		while (uCount--)
		{
			// First Tri in cell
			//*v2
			//*
			//*    *
			//v1   v3
			v1 = ((v + vInc) * meshWidth) + u;
			v2 = (v * meshWidth) + u;
			v3 = ((v + vInc) * meshWidth) + (u + uInc);
			// Output indexes
			pSub->AddIndex(v2);
			pSub->AddIndex(v1);
			pSub->AddIndex(v3);
			
			// Second Tri in cell
			//*v2 *v3
			//       *
			//       *v1
			v1 = ((v + vInc) * meshWidth) + (u + uInc);
			v2 = (v * meshWidth) + u;
			v3 = (v * meshWidth) + (u + uInc);
			// Output indexes			
			pSub->AddIndex(v2);
			pSub->AddIndex(v1);
			pSub->AddIndex(v3);
	
			// Next column
			u += uInc;
		}
		// Next row
		v += vInc;
		u = 0;
	}
}
