layout (location = 0) in vec3 aPosition;

uniform mat4 uModelMatrix;
uniform mat4 uLightViewProj; // light projection * light view

void main()
{
	gl_Position = uLightViewProj * uModelMatrix * vec4(aPosition, 1.0);
}