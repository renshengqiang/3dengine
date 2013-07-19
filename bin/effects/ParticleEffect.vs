#version 120

uniform float time;
uniform mat4 wvpMatrix;

attribute vec3 pos;
attribute vec3 velocity;
attribute vec2 texCoord;

varying vec2 _texCoord;

void main()
{
	vec3 newPos = pos + velocity * time;
	gl_Position = wvpMatrix * vec4(newPos, 1.0f);
	_texCoord = texCoord;
}
