#include "Mesh.h"
#include "Render.h"
#include "Shader.h"
#include "util.h"
#include <assert.h>

Mesh::Mesh(const std::string &fileName):
	mp_scene(NULL)
{
	m_numBones = 0;
	LoadMesh(fileName);
}
//--------------------------------------------------------------------------------------
Mesh::~Mesh()
{
	Clear();
}
//--------------------------------------------------------------------------------------
void Mesh::Clear()
{
	for (unsigned int i = 0 ; i < m_textures.size() ; i++) {
		SAFE_DELETE(m_textures[i]);
	}
	for(unsigned int i = 0; i < m_subMeshes.size(); ++i){
		SAFE_DELETE(m_subMeshes[i]);
	}
}
//--------------------------------------------------------------------------------------
bool Mesh::LoadMesh(const std::string& Filename)
{
	// Release the previously loaded mesh (if it exists)
	Clear();

	bool Ret = false;
	mp_scene = m_importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (mp_scene) {
		m_globalInverseTransform = mp_scene->mRootNode->mTransformation;
		m_globalInverseTransform.Inverse();
		Ret = InitFromScene(mp_scene, Filename);

		printf("Mesh %s has %d animation(s)\n", Filename.c_str(), mp_scene->mNumAnimations);
		for(unsigned int i=0;i<mp_scene->mNumAnimations;++i){
			const aiAnimation *pAnimation = mp_scene->mAnimations[i];
			printf("Animation name : %s duration %f ticksPerSecond %f\n",pAnimation->mName.data, pAnimation->mDuration, pAnimation->mTicksPerSecond);
		}
	}
	else {
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_importer.GetErrorString());
	}

	return Ret;
}
//--------------------------------------------------------------------------------------
bool Mesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{  	
	m_subMeshes.resize(pScene->mNumMeshes);
	m_textures.resize(pScene->mNumMaterials);

	//Initlialize the materials
	if(InitMaterials(pScene, Filename) == false)
		return false;
	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0 ; i < m_subMeshes.size() ; ++i) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		m_subMeshes[i] = new SubMesh();
		InitMesh(i, paiMesh);
	}
	return true;
}
//--------------------------------------------------------------------------------------
void Mesh::InitMesh(unsigned int Index, const aiMesh* paiMesh)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
		const aiVector3D *pPos      = &(paiMesh->mVertices[i]);
		//const aiVector3D *pNormal   = &(paiMesh->mNormals[i]);
		const aiVector3D *pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		m_subMeshes[Index]->AddCoord(Vector3f(pPos->x, pPos->y, pPos->z));
		m_subMeshes[Index]->AddTextureCoord(Vector2f(pTexCoord->x, pTexCoord->y));
	}
	LoadBones(paiMesh, m_subMeshes[Index]);
	for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		m_subMeshes[Index]->AddIndex(Face.mIndices[0]);
		m_subMeshes[Index]->AddIndex(Face.mIndices[1]);
		m_subMeshes[Index]->AddIndex(Face.mIndices[2]);
	}
	m_subMeshes[Index]->AddTexture(m_textures[paiMesh->mMaterialIndex]->GetTextureObj());
	m_subMeshes[Index]->Finalize();

}
//--------------------------------------------------------------------------------------
void Mesh::LoadBones(const aiMesh* pMesh, SubMesh *subMesh)
{
	for(unsigned int i=0;i< pMesh->mNumBones;++i){
		unsigned int boneIndex = 0;
		std::string boneName(pMesh->mBones[i]->mName.data);

		if(m_boneMapping.find(boneName) == m_boneMapping.end()){
			//在m_boneMapping中没有改骨骼信息，allocate an index for a new bone
			boneIndex = m_numBones;
			++m_numBones;
			struct BoneInfo bi;
			m_boneInfo.push_back(bi);
			m_boneInfo[boneIndex].m_boneOffset = pMesh->mBones[i]->mOffsetMatrix;
			m_boneMapping[boneName] = boneIndex;
		}
		else{
			boneIndex = m_boneMapping[boneName];
		}
		for(unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; ++j){
			unsigned short vertexId = pMesh->mBones[i]->mWeights[j].mVertexId;
			float weight = pMesh->mBones[i]->mWeights[j].mWeight;

			subMesh->AddBoneData(vertexId,boneIndex,weight);
		}//for(unsigned int j
	}//for(unsigned int i
}
//--------------------------------------------------------------------------------------
bool Mesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
	// Extract the directory part from the file name
	std::string::size_type SlashIndex = Filename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) {
	    Dir = ".";
	}
	else if (SlashIndex == 0) {
	    Dir = "/";
	}
	else {
	    Dir = Filename.substr(0, SlashIndex);
	}

	bool Ret = true;

	// Initialize the materials
	for (unsigned int i = 0 ; i < pScene->mNumMaterials ; i++) {
	    const aiMaterial* pMaterial = pScene->mMaterials[i];

	    m_textures[i] = NULL;

	    if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
	        aiString Path;

	        if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
	            std::string FullPath = Dir + "/" + Path.data;
	            m_textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

	            if (!m_textures[i]->Load()) {
	                printf("Error loading texture '%s'\n", FullPath.c_str());
	                delete m_textures[i];
	                m_textures[i] = NULL;
	                Ret = false;
	            }
	            else {
	                printf("Loaded texture '%s'\n", FullPath.c_str());
	            }
	        }
	    }

	    // Load a white texture in case the model does not include its own texture
	    if (!m_textures[i]) {
	        m_textures[i] = new Texture(GL_TEXTURE_2D, "./white.png");

	        Ret = m_textures[i]->Load();
	    }
	}
    return Ret;
}
//--------------------------------------------------------------------------------------
void Mesh::Render()
{
	for(unsigned int i=0; i < m_subMeshes.size(); ++i){
		//UpdateMeshEntry(meshEntry);
		//UpdateVertexObject(meshEntry.vertexObject,(float*)&meshEntry.finalVertexVector[0]);
		m_subMeshes[i]->Render();
	}
}
//--------------------------------------------------------------------------------------
void Mesh::RenderUseShader()
{
	for(unsigned int i=0;i < m_subMeshes.size();++i){
		for(size_t j=0; j<m_boneInfo.size();++j){
			SetTranslateMatrix(g_boneTransformLocation[j],&(m_boneInfo[j].m_finalTransformation));
		}
		if(m_numBones>0) SetIntValue(g_hasBonesLocation,1);
		else SetIntValue(g_hasBonesLocation,0);
		
		m_subMeshes[i]->RenderUseShader();
	}
}
///////////////////////////////////// skeleton animation /////////////////////////////////////////////
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
				struct AttachedBone *pBone = subMesh.GetVertexAttachedBones(i);
				finalMatrix += (m_boneInfo[pBone->boneId[j]].m_finalTransformation * pBone->weight[j]);
			}
		}
		else
			finalMatrix.InitIdentity();
		subMesh.CoordTransform(i,finalMatrix);
	}
	return;
}
/*
	@remarks:calculate the bones' transform matrix, result saved in 'm_boneInfo'
*/
void Mesh::BoneTransform(float timeInSeconds)
{
	Matrix4f identity;
	float ticksPerSecond = (float)(mp_scene->mAnimations[0]->mTicksPerSecond!=0 ? mp_scene->mAnimations[0]->mTicksPerSecond : 24.0f);
	float timeInTicks = ticksPerSecond * timeInSeconds;
	float animationTime = fmodf(timeInTicks, (float)mp_scene->mAnimations[0]->mDuration);
	identity.InitIdentity();

	ReadNodeHeirarchy(animationTime,mp_scene->mRootNode,identity);
	return;
}
//--------------------------------------------------------------------------------------
void Mesh::ReadNodeHeirarchy(float animationTime,const aiNode * pNode,const Matrix4f & parentTransform)
{
	std::string nodeName(pNode->mName.data);
	const aiAnimation *pAnimation = mp_scene->mAnimations[0];
	Matrix4f nodeTransformation(pNode->mTransformation);
	const aiNodeAnim *pNodeAnim = FindNodeAnim(pAnimation,nodeName);

	if(pNodeAnim){
		//interpolate scaling and generate scaling transformation matrix
		aiVector3D scaling;
		CalcInterpolatedScaling(scaling,animationTime,pNodeAnim);
		Matrix4f scalingM;
		scalingM.InitScaleTransform(scaling.x,scaling.y,scaling.z);

		//interpolate rotation and generate rotation transformation matrix
		aiQuaternion rotation;
		CalcInterpolatedRotation(rotation,animationTime,pNodeAnim);
		Matrix4f rotationM = Matrix4f(rotation.GetMatrix());

		//interpolate translation and generate translation transformation matrix
		aiVector3D translation;
		CalcInterpolatedPosition(translation,animationTime,pNodeAnim);
		Matrix4f translationM;
		translationM.InitTranslationTransform(translation.x, translation.y, translation.z);

		//combine the above transformations
		nodeTransformation = translationM * rotationM * scalingM;	
	}

	Matrix4f globalTranformation = parentTransform * nodeTransformation;

	if(m_boneMapping.find(nodeName) != m_boneMapping.end()){
		unsigned boneIndex = m_boneMapping[nodeName];
		m_boneInfo[boneIndex].m_finalTransformation = m_globalInverseTransform * globalTranformation * m_boneInfo[boneIndex].m_boneOffset;
	}
	for(unsigned i=0; i < pNode->mNumChildren; ++i){
		ReadNodeHeirarchy(animationTime,pNode->mChildren[i],globalTranformation);
	}
}
//--------------------------------------------------------------------------------------
const aiNodeAnim *Mesh::FindNodeAnim(const aiAnimation * pAnimation,const std::string nodeName)
{
	for(unsigned i=0; i < pAnimation->mNumChannels; ++i){
		const aiNodeAnim *pNodeAnim = pAnimation->mChannels[i];
		if(std::string(pNodeAnim->mNodeName.data) == nodeName){
			return pNodeAnim;
		}
	}
	return NULL;
}
//--------------------------------------------------------------------------------------
void Mesh::CalcInterpolatedPosition(aiVector3D & out,float animationTime,const aiNodeAnim * pNodeAnim)
{
	// we need at least two values to interpolate...
	if(pNodeAnim->mNumPositionKeys == 1){
		out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}
	unsigned positionIndex = FindPosition(animationTime,pNodeAnim);
	unsigned nextPositionIndex = positionIndex + 1;
	float deltaTime = (float)(pNodeAnim->mPositionKeys[nextPositionIndex].mTime - pNodeAnim->mPositionKeys[positionIndex].mTime);
	float factor = (animationTime - (float)pNodeAnim->mPositionKeys[positionIndex].mTime)/deltaTime;
	const aiVector3D &start = pNodeAnim->mPositionKeys[positionIndex].mValue;
	const aiVector3D &end = pNodeAnim->mPositionKeys[nextPositionIndex].mValue;
	aiVector3D delta = end - start;
	out = start + factor * delta;
	return;
}
//--------------------------------------------------------------------------------------
void Mesh::CalcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumRotationKeys == 1) {
        out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }
    
    unsigned rotationIndex = FindRotation(animationTime, pNodeAnim);
    unsigned nextRotationIndex = (rotationIndex + 1);

    float deltaTime = (float)(pNodeAnim->mRotationKeys[nextRotationIndex].mTime - pNodeAnim->mRotationKeys[rotationIndex].mTime);
    float factor = (animationTime - (float)pNodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;
    const aiQuaternion& startRotationQ = pNodeAnim->mRotationKeys[rotationIndex].mValue;
    const aiQuaternion& endRotationQ   = pNodeAnim->mRotationKeys[nextRotationIndex].mValue;    
    aiQuaternion::Interpolate(out, startRotationQ, endRotationQ, factor);
    out = out.Normalize();
	return;
}
//--------------------------------------------------------------------------------------
void Mesh::CalcInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    unsigned scalingIndex = FindScaling(animationTime, pNodeAnim);
    unsigned nextScalingIndex = (scalingIndex + 1);
    float deltaTime = (float)(pNodeAnim->mScalingKeys[nextScalingIndex].mTime - pNodeAnim->mScalingKeys[scalingIndex].mTime);
    float factor = (animationTime - (float)pNodeAnim->mScalingKeys[scalingIndex].mTime) / deltaTime;
    const aiVector3D& start = pNodeAnim->mScalingKeys[scalingIndex].mValue;
    const aiVector3D& end   = pNodeAnim->mScalingKeys[nextScalingIndex].mValue;
    aiVector3D delta = end - start;
    out = start + factor * delta;
	return;
}
//--------------------------------------------------------------------------------------
unsigned Mesh::FindPosition(float animationTime,const aiNodeAnim * pNodeAnim)
{
	for(unsigned i=0; i<pNodeAnim->mNumPositionKeys - 1; ++i){
		if(animationTime < (float)pNodeAnim->mPositionKeys[i+1].mTime){
			return i;
		}
	}
	return 0;
}
//--------------------------------------------------------------------------------------
unsigned Mesh::FindRotation(float animationTime, const aiNodeAnim* pNodeAnim)
{
    for (unsigned i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; ++i) {
        if (animationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }
    return 0;
}
//--------------------------------------------------------------------------------------
unsigned Mesh::FindScaling(float animationTime, const aiNodeAnim* pNodeAnim)
{
    for (unsigned i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; ++i) {
        if (animationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }
    return 0;
}
