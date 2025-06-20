#version 450 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAmbient;
layout(location = 3) out vec4 gDiffuse;
layout(location = 4) out vec4 gSpecular;

in vec3 fragPos;     // depuis le VS
in vec3 fragNormal;  // normal interpolée

void main()
{
    // === Paramètres de matériau ===
    vec3 ambientColor  = vec3(0.1);
    vec3 diffuseColor  = vec3(0.8, 0.5, 0.3);
    vec3 specularColor = vec3(1.0);
    float shininess    = 32.0;

    // === G-Buffer outputs ===
    gPosition  = vec4(fragPos, 1.0);
    gNormal    = vec4(normalize(fragNormal), 0.0);
    gAmbient   = vec4(ambientColor, 1.0);
    gDiffuse   = vec4(diffuseColor, 1.0);
    gSpecular  = vec4(specularColor, shininess); 
}
