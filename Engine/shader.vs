#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 texPos;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 vp;
uniform mat4 model;

void main()
{
	FragPos = (model * vec4(aPos, 1)).xyz;
	Normal = (inverse(transpose(model)) * vec4(norm, 1)).xyz;
	TexCoord = texPos;
	gl_Position = vp * vec4(FragPos, 1);
}