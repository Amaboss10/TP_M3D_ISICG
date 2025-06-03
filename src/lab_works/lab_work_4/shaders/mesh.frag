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
uniform bool useBlinn;

void main()
{
    vec3 N = normalize(vNormalView);
    vec3 L = normalize(vLightPosView - vFragPosView);   // Direction lumière
    vec3 V = normalize(vViewDirView);                   // Direction vue (caméra)

    // Blinn-Phong
	vec3 H = normalize(L + V);
	float specBlinn = pow(max(dot(N, H), 0.0), shininess);

	// Phong
	vec3 R = reflect(-L, N);
	float specPhong = pow(max(dot(R, V), 0.0), shininess);

	// Sélection dynamique
	float spec = useBlinn ? specBlinn : specPhong;

	// Éclairage final
	vec3 ambient = ambientColor;
	vec3 diffuse = max(dot(N, L), 0.0) * difusColor;
	vec3 specular = spec * speculaireColor;

	fragColor = vec4(ambient + diffuse + specular, 1.0);

}
