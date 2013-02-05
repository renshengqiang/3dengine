#ifndef MESH_H
#define	MESH_H

#include <map>
#include <vector>
#include <Importer.hpp>      // C++ importer interface
#include <scene.h>       // Output data structure
#include <postprocess.h> // Post processing flags

#include <Export.h>
#include <util.h>
#include <math_3d.h>
#include <Texture.h>
#include <Render.h>
#include <AnimationState.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

#define MAX_BONE_ATTACHED 4
struct AttachedBone{
	float boneId[MAX_BONE_ATTACHED];
	float weight[MAX_BONE_ATTACHED];
	AttachedBone(){
		for(int i=0; i<MAX_BONE_ATTACHED;++i){
			boneId[i]=0;
			weight[i]=0;
		}
	}
};
struct BoneInfo{
	Matrix4f m_boneOffset;
	Matrix4f m_finalTransformation;
	BoneInfo()
	{
		m_boneOffset.InitIdentity();
		m_finalTransformation.InitIdentity();
	}
};
struct MeshEntry 
{
	MeshEntry();
	~MeshEntry();

	void Init(const std::vector<unsigned int>& Indices);
	void AddBoneData(int index, unsigned short boneId, float weight);
	void AddCoord(const Vector3f &coord);
	void AddTextureCoord(const Vector2f &textureCoord);

	std::vector<Vector3f> coordVec;
	std::vector<Vector3f> finalCoordVec;/*for software animation use*/
	std::vector<Vector2f> textureCoordVec;
	std::vector<struct AttachedBone> attachedBoneVec;
	std::vector<int> boneNumVec;//该数组和上面数组对应，用于插入骨骼到正确位置

	VERTEX_OBJ *vertexObject;
	INDEX_OBJ *indexObject;
	unsigned int MaterialIndex;
};

class ENGINE_EXPORT Mesh
{
public:
	Mesh();
	Mesh(const std::string& Filename);

	~Mesh();

	bool LoadMesh(const std::string& Filename);

	void RenderUseShader();
	void Render();
	
	void BoneTransform(float timeInSeconds);

private:
	bool InitFromScene(const aiScene* pScene, const std::string& Filename);
	void InitMesh(unsigned int Index, const aiMesh* paiMesh);
	bool InitMaterials(const aiScene* pScene, const std::string& Filename);
	void LoadBones(const aiMesh* pMesh, struct MeshEntry &meshEntry);
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
	void UpdateMeshEntry(struct MeshEntry &meshEntry);

private:
	const aiScene *mp_scene;
	Assimp::Importer m_importer;
	//submesh
	std::vector<MeshEntry> m_Entries;
	//texture
	std::vector<Texture*> m_Textures;
	//bone info
	unsigned m_numBones;
	std::map<std::string, unsigned> m_boneMapping;
	std::vector<struct BoneInfo> m_boneInfo;
	Matrix4f m_globalInverseTransform;
};
#endif	/* MESH_H */

