#version 450

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 texCoords;
in mat3 TBN;

uniform vec3 cameraPosition;
uniform vec3 uLightPosition;

uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;
uniform float uSpecularIntensity;
uniform float uAOStrength;
uniform bool uUseBlinn;

layout(binding = 1) uniform sampler2D uDiffuseMap;
layout(binding = 2) uniform sampler2D uSpecularMap;
layout(binding = 3) uniform sampler2D uNormalMap;
layout(binding = 4) uniform sampler2D uAOMap;

uniform bool uEnableDiffuseMap;
uniform bool uEnableSpecularMap;
uniform bool uEnableNormalMap;
uniform bool uEnableAOMap;

out vec4 FragColor;

void main()
{
	vec3 N = normalize(fragNormal);

	if (uEnableNormalMap && length(TBN[0]) > 0.01)
	{
		vec3 normTex = texture(uNormalMap, texCoords).rgb * 2.0 - 1.0;
		N = normalize(TBN * normTex);
	}

	vec3 L = normalize(uLightPosition - fragPosition);
	vec3 V = normalize(cameraPosition - fragPosition);
	vec3 H = normalize(L + V); // for Blinn

	vec3 baseColor = uEnableDiffuseMap ? texture(uDiffuseMap, texCoords).rgb : diffuseColor;
	float specMap  = uEnableSpecularMap ? texture(uSpecularMap, texCoords).r : 1.0;
	float ao       = uEnableAOMap ? texture(uAOMap, texCoords).r : 1.0;

	vec3 ambient = ambientColor * baseColor * mix(1.0, ao, uAOStrength);
	vec3 diffuse  = max(dot(N, L), 0.0) * baseColor;
	float spec    = pow(max(dot(N, uUseBlinn ? H : reflect(-L, N)), 0.0), shininess);
	vec3 specular = spec * specularColor * specMap * uSpecularIntensity;

	vec3 finalColor = ambient + diffuse + specular;
	FragColor = vec4(finalColor, 1.0);
}