#include "SubMesh.h"

SubMesh::SubMesh()
{
	vertexObject = NULL;
	indexObject = NULL;
};
//--------------------------------------------------------------------------------------
SubMesh::~SubMesh()
{
	DestroyVertexObject(vertexObject);
	DestroyIndexObject(indexObject);
}
//--------------------------------------------------------------------------------------
void SubMesh::AddCoord(const Vector3f &coord)
{
	struct AttachedBone attachedBone;
	coordVec.push_back(coord);
	finalCoordVec.push_back(Vector3f());
	attachedBoneVec.push_back(attachedBone);
	boneNumVec.push_back(0);
	return;
}
//--------------------------------------------------------------------------------------
void SubMesh::SetTexture(Texture *pTexture)
{
	//pixelObject = CreatePixelObject2(texture,GL_TEXTURE0,GL_TEXTURE_2D);
	mp_texture = pTexture;
}
//--------------------------------------------------------------------------------------
void SubMesh::AddTextureCoord(const Vector2f &textureCoord)
{
	textureCoordVec.push_back(textureCoord);
	return;
}
//--------------------------------------------------------------------------------------
void SubMesh::AddIndex(int index)
{
	indexVec.push_back(index);
}
//--------------------------------------------------------------------------------------
void SubMesh::AddBoneData(int index, unsigned short boneId, float weight)
{
	int n = boneNumVec[index];

	attachedBoneVec[index].boneId[n] = boneId;
	attachedBoneVec[index].weight[n] = weight;
	return;
}
//--------------------------------------------------------------------------------------
void SubMesh::Finalize()
{
	//vertexObject = CreateVertexObject(COORD_3|TEXTURE_2|NORMAL_3, Vertices.size(), (float *)&Vertices[0]);
	vertexObject = CreateVertexObject(COORD_3|TEXTURE_2|BONE_4,coordVec.size());
	/*
	for(size_t i=0;i<vertexVector.size();++i){
		VertexObjectPushElement(vertexObject, COORD_3, (float *)&vertexVector[i].m_pos);
		VertexObjectPushElement(vertexObject, TEXTURE_2, (float *)&vertexVector[i].m_tex);
		//VertexObjectPushElement(vertexObject, NORMAL_3, (float *)&vertexVector[i].m_normal);
	}
	*/
	VertexObjectPushElementAll(vertexObject, COORD_3, (float*)&coordVec[0]);
	VertexObjectPushElementAll(vertexObject, TEXTURE_2, (float*)&textureCoordVec[0]);
	VertexObjectPushElementAll(vertexObject, BONE_4, (float *)&attachedBoneVec[0]);
	VertexObjectEnd(vertexObject);
	
	indexObject = CreateIndexObject(indexVec.size(),(unsigned int *)&indexVec[0]);
	pixelObject = CreatePixelObject2(mp_texture->GetTextureObj(),GL_TEXTURE0,GL_TEXTURE_2D);
}
//--------------------------------------------------------------------------------------
unsigned SubMesh::GetVertexNum()
{
	return coordVec.size();
}
//--------------------------------------------------------------------------------------
unsigned SubMesh::GetVertexAttachedBoneNum(unsigned vertexIndex)
{
	return boneNumVec[vertexIndex];
}
//--------------------------------------------------------------------------------------
struct AttachedBone *SubMesh::GetVertexAttachedBones(unsigned vertexIndex)
{
	return &(attachedBoneVec[vertexIndex]);
}
//--------------------------------------------------------------------------------------
void SubMesh::CoordTransform(unsigned vertexIndex, const Matrix4f & transMatrix)
{
	finalCoordVec[vertexIndex] = transMatrix * coordVec[vertexIndex];
	return;
}
//--------------------------------------------------------------------------------------
/*
void SubMesh::Render()
{
	DrawObject(0,indexObject,vertexObject,pixelObject,NULL);	
}
*/
//--------------------------------------------------------------------------------------
void SubMesh::RenderUseShader(SimpleMeshEffect& effect)
{
	DrawOjectUseShader(indexObject,  vertexObject, pixelObject, effect);
}
