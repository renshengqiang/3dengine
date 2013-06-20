#ifndef MESH_H
#define	MESH_H

#include <Export.h>
#include <SubMesh.h>
#include <math_3d.h>
#include <Texture.h>
#include <Skeleton.h>
#include <AxisAlignedBox.h>

#include <Importer.hpp>      // C++ importer interface
#include <scene.h>       // Output data structure
#include <postprocess.h> // Post processing flags

#include <vector>
#include <string>
#include <memory>

class ENGINE_EXPORT Mesh
{
public:
	Mesh(const std::string& Filename);
	~Mesh();
	Skeleton *GetSkeleton(void);
	void RenderUseShader(void);
	unsigned GetRelatedBoneNum(void);
private:
	bool _LoadMesh(const std::string& filename);	
	bool _InitMaterials(const std::string& filename);
	void _InitSubMesh(unsigned int index, const aiMesh* paiMesh);
	void _InitSubMeshAttachedBoneInfo(const aiMesh* pMesh, SubMesh *submesh);
	void _InitSkeleton(const std::string& filename);
	void _InitSkeletonNodeHeirarchy(SkeletonBone *pBone, const aiNode *paiNode);
	void _InitSkeletonAnimation(void);
	void _Clear();

private:
	/// Local bounding box volume
	AxisAlignedBox m_AABB;
	//assimp class info, used to parse the model file
	const aiScene *mp_scene;
	Assimp::Importer m_importer;
	//submesh
	std::vector<SubMesh*> m_subMeshes;
	//texture
	std::vector<Texture*> m_textures;
	//bone info
	unsigned m_numBones;
	//skeleton node Animation
	Skeleton *mp_skeleton;

	bool m_finalized;
	
};

typedef std::shared_ptr<Mesh>  MeshPtr;
#endif	/* MESH_H */

