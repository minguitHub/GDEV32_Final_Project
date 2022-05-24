#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aNorm;

out vec3 fragPos, fragColor, fragNorm;
out vec4 fragPosLightPOV;

uniform mat4 model, view, projection;
uniform mat4 lightView, lightProjection;

void main()
{
	fragPos = vec3(model * vec4(aPos, 1.0f));
	fragNorm = mat3(transpose(inverse(mat3(model)))) * aNorm;
	fragPosLightPOV = lightProjection * lightView * vec4(fragPos, 1.0f);
	
	gl_Position = projection * view * model * vec4(aPos, 1.0f); // gl_Position is predefined output

	fragColor = aColor;
};