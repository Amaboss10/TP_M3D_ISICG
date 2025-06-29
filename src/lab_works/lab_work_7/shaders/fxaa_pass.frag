#version 450

in vec2 vUV;
layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D uSceneTex;
uniform vec2 uInverseScreenSize; // = vec2(1.0 / width, 1.0 / height)

void main()
{
    vec3 rgbNW = texture(uSceneTex, vUV + vec2(-1.0, -1.0) * uInverseScreenSize).rgb;
    vec3 rgbNE = texture(uSceneTex, vUV + vec2( 1.0, -1.0) * uInverseScreenSize).rgb;
    vec3 rgbSW = texture(uSceneTex, vUV + vec2(-1.0,  1.0) * uInverseScreenSize).rgb;
    vec3 rgbSE = texture(uSceneTex, vUV + vec2( 1.0,  1.0) * uInverseScreenSize).rgb;
    vec3 rgbM  = texture(uSceneTex, vUV).rgb;

    vec3 lumaWeights = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, lumaWeights);
    float lumaNE = dot(rgbNE, lumaWeights);
    float lumaSW = dot(rgbSW, lumaWeights);
    float lumaSE = dot(rgbSE, lumaWeights);
    float lumaM  = dot(rgbM,  lumaWeights);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * 0.5, 1.0 / 128.0);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = clamp(dir * rcpDirMin * uInverseScreenSize, -8.0, 8.0);

    vec3 result1 = 0.5 * (
        texture(uSceneTex, vUV + dir * (1.0 / 3.0 - 0.5)).rgb +
        texture(uSceneTex, vUV + dir * (2.0 / 3.0 - 0.5)).rgb
    );

    vec3 result2 = 0.25 * (
        texture(uSceneTex, vUV + dir * -0.5).rgb +
        texture(uSceneTex, vUV + dir *  0.5).rgb
    ) + 0.5 * result1;

    float lumaResult2 = dot(result2, lumaWeights);
    if (lumaResult2 < lumaMin || lumaResult2 > lumaMax)
        FragColor = vec4(result1, 1.0);
    else
        FragColor = vec4(result2, 1.0);
}
