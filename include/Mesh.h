#ifndef MESH_H
#define	MESH_H

#include "Export.h"
#include "SubMesh.h"
#include "math_3d.h"
#include "Texture.h"
#include "Skeleton.h"

#include <Importer.hpp>      // C++ importer interface
#include <scene.h>       // Output data structure
#include <postprocess.h> // Post processing flags

#include <vector>
#include <string>

class Entity;
class ENGINE_EXPORT Mesh
{
friend class Entity;
public:
	Mesh(const std::string& Filename);
	~Mesh();
	Skeleton *GetSkeleton(void);
	void RenderUseShader(void);
	void Render(void);
private:
	bool _LoadMesh(const std::string& filename);	
	bool _InitMaterials(const std::string& filename);
	void _InitSubMesh(unsigned int index, const aiMesh* paiMesh);
	void _InitSubMeshAttachedBoneInfo(const aiMesh* pMesh, SubMesh *submesh);
	void _InitSkeleton(const std::string& filename);
	void _InitSkeletonNodeHeirarchy(SkeletonBone *pBone, const aiNode *paiNode);
	void _InitSkeletonAnimation(void);
	void _Clear();
	//about skeleton animation 
	void UpdateSubMesh(SubMesh &submesh);

private:
	const aiScene *mp_scene;
	Assimp::Importer m_importer;
	//submesh
	std::vector<SubMesh*> m_subMeshes;
	//texture
	std::vector<Texture*> m_textures;
	//bone info
	unsigned m_numBones;
	std::map<std::string, unsigned> m_boneMapping;
	std::vector<struct BoneInfo> m_boneInfo;
	Matrix4f m_globalInverseTransform;
	//skeleton node Animation
	Skeleton *mp_skeleton;
};
#endif	/* MESH_H */

