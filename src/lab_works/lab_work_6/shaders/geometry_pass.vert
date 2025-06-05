#version 450
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uMVP;
uniform mat4 uModel;
uniform mat3 uNormalMatrix;

out vec3 fragPos;
out vec3 fragNormal;

void main() {
    fragPos = vec3(uModel * vec4(aPosition, 1.0));
    fragNormal = normalize(uNormalMatrix * aNormal);
    gl_Position = uMVP * vec4(aPosition, 1.0);
}
