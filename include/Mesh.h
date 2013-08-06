#ifndef MESH_H
#define	MESH_H

#include <Export.h>
#include <SubMesh.h>
#include <math_3d.h>
#include <Texture.h>
#include <Skeleton.h>
#include <AxisAlignedBox.h>
#include <SimpleMeshEffect.h>

#include <Importer.hpp>      // C++ importer interface
#include <scene.h>       // Output data structure
#include <postprocess.h> // Post processing flags

#include <vector>
#include <string>
#include <tr1/memory>

class ENGINE_EXPORT Mesh
{
public:
	Mesh();
	Mesh(const std::string& Filename);
	~Mesh();
	void SetName(const std::string& name);
	std::string GetName(void);
	SubMesh *GetSubMesh(unsigned index);
	Skeleton *GetSkeleton(void);
	const AxisAlignedBox& GetBoundingBox(void) const;
	void SetBoundingBox(const Vector3f& min, const Vector3f& max);
	unsigned GetRelatedBoneNum(void);
	void AddSubMesh(SubMesh *pSubMesh);
	void AddTexture(Texture *pTexture);
	void RenderUseShader(SimpleMeshEffect& effect);
	
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
	/// name of the mesh
	std::string m_name;
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

typedef std::tr1::shared_ptr<Mesh>  MeshPtr;
typedef std::tr1::weak_ptr<Mesh> MeshWeakPtr;
#endif	/* MESH_H */

