#ifndef MESH_H
#define	MESH_H

#include "Export.h"
#include "SubMesh.h"
#include "math_3d.h"
#include "Texture.h"

#include <Importer.hpp>      // C++ importer interface
#include <scene.h>       // Output data structure
#include <postprocess.h> // Post processing flags

#include <vector>
#include <string>

struct BoneInfo{
	Matrix4f m_boneOffset;
	Matrix4f m_finalTransformation;
	BoneInfo()
	{
		m_boneOffset.InitIdentity();
		m_finalTransformation.InitIdentity();
	}
};
class ENGINE_EXPORT Mesh
{
public:
	Mesh(const std::string& Filename);
	~Mesh();
	void RenderUseShader();
	void Render();
	void BoneTransform(float timeInSeconds);

private:
	bool LoadMesh(const std::string& Filename);
	bool InitFromScene(const aiScene* pScene, const std::string& Filename);
	void InitMesh(unsigned int Index, const aiMesh* paiMesh);
	bool InitMaterials(const aiScene* pScene, const std::string& Filename);
	void LoadBones(const aiMesh* pMesh, SubMesh *submesh);
	void Clear();
	//about skeleton animation 
	const aiNodeAnim *FindNodeAnim(const aiAnimation *pAnimation, const std::string nodeName);
	void ReadNodeHeirarchy(float animationTime, const aiNode *pNode, const Matrix4f &parentTransform);
	unsigned FindPosition(float animationTime, const aiNodeAnim *pNodeAnim);
	unsigned FindRotation(float animationTime, const aiNodeAnim *pNodeAnim);
	unsigned FindScaling(float animationTime, const aiNodeAnim *pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D &out, float animationTime, const aiNodeAnim *pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion &out, float animationTime, const aiNodeAnim *pNodeAnim);
	void CalcInterpolatedScaling(aiVector3D &out, float animationTime, const aiNodeAnim *pNodeAnim);
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
};
#endif	/* MESH_H */

