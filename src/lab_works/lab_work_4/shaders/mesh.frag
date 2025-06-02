#version 450

layout( location = 0 ) out vec4 fragColor;

uniform vec3 ambientColor, diffuseColor, specularColor;


void main()
{
	fragColor = vec4(ambientColor + diffuseColor, 1.f );
}
