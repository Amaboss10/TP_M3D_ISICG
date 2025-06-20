#version 450 

in vec2 texCoords;
out vec4 FragColor;

layout(binding = 0) uniform sampler2D uScene;

void main()
{
	vec2 resolution = textureSize(uScene, 0);
	vec2 texel = 1.0 / resolution;

	vec3 rgbNW = texture(uScene, texCoords + texel * vec2(-1, -1)).rgb;
	vec3 rgbNE = texture(uScene, texCoords + texel * vec2(1, -1)).rgb;
	vec3 rgbSW = texture(uScene, texCoords + texel * vec2(-1, 1)).rgb;
	vec3 rgbSE = texture(uScene, texCoords + texel * vec2(1, 1)).rgb;
	vec3 rgbM  = texture(uScene, texCoords).rgb;

	vec3 luma = vec3(0.299, 0.587, 0.114);
	float lumaM = dot(rgbM, luma);
	float lumaMin = min(lumaM, min(min(dot(rgbNW, luma), dot(rgbNE, luma)), min(dot(rgbSW, luma), dot(rgbSE, luma))));
	float lumaMax = max(lumaM, max(max(dot(rgbNW, luma), dot(rgbNE, luma)), max(dot(rgbSW, luma), dot(rgbSE, luma))));

	if (lumaMax - lumaMin < 0.1)
		FragColor = vec4(rgbM, 1.0);
	else
		FragColor = vec4((rgbNW + rgbNE + rgbSW + rgbSE) * 0.25, 1.0);
}