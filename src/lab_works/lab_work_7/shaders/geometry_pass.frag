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
uniform bool uEnableShadow;

layout(binding = 1) uniform sampler2D uDiffuseMap;
layout(binding = 2) uniform sampler2D uSpecularMap;
layout(binding = 3) uniform sampler2D uNormalMap;
layout(binding = 4) uniform sampler2D uAOMap;

// Nouveaux uniformes pour les ombres
layout(binding = 5) uniform sampler2D uShadowMap;
uniform mat4 uLightViewProj;
uniform float uShadowBias;

uniform bool uEnableDiffuseMap;
uniform bool uEnableSpecularMap;
uniform bool uEnableNormalMap;
uniform bool uEnableAOMap;

out vec4 FragColor;

//  Fonction de shadow mapping
float computeShadow(vec3 fragPosWorld)
{
	vec4 lightSpacePos = uLightViewProj * vec4(fragPosWorld, 1.0);
	vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
	projCoords = projCoords * 0.5 + 0.5;

	// Ignore les fragments hors de la shadow map
	if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
		return 1.0;

	float closestDepth = texture(uShadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;

	// Si fragment derrière un objet => ombré
	return (currentDepth - uShadowBias > closestDepth) ? 0.3 : 1.0;
}

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
	vec3 H = normalize(L + V); // pour Blinn

	vec3 baseColor = uEnableDiffuseMap ? texture(uDiffuseMap, texCoords).rgb : diffuseColor;
	float specMap  = uEnableSpecularMap ? texture(uSpecularMap, texCoords).r : 1.0;
	float ao       = uEnableAOMap ? texture(uAOMap, texCoords).r : 1.0;

	vec3 ambient = ambientColor * baseColor * mix(1.0, ao, uAOStrength);
	vec3 diffuse = max(dot(N, L), 0.0) * baseColor;
	float spec   = pow(max(dot(N, uUseBlinn ? H : reflect(-L, N)), 0.0), shininess);
	vec3 specular = spec * specularColor * specMap * uSpecularIntensity;

	//  Calcul du facteur d’ombre
	float shadowFactor = uEnableShadow ? computeShadow(fragPosition) : 1.0;

	vec3 finalColor = ambient + shadowFactor * (diffuse + specular);
	FragColor = vec4(finalColor, 1.0);
}
