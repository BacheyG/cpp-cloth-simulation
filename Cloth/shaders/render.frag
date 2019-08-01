#version 430

out vec4 outColor;
in vec3 normalVector;
in vec3 fragPos;
void main()
{
	vec3 lightPos = vec3(0,-2,1);
	vec3 diffuseColor = vec3(0.8, 0.2, 0.2);
	vec3 ambient = vec3(0.4, 0.1, 0.1);
	vec3 lightDir = normalize(lightPos - fragPos); 
	float diff = max(dot(normalVector, lightDir), 0.0);
	vec3 result = ambient + diffuseColor * diff;
	outColor = vec4(result, 1.0);
}