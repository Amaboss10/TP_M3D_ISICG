#version 450 

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 uMVMatrix;
uniform mat4 uMVPMatrix;
uniform mat3 uNormalMatrix;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 texCoords;
out mat3 TBN;

void main()
{
	gl_Position   = uMVPMatrix * vec4(aPosition, 1.0);
	vec4 viewPos  = uMVMatrix * vec4(aPosition, 1.0);
	fragPosition  = viewPos.xyz;
	fragNormal    = normalize(uNormalMatrix * aNormal);
	texCoords     = aTexCoords;

	vec3 T = normalize(uNormalMatrix * aTangent);
	vec3 B = normalize(uNormalMatrix * aBitangent);
	vec3 N = fragNormal;
	TBN = mat3(T, B, N);
}
