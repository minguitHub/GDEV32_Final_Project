#version 330 core

in vec3 fragPos, fragColor, fragNorm;
in vec4 fragPosLightPOV;

out vec4 FinalColor;

uniform vec3 lightColor;
uniform vec3 pointLightPos, pointLightAmbientIntensity, pointLightDiffuseIntensity, pointLightSpecularIntensity;
uniform float pointLightConstant, pointLightLinear, pointLightQuadratic, pointLightDistance;
uniform vec3 directionalLightPos, directionalLightAmbientIntensity, directionalLightDiffuseIntensity, directionalLightSpecularIntensity;
uniform vec3 eyePos;
uniform sampler2D shadowMapTexture;
uniform float time;

void main()
{
	// shadow calculations
	vec3 fragLightNDC = fragPosLightPOV.xyz / fragPosLightPOV.w;
	fragLightNDC = (fragLightNDC.xyz + 1) / 2;
	float closestDepth = texture(shadowMapTexture, fragLightNDC.xy).r;
	float currentDepth = fragLightNDC.z;
	vec3 norm = normalize(fragNorm);
	vec3 directionalLightDir = normalize(directionalLightPos - fragPos);
	float shadowBias = max(0.1f * (1.0f - dot(norm, directionalLightDir)), 0.0075f);
	bool isShadowed = closestDepth < currentDepth - shadowBias;

	// PCF
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
		vec3 directionalChangingLightColor = lightColor * abs(time);
		vec3 pointLightChangingLightColor = lightColor * (1 - abs(time));

		// point light
		// ambient lighting
			vec3 pointLightAmbient = pointLightChangingLightColor * pointLightAmbientIntensity;

			// diffuse lighting
			vec3 pointNorm = normalize(fragNorm);
			vec3 pointLightDir = normalize(pointLightPos - fragPos);
			float pointLightDiff = max(dot(pointNorm, pointLightDir), 0.0);
			vec3 pointLightDiffuse = pointLightDiff * pointLightChangingLightColor * pointLightDiffuseIntensity;

			// specular lighting
			vec3 pointEyeDir = normalize(eyePos - fragPos);
			vec3 pointLightReflectDir = reflect(-pointLightDir, norm);
			float pointLightSpec = pow(max(dot(pointEyeDir, pointLightReflectDir), 0.0f), 128.0f);
			vec3 pointLightSpecular = 1.0f * pointLightSpec * pointLightChangingLightColor * pointLightSpecularIntensity;

			// attenuation
			float pointLightAttenuation =  1 / (pointLightConstant + (pointLightLinear * pointLightDistance) + (pointLightQuadratic * pointLightDistance * pointLightDistance));

		// directional light
			// ambient lighting
			vec3 directionalLightAmbient = directionalChangingLightColor * directionalLightAmbientIntensity;

			// diffuse lighting
			vec3 directionalNorm = normalize(fragNorm); 
			vec3 directionalLightDir = normalize(directionalLightPos - fragPos);
			float directionalDiff = max(dot(directionalNorm, directionalLightDir), 0.0);
			vec3 directionalLightDiffuse = directionalDiff * directionalChangingLightColor * directionalLightDiffuseIntensity;

			// specular lighting
			vec3 directionalEyeDir = normalize(eyePos - fragPos);
			vec3 directionalReflectDir = reflect(-directionalLightDir, directionalNorm);
			float directionalSpec = pow(max(dot(directionalEyeDir, directionalReflectDir), 0.0f), 128.0f);
			vec3 directionalLightSpecular = directionalSpec * directionalChangingLightColor * directionalLightSpecularIntensity;

		vec3 pointLightPhongLightingColor = (pointLightAmbient + pointLightDiffuse + pointLightSpecular);
		vec3 directionalPhongLightingColor = directionalLightAmbient + directionalLightDiffuse + directionalLightSpecular;
		vec3 phongLightingColor = (pointLightPhongLightingColor + directionalPhongLightingColor) * fragColor;
		phongLightingColor = (1.0 - shadowing) * phongLightingColor;

		FinalColor = vec4(phongLightingColor, 1.0f);
	}
	else
	{
		FinalColor = vec4(0.0f);
	}
};