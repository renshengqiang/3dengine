#ifndef _MESH_MANAGER_H
#define _MESH_MANAGER_H
#include "Mesh.h"
#include "Singleton.h"
#include <map>
#include <string>

class MeshManager : public Singleton<MeshManager>{
public:
	typedef std::map<std::string, MeshPtr> MeshPtrMap;
	typedef MeshPtrMap::iterator MeshPtrMapIterator;
	typedef MeshPtrMap::value_type MeshPtrMapValueType;

	MeshManager();
	~MeshManager();

	static MeshManager& GetSingleton(void);
	static MeshManager* GetSingletonPtr(void);

	MeshPtr CreateMesh(const std::string& name);
private:
	MeshPtrMap mMeshPtrMap;
};


#endif
