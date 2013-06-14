#include "Mesh.h"
#include "Render.h"
#include "Shader.h"
#include "util.h"
#include "SkeletonBone.h"
#include "Animation.h"
#include "AnimationTrack.h"
#include "KeyFrame.h"
#include <assert.h>

//-----------------------------------------------------------------------
Mesh::Mesh(const std::string &fileName):
	mp_scene(NULL),
	mp_skeleton(NULL),
	m_finalized(false)
{
	m_numBones = 0;
	_LoadMesh(fileName);
}
//--------------------------------------------------------------------------------------
Mesh::~Mesh()
{
	_Clear();
}
//--------------------------------------------------------------------------------------
Skeleton *Mesh::GetSkeleton(void)
{
	return mp_skeleton;
}
//--------------------------------------------------------------------------------------
unsigned Mesh::GetRelatedBoneNum(void)
{
	return m_numBones;
}
//--------------------------------------------------------------------------------------
void Mesh::_Clear()
{
	for (unsigned int i = 0 ; i < m_textures.size() ; i++) {
		SAFE_DELETE(m_textures[i]);
	}
	for(unsigned int i = 0; i < m_subMeshes.size(); ++i){
		SAFE_DELETE(m_subMeshes[i]);
	}
	if(mp_skeleton) delete mp_skeleton;
}
//--------------------------------------------------------------------------------------
bool Mesh::_LoadMesh(const std::string& filename)
{
	// Release the previously loaded mesh (if it exists)
	_Clear();
	mp_scene = m_importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (mp_scene) {
		Matrix4f globalInverseTransform;
		globalInverseTransform = mp_scene->mRootNode->mTransformation;
		globalInverseTransform.Inverse();
		
		m_subMeshes.resize(mp_scene->mNumMeshes);
		m_textures.resize(mp_scene->mNumMaterials);
		
		printf("-----------------------------------------\n");
		printf("Parsing Mesh : '%s'\n", filename.c_str());
		//Initlialize the materials
		if(_InitMaterials(filename) == false)
			return false;
		
		//Initilize the skeleton info
		_InitSkeleton(filename);
		
		// Initialize the meshes in the scene one by one
		printf("Mesh '%s' has %d subMesh(es):\n", filename.c_str(), m_subMeshes.size());
		for (unsigned int i = 0 ; i < m_subMeshes.size() ; ++i) {
			const aiMesh* paiMesh = mp_scene->mMeshes[i];
			m_subMeshes[i] = new SubMesh();
			_InitSubMesh(i, paiMesh);
		}
		if(mp_skeleton){
			mp_skeleton->SetGloabalInverseMatrix(globalInverseTransform);
			m_numBones = mp_skeleton->GetBoneNum();
		}
				
		//Free the aiScene
		m_importer.FreeScene();
		mp_scene = NULL;
		
		printf("Parsing Mesh : '%s' end\n", filename.c_str());
		printf("-----------------------------------------\n\n");
		return true;
	}else {
		printf("Error parsing '%s': '%s'\n", filename.c_str(), m_importer.GetErrorString());
		return false;
	}
}
//--------------------------------------------------------------------------------------
bool Mesh::_InitMaterials(const std::string& filename)
{
	// Extract the directory part from the file name
	std::string::size_type SlashIndex = filename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) {
	    Dir = ".";
	}
	else if (SlashIndex == 0) {
	    Dir = "/";
	}
	else {
	    Dir = filename.substr(0, SlashIndex);
	}

	bool Ret = true;

	// Initialize the materials
	for (unsigned int i = 0 ; i < mp_scene->mNumMaterials ; i++) {
	    const aiMaterial* pMaterial = mp_scene->mMaterials[i];

	    m_textures[i] = NULL;

	    if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
	        aiString Path;

	        if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
	            std::string FullPath = Dir + "/" + Path.data;
	            m_textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());
		/*
	            if (!m_textures[i]->Load()) {
	                printf("Error loading texture '%s'\n", FullPath.c_str());
	                delete m_textures[i];
	                m_textures[i] = NULL;
	                Ret = false;
	            }
	            else {
	                printf("Loaded texture '%s'\n", FullPath.c_str());
	            }
	         */
	        }
	    }

	    // Load a white texture in case the model does not include its own texture
	    if (!m_textures[i]) {
		m_textures[i] = new Texture(GL_TEXTURE_2D, "./models/white.png");
		/*
	        m_textures[i] = new Texture(GL_TEXTURE_2D, "./white.png");

	        Ret = m_textures[i]->Load();
	        */
	    }
	}
    return Ret;
}
//--------------------------------------------------------------------------------------
void Mesh::_InitSubMesh(unsigned int index, const aiMesh* paiMesh)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	printf("subMesh %d has %d vertices, %d faces\n", index, paiMesh->mNumVertices, paiMesh->mNumFaces);
	for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
		const aiVector3D *pPos      = &(paiMesh->mVertices[i]);
		//const aiVector3D *pNormal   = &(paiMesh->mNormals[i]);
		const aiVector3D *pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		m_subMeshes[index]->AddCoord(Vector3f(pPos->x, pPos->y, pPos->z));
		m_subMeshes[index]->AddTextureCoord(Vector2f(pTexCoord->x, pTexCoord->y));
	}
	for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		
		assert(Face.mNumIndices == 3);
		m_subMeshes[index]->AddIndex(Face.mIndices[0]);
		m_subMeshes[index]->AddIndex(Face.mIndices[1]);
		m_subMeshes[index]->AddIndex(Face.mIndices[2]);
	}
	_InitSubMeshAttachedBoneInfo(paiMesh, m_subMeshes[index]);
	m_subMeshes[index]->AddTexture(m_textures[paiMesh->mMaterialIndex]);

	//m_subMeshes[index]->Finalize();
}
//--------------------------------------------------------------------------------------
void Mesh::_InitSubMeshAttachedBoneInfo(const aiMesh* pMesh, SubMesh *subMesh)
{
	for(unsigned int i=0; i< pMesh->mNumBones;++i){
		unsigned int boneIndex = 0;
		std::string boneName(pMesh->mBones[i]->mName.data);

		if(mp_skeleton) boneIndex = mp_skeleton->AddBoneOffset(boneName, pMesh->mBones[i]->mOffsetMatrix);
		for(unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; ++j){
			unsigned short vertexId = pMesh->mBones[i]->mWeights[j].mVertexId;
			float weight = pMesh->mBones[i]->mWeights[j].mWeight;

			subMesh->AddBoneData(vertexId,boneIndex,weight);
		}
	}
}
//--------------------------------------------------------------------------------------
void Mesh::_InitSkeleton(const std::string& filename)
{
	printf("Mesh '%s' has %d skeleton animation(s):\n", filename.c_str(), mp_scene->mNumAnimations);
	for(unsigned int i=0;i<mp_scene->mNumAnimations;++i){
		const aiAnimation *pAnimation = mp_scene->mAnimations[i];
		printf("Animation name : '%s' duration %f ticksPerSecond %f\n",pAnimation->mName.data, pAnimation->mDuration, pAnimation->mTicksPerSecond);
	}
	if(mp_scene->mNumAnimations > 0){
		std::string boneName(mp_scene->mRootNode->mName.data);
		Matrix4f boneTrans(mp_scene->mRootNode->mTransformation);
		
		mp_skeleton = new Skeleton();
		SkeletonBone *rootBone = new SkeletonBone(boneName, boneTrans);
		mp_skeleton->SetRootBoneNode(rootBone);
		mp_skeleton->AddBoneNode(boneName, rootBone);
		_InitSkeletonNodeHeirarchy(rootBone, mp_scene->mRootNode);
		_InitSkeletonAnimation();
	}
}
//--------------------------------------------------------------------------------------
void Mesh::_InitSkeletonNodeHeirarchy(SkeletonBone *pBone, const aiNode *paiNode)
{
	for(unsigned i=0; i < paiNode->mNumChildren; ++i){
		std::string boneName(paiNode->mChildren[i]->mName.data);
		Matrix4f boneTrans(paiNode->mChildren[i]->mTransformation);

		SkeletonBone *childBone = pBone->CreateChildBone(boneName, boneTrans);
		mp_skeleton->AddBoneNode(boneName, childBone);
		_InitSkeletonNodeHeirarchy(childBone, paiNode->mChildren[i]);
	}
}
//--------------------------------------------------------------------------------------
void Mesh::_InitSkeletonAnimation(void)
{
	for(unsigned i = 0; i < mp_scene->mNumAnimations; ++i){
		float ticks = mp_scene->mAnimations[i]->mDuration;
		float ticksPerSecond = mp_scene->mAnimations[i]->mTicksPerSecond;
		char *name = mp_scene->mAnimations[i]->mName.data;
		SkeletonAnimation *pSkeletonAnimation = new SkeletonAnimation(mp_skeleton, name, ticks, ticksPerSecond);
		if(ticksPerSecond == 0) ticksPerSecond =  24.0f;

		for(unsigned j = 0; j < mp_scene->mAnimations[i]->mNumChannels; ++j){
			const aiNodeAnim *pAiNodeAnim = mp_scene->mAnimations[i]->mChannels[j];
			SkeletonNodeTrack *pTrack = new SkeletonNodeTrack(pAiNodeAnim->mNodeName.data);

			for(unsigned k = 0; k < ticks;++k){
				Vector3f transVec(0, 0, 0);
				Vector3f scaleVec(1, 1, 1);
				Quaternion rotateQuat(0,0,0,1);

				if(pAiNodeAnim->mNumPositionKeys > k){
					const aiVector3D &trans = pAiNodeAnim->mPositionKeys[k].mValue;
					transVec = Vector3f(trans.x, trans.y, trans.z);
				}
				if(pAiNodeAnim->mNumScalingKeys > k){
					const aiVector3D &scale = pAiNodeAnim->mScalingKeys[k].mValue;
					scaleVec = Vector3f(scale.x, scale.y, scale.z);
				}
				if(pAiNodeAnim->mNumRotationKeys > k){
					const aiQuaternion &rotate = pAiNodeAnim->mRotationKeys[k].mValue;
					rotateQuat = Quaternion(rotate.x, rotate.y, rotate.z, rotate.w);
				}

				pTrack->AddKeyFrame(transVec, rotateQuat,scaleVec);
			}
			pSkeletonAnimation->AddTrack(pTrack);
		}
		mp_skeleton->AddAnimation(pSkeletonAnimation);
	}
}
//--------------------------------------------------------------------------------------
void Mesh::Render()
{
	for(unsigned int i=0; i < m_subMeshes.size(); ++i){
		//UpdateSubMesh(meshEntry);
		//UpdateVertexObject(meshEntry.vertexObject,(float*)&meshEntry.finalVertexVector[0]);
		m_subMeshes[i]->Render();
	}
}
//--------------------------------------------------------------------------------------
void Mesh::RenderUseShader()
{
	if(m_finalized == false)
	{
		for(unsigned i=0; i < m_textures.size(); ++i)
		{
			m_textures[i]->Load();
		}
	}
	for(unsigned int i=0;i < m_subMeshes.size();++i){
		if(m_finalized == false)
			m_subMeshes[i]->Finalize();
		m_subMeshes[i]->RenderUseShader();
	}
	if(m_finalized == false)
	{
		for(unsigned i=0; i < m_textures.size(); ++i)
		{
			delete m_textures[i];
		}
	}
	m_finalized = true;
}
//--------------------------------------------------------------------------------------
/*
	@remarks: for software animation use(skeleton animation & vertex animation)
*/
void Mesh::UpdateSubMesh(SubMesh &subMesh)
{
	Matrix4f finalMatrix;
	for(unsigned i=0; i<subMesh.GetVertexNum();++i){
		if(subMesh.GetVertexAttachedBoneNum(i)> 0){
			finalMatrix.InitZero();
			for(unsigned j=0; j<4;++j){
				//struct AttachedBone *pBone = subMesh.GetVertexAttachedBones(i);
				//finalMatrix += ( m_boneInfo[pBone->boneId[j]].m_finalTransformation * pBone->weight[j]);
			}
		}
		else
			finalMatrix.InitIdentity();
		subMesh.CoordTransform(i,finalMatrix);
	}
	return;
}
