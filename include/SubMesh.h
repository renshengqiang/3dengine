#ifndef _SUBMESH_H
#define _SUBMESH_H
#include "math_3d.h"
#include "Render.h"
#include "Texture.h"
#include <vector>


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
class SubMesh {
public:
	SubMesh();
	~SubMesh();

	void AddCoord(const Vector3f &coord);
	void AddTexture(Texture *pTexture);
	void AddTextureCoord(const Vector2f &textureCoord);
	void AddIndex(int index);
	void AddBoneData(int index, unsigned short boneId, float weight);
	void Finalize();
	void Render();
	void RenderUseShader();
	unsigned GetVertexNum();
	unsigned GetVertexAttachedBoneNum(unsigned vertexIndex);
	struct AttachedBone * GetVertexAttachedBones(unsigned vertexIndex);
	void CoordTransform(unsigned vertexIndex, const Matrix4f & transMatrix);
	
private:	
	//vertex coord vector
	std::vector<Vector3f> coordVec;
	//a temporary coord vector for CPU based software animation use
	std::vector<Vector3f> finalCoordVec;
	//vertex texture coord vector
	std::vector<Vector2f> textureCoordVec;
	//vertex binded bone Ids vector
	std::vector<struct AttachedBone> attachedBoneVec;
	//每个顶点当前对应的骨骼数量
	std::vector<unsigned> boneNumVec;
	//vertex index vector
	std::vector<int> indexVec;
	//texture
	Texture *mp_texture;

	//该submesh创建的三个Buffer Object
	VERTEX_OBJ *vertexObject;
	INDEX_OBJ *indexObject;
	PIXEL_OBJ *pixelObject;
};

#endif
