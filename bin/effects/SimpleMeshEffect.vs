#version 120
const int MAX_BONE_NUM = 100;

attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord;
attribute vec4 boneIds;
attribute vec4 weights;
uniform bool hasBones;

uniform mat4 PVMMatrix;
uniform mat4 boneTransform[MAX_BONE_NUM];

varying vec2 texCoord;
void main()
{
	mat4 boneTrans = mat4(1.0);
	if(hasBones)
	{
		boneTrans = boneTransform[int(boneIds[0])] * weights[0];
		boneTrans += boneTransform[int(boneIds[1])] * weights[1];
		boneTrans += boneTransform[int(boneIds[2])] * weights[2];
		boneTrans += boneTransform[int(boneIds[3])] * weights[3];
	}
	gl_Position = PVMMatrix*boneTrans*vec4(vertexPosition, 1.0f);
	texCoord = vertexTexCoord;
}
