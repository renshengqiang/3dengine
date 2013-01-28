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

struct VertexBoneAttachWeight{
	unsigned short m_boneId;
	float weight;
};
struct VertexBoneAttachInfo{
	std::vector<struct VertexBoneAttachWeight> vertexBoneAttachWeightVector;

	void AddBoneAttachInfo(unsigned short boneId, float weight)
	{
		struct VertexBoneAttachWeight attachWeight;
		attachWeight.m_boneId = boneId;
		attachWeight.weight = weight;

		vertexBoneAttachWeightVector.push_back(attachWeight);
		return;
	}
};
struct Vertex
{
    Vector3f m_pos;
    Vector2f m_tex;
    Vector3f m_normal;
    Vertex() {}

    Vertex(const Vector3f& pos, const Vector2f& tex, const Vector3f& normal)
    {
        m_pos    = pos;
        m_tex    = tex;
        m_normal = normal;
    }
    Vertex(const Vector3f& pos, const Vector2f& tex)
    {
        m_pos = pos;
        m_tex = tex;
    }
};

class ENGINE_EXPORT Mesh
{
public:
	#define INVALID_MATERIAL 0xFFFFFFFF

    struct MeshEntry {
        MeshEntry();
        ~MeshEntry();

        void Init(const std::vector<Vertex>& Vertices,
                  const std::vector<unsigned int>& Indices);
		inline void AddBoneData(int index, unsigned short boneId, float weight)
		{
			struct VertexBoneAttachInfo &boneAttachInfo = vertexBoneAttachInfoVector[index];
			boneAttachInfo.AddBoneAttachInfo(boneId,weight);
			return;
		}
		//下面两个数组中对应下标顶点保持对应一致
		std::vector<struct Vertex> vertexVector;
		std::vector<struct Vertex> finalVertexVector;
		std::vector<struct VertexBoneAttachInfo> vertexBoneAttachInfoVector;
		VERTEX_OBJ *vertexObject;
		INDEX_OBJ *indexObject;
        unsigned int MaterialIndex;
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
	struct SkeletonAnimation{
		AnimationStateSet animationStateSet;
		
	};
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

