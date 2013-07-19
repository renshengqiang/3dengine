#version 120

varying vec2 _texCoord;
uniform sampler2D sampler;

void main()
{
	vec4 color = texture2D(sampler, _texCoord.st);
	gl_FragColor = texture2D(sampler, _texCoord.st);
}
