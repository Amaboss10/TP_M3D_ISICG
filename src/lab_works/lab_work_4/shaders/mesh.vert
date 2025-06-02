#version 450

// --- Entrées du vertex shader (provenant du VAO) ---
layout(location = 0) in vec3 aVertexPosition; // Position du sommet (dans l'espace objet)
layout(location = 1) in vec3 aVertexNormal;    // Normale associée au sommet (espace objet)

// --- Sorties vers le fragment shader ---
out vec3 vFragPosView;    // Position du fragment dans l'espace vue (camera space)
out vec3 vNormalView;     // Normale transformée dans l'espace vue
out vec3 vLightPosView;   // Position de la lumière dans l'espace vue
out vec3 vViewDirView;    // Direction vers la caméra (vue inversée)

// --- Uniformes (envoyés depuis l'application) ---
uniform mat4 uMVMatrix;      // Matrice ModelView : transforme objet -> vue
uniform mat4 uMVPMatrix;     // Matrice de projection finale (ModelViewProjection)
uniform mat4 uViewMatrix;    // Matrice de vue seule (caméra)
uniform mat3 uNormalMatrix;  // Matrice pour transformer correctement les normales
uniform vec3 uLightPosition; // Position de la lumière 
uniform vec3 cameraPosition; // Position de la caméra 

void main()
{
    // Calcul de la position projetée finale (clip space)
    gl_Position = uMVPMatrix * vec4(aVertexPosition, 1.0);

    // Calcul de la position du sommet dans l'espace vue
    vec4 fragPosView = uMVMatrix * vec4(aVertexPosition, 1.0);
    vFragPosView = fragPosView.xyz;

    // Transformation de la normale dans l'espace vue (et normalisation)
    vNormalView = normalize(uNormalMatrix * aVertexNormal);

    // Transformation de la position de la lumière 
    vLightPosView = vec3(uViewMatrix * vec4(uLightPosition, 1.0));

    // Calcul de la direction vers la caméra depuis le fragment (caméra à l'origine en vue)
    vViewDirView = normalize(-vFragPosView.xyz);
}

