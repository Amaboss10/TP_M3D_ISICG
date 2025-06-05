#version 450

// === Inputs ===
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

// === Uniforms ===
uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uViewMatrix;
uniform mat3 uNormalMatrix;

uniform vec3 cameraPosition;
uniform vec3 uLightPosition;

// === Outputs vers le Fragment Shader ===
out vec3 vFragPos_Tangent;
out vec3 vViewPos_Tangent;
out vec3 vLightPos_Tangent;
out vec2 vTexCoord;

void main()
{
	// Transformations en View space
	vec3 T = normalize(vec3(uMVMatrix * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(uMVMatrix * vec4(aBitangent, 0.0)));
	vec3 N = normalize(uNormalMatrix * aNormal);

	mat3 TBN = transpose(mat3(T, B, N)); // View -> Tangent space

	vec3 fragPos  = vec3(uMVMatrix * vec4(aPosition, 1.0));
	vec3 viewPos  = vec3(uViewMatrix * vec4(cameraPosition, 1.0));
	vec3 lightPos = vec3(uViewMatrix * vec4(uLightPosition, 1.0));

	vFragPos_Tangent  = TBN * fragPos;
	vViewPos_Tangent  = TBN * viewPos;
	vLightPos_Tangent = TBN * lightPos;
	vTexCoord         = aTexCoord;

	gl_Position = uMVPMatrix * vec4(aPosition, 1.0);
}



/*#version 450  bunny

// === Entrées depuis le VAO ===
layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;
layout(location = 2) in vec2 aVertexTexCoords;

// === Sorties vers le fragment shader ===
out vec2 vertexTexCoords;
out vec3 fragNormal;
out vec3 fragPosition;

// === Uniformes ===
uniform mat4 uMVMatrix;        // Model * View
uniform mat4 uMVPMatrix;       // Model * View * Projection
uniform mat3 uNormalMatrix;    // Matrice de transformation des normales

void main()
{
    // Position du vertex dans l'espace vue
    vec4 viewPos = uMVMatrix * vec4(aVertexPosition, 1.0);
    fragPosition = viewPos.xyz;

    // Normale transformée dans l’espace vue
    fragNormal = normalize(uNormalMatrix * aVertexNormal);

    // Coordonnées de texture passées au fragment shader
    vertexTexCoords = aVertexTexCoords;

    // Position projetée (clip space)
    gl_Position = uMVPMatrix * vec4(aVertexPosition, 1.0);
}*/
