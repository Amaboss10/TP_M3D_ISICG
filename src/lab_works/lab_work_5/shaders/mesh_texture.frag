#version 450

// === Inputs ===
in vec3 vFragPos_Tangent;
in vec3 vViewPos_Tangent;
in vec3 vLightPos_Tangent;
in vec2 vTexCoord;

// === Output ===
out vec4 fragColor;

// === Textures (fixed bindings) ===
layout(binding = 1) uniform sampler2D uDiffuseMap;
layout(binding = 2) uniform sampler2D uAmbientMap;
layout(binding = 3) uniform sampler2D uSpecularMap;
layout(binding = 4) uniform sampler2D uShininessMap;
layout(binding = 5) uniform sampler2D uNormalMap;

// === Texture presence flags ===
uniform bool uHasDiffuseMap;
uniform bool uHasAmbientMap;
uniform bool uHasSpecularMap;
uniform bool uHasShininessMap;
uniform bool uHasNormalMap;

// === User-controlled toggles ===
uniform bool uEnableDiffuseMap;
uniform bool uEnableSpecularMap;
uniform bool uEnableNormalMap;
uniform float uSpecularIntensity;

// === Material ===
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;
uniform bool useBlinn;

// === Fonction pour la normale ===
vec3 getNormal()
{
	if ( uHasNormalMap && uEnableNormalMap )
	{
		vec3 normal = texture(uNormalMap, vTexCoord).rgb;
		return normalize(normal * 2.0 - 1.0);
	}
	else
	{
		return vec3(0.0, 0.0, 1.0); // Normale par défaut en Tangent Space
	}
}

void main()
{

	if ( uHasDiffuseMap )
	{
		float alpha = texture(uDiffuseMap, vTexCoord).a;
		if ( alpha < 0.5 ) discard;
	}


	vec3 N = getNormal();
	vec3 L = normalize(vLightPos_Tangent - vFragPos_Tangent);
	vec3 V = normalize(vViewPos_Tangent - vFragPos_Tangent);
	vec3 R = reflect(-L, N);

	// === Valeurs de matériaux
	vec3 ambient = (uHasAmbientMap ? texture(uAmbientMap, vTexCoord).rgb : ambientColor);

	vec3 diffuseCol = (uHasDiffuseMap && uEnableDiffuseMap)
		? texture(uDiffuseMap, vTexCoord).rgb
		: diffuseColor;

	vec3 specularCol = (uHasSpecularMap && uEnableSpecularMap)
		? texture(uSpecularMap, vTexCoord).rrr
		: specularColor;

	float shin = (uHasShininessMap ? texture(uShininessMap, vTexCoord).r : shininess);
	shin = clamp(shin, 1.0, 128.0);

	// === Calcul d’éclairage
	float diff = max(dot(N, L), 0.0);
	float spec = 0.0;

	if ( diff > 0.0 )
	{
		if ( useBlinn )
		{
			vec3 H = normalize(L + V);
			spec = pow(max(dot(N, H), 0.0), shin);
		}
		else
		{
			spec = pow(max(dot(R, V), 0.0), shin);
		}
	}

	vec3 finalSpecular = specularCol * spec * uSpecularIntensity;

	vec3 finalColor = ambient + diffuseCol * diff + finalSpecular;

	// Correction gamma
	finalColor = pow(finalColor, vec3(1.0 / 2.2));

	float alpha = 1.0;
	if ( uHasDiffuseMap )
		alpha = texture(uDiffuseMap, vTexCoord).a;

	fragColor = vec4(finalColor, alpha);

}



/*
#version 450

layout(location = 0) out vec4 fragColor;

// === Entrées depuis le vertex shader ===
in vec3 fragPosition;
in vec3 fragNormal;
in vec2 vertexTexCoords;

// === Uniformes ===
uniform bool uHasDiffuseMap;
layout(binding = 1) uniform sampler2D uDiffuseMap;

uniform bool uHasSpecularMap;
layout(binding = 2) uniform sampler2D uSpecularMap;

uniform bool uHasShininessMap;
layout(binding = 3) uniform sampler2D uShininessMap;

uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;

uniform vec3 lightPos;        // En espace vue
uniform vec3 cameraPosition;  // En espace vue
uniform bool useBlinn;

void main()
{
    vec3 N = normalize(fragNormal);
    vec3 L = normalize(lightPos - fragPosition);
    vec3 V = normalize(cameraPosition - fragPosition);
    vec3 H = normalize(L + V);
    vec3 R = reflect(-L, N);

    float diff = max(dot(N, L), 0.0);
    float spec = 0.0;

    // Choix shininess
    float phongExp = uHasShininessMap ? texture(uShininessMap, vertexTexCoords).x : shininess;

    if (diff > 0.0)
    {
        if (useBlinn)
            spec = pow(max(dot(N, H), 0.0), phongExp);
        else
            spec = pow(max(dot(R, V), 0.0), phongExp);
    }

    vec3 kd = uHasDiffuseMap   ? texture(uDiffuseMap, vertexTexCoords).rgb   : diffuseColor;
    vec3 ks = uHasSpecularMap  ? texture(uSpecularMap, vertexTexCoords).xxx  : specularColor;

    vec3 finalColor = ambientColor * kd + diff * kd + spec * ks;
    fragColor = vec4(finalColor, 1.0);
}*/
