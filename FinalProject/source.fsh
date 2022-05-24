#version 330 core

in vec3 fragPos, fragColor, fragNorm;
in vec4 fragPosLightPOV;

out vec4 FinalColor;

uniform vec3 lightColor, directionalLightPos, directionalLightAmbientIntensity, directionalLightDiffuseIntensity, directionalLightSpecularIntensity;
uniform vec3 eyePos;
uniform sampler2D shadowMapTexture;

void main()
{
	// shadow calculations
	vec3 fragLightNDC = fragPosLightPOV.xyz / fragPosLightPOV.w;
	fragLightNDC = (fragLightNDC.xyz + 1) / 2;
	float closestDepth = texture(shadowMapTexture, fragLightNDC.xy).r;
	float currentDepth = fragLightNDC.z;
	vec3 norm = normalize(fragNorm);
	vec3 directionalLightDir = normalize(directionalLightPos - fragPos);
	float shadowBias = max(0.1f * (1.0f - dot(norm, directionalLightDir)), 0.005f);
	bool isShadowed = closestDepth < currentDepth - shadowBias;

	// PCF (idk why it won't work :<)
	float shadowing = 0.0f;
	vec2 texelSize = 1.0f / textureSize(shadowMapTexture, 0);
	for(int x = -2; x <= 1.0f; ++x)
	{
		for(int y = -2; y <= 1.0f; ++y)
		{
			float PCFDepth = texture(shadowMapTexture, fragLightNDC.xy + vec2(x, y) * texelSize).r;
			shadowing += PCFDepth < currentDepth - shadowBias ? 1.0f : 0.0f;
		}
	}
	shadowing /= 9.0f;

	if(fragLightNDC.z > 1.0f) shadowing = 0.0f;

	if (!isShadowed)
	{
		// ambient lighting
		vec3 directionalLightAmbient = lightColor * directionalLightAmbientIntensity;

		// diffuse lighting
		// vec3 norm = normalize(fragNorm); // moved to shadow calculations
		// vec3 directionalLightDir = normalize(directionalLightPos - fragPos); // moved to shadow calculations
		float diff = max(dot(norm, directionalLightDir), 0.0);
		vec3 directionalLightDiffuse = diff * lightColor * directionalLightDiffuseIntensity;

		// specular lighting
		vec3 eyeDir = normalize(eyePos - fragPos);
		vec3 reflectDir = reflect(-directionalLightDir, norm);
		float spec = pow(max(dot(eyeDir, reflectDir), 0.0f), 128.0f);
		vec3 directionalLightSpecular = spec * lightColor * directionalLightSpecularIntensity;

		vec3 phongLightingColor = (directionalLightAmbient + (1.0 - shadowing)  * (directionalLightDiffuse + directionalLightSpecular)) * fragColor;
		//vec3 phongLightingColor = (directionalLightAmbient + directionalLightDiffuse + directionalLightSpecular) * fragColor;

		FinalColor = vec4(phongLightingColor, 1.0f);
	}
	else
	{
		FinalColor = vec4(0.0f);
	}
};