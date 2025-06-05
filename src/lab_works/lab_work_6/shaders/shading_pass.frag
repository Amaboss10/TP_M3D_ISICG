#version 450 core

layout(location = 0) out vec4 outColor;

// === Textures du G-Buffer ===
layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gAmbient;
layout(binding = 3) uniform sampler2D gDiffuse;
layout(binding = 4) uniform sampler2D gSpecular;

in vec2 fragUV; 
uniform vec3 uLightPosition;

void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(textureSize(gPosition, 0));

    vec3 fragPos = texture(gPosition, uv).xyz;
    vec3 normal  = normalize(texture(gNormal, uv).xyz);
    vec3 ambient = texture(gAmbient, uv).rgb;
    vec3 diffuse = texture(gDiffuse, uv).rgb;
    vec4 specTex = texture(gSpecular, uv);
    vec3 specular = specTex.rgb;
    float shininess = specTex.a;

    // === Configuration de la lumière ===
    vec3 lightPos = uLightPosition;
    vec3 lightColor = vec3(1.0);
    vec3 viewPos = vec3(0.0, 0.0, 0.0); // la caméra est à l’origine en View Space

    // === Calculs de Blinn-Phong ===
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    vec3 result = ambient + diffuse * diff + specular * spec;
    outColor = vec4(result, 1.0);
}

