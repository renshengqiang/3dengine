#ifndef _SIMPLE_MESH_EFFECT_H
#define _SIMPLE_MESH_EFFECT_H
#include <Effect.h>
#include <math_3d.h>
#include <string>

#define MAX_BONE_NUM 100
class SimpleMeshEffect : public Effect
{
public:
	SimpleMeshEffect(const char *vertexShaderFile, const char *fragmentShaderFile);
	~SimpleMeshEffect();

	virtual bool Init(void);
	
	void EnablePositionAttrib(void);
	void EnableTextureCoordAtrrib(void);
	void EnableBoneIdAttrib(void);
	void EnableBoneWeightAttrib(void);
	void EnableVertexArray(void);
	void DisableVertexArray(void);
	
	void SetWVP(const Matrix4f& WVP);
	void SetTextureUnit(int textureUnit);
	void SetHasBones(bool hasBone);
	void SetBoneTransMatrix(const Matrix4f& trans, unsigned boneId);

	void SetPositionAttribPointer(int coordNum, int stride, int offset);
	void SetTextureCoordAttribPointer(int coordNum, int stride, int offset);
	void SetBoneIdAttribPointer(int coordNum, int stride, int offset);
	void SetBoneWeightAttribPointer(int coordNum, int stride, int offset);	
	
private:
	std::string m_vertexShaderFile, m_fragmentShaderFile;
	GLuint m_positionLocation;
	GLuint m_textureCoordLocation;
	GLuint m_boneIdLocation;
	GLuint m_boneWeightLocation;
	GLuint m_hasBonesLocation;
	GLuint m_WVPLocation;
	GLuint m_boneTransMatrixLocation[MAX_BONE_NUM];
	GLuint m_sampleLocation;
};

#endif
