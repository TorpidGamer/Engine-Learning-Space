#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 cameraPos;

// textures
uniform sampler2D diffuseTex1;
uniform sampler2D specTex1;
uniform int texNum = 0;

void main()
{
	vec3 tex = texture(diffuseTex1, TexCoord).rgb;
	vec3 ambCol = vec3(0.05) * tex;

	vec3 lightDirection = normalize(lightPos - FragPos);
	vec3 normal = normalize(Normal);
	float diff = max(dot(lightDirection, normal), 0.f);
	vec3 diffuse = diff * tex;

	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDirection, normal);
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDirection + viewDir);
	spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0);
	vec3 specular = vec3(0.5f) * spec;

	FragColor = vec4(ambCol + diffuse + specular, 1);
}