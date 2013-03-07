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
