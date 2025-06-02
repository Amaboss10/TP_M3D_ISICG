#version 450

// --- Sortie finale du fragment shader ---
layout(location = 0) out vec4 fragColor;


// --- Variables interpolées depuis le vertex shader ---
in vec3 vFragPosView;   // Position du fragment 
in vec3 vNormalView;    // Normale du fragment 
in vec3 vLightPosView;  // Position de la lumière
in vec3 vViewDirView;   // Direction de vue (depuis fragment vers caméra)

// --- Uniformes ---
uniform vec3 ambientColor;     // Couleur ambiante (constante, non directionnelle)
uniform vec3 difusColor;       // Couleur diffuse (Lambert)
uniform vec3 speculaireColor;  // Couleur spéculaire (Blinn-Phong)
uniform float shininess;       // Brillance (concentration du reflet)

void main()
{
    vec3 N = normalize(vNormalView);
    vec3 L = normalize(vLightPosView - vFragPosView);   // Direction lumière
    vec3 V = normalize(vViewDirView);                   // Direction vue (caméra)
    vec3 H = normalize(L + V);                          // Blinn-Phong

    float diff = max(dot(N, L), 0.0);                   // Composante diffuse
    float spec = pow(max(dot(N, H), 0.0), shininess);   // Composante spéculaire Blinn

    vec3 ambient = ambientColor;
    vec3 diffuse = diff * difusColor;
    vec3 specular = spec * speculaireColor;

    vec3 finalColor = ambient + diffuse + specular;
    fragColor = vec4(finalColor, 1.0);
}
