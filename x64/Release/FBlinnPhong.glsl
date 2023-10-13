#version 330

uniform mat4 View;
uniform sampler2D edgeTexture;
uniform sampler2D normalMap;
uniform samplerCube environmentMap;	

in vec3 Position;
in vec3 Normal;
in vec3 Tangent;
in vec3 Binormal;
in vec2 UV;
in vec3 Color;

out vec4 FragColor;

const float roughness = 0.7;

const vec3 lightColor = vec3(1.0, 1.0,1.0);
const vec3 light_0 = vec3(4, 4, 0);
const vec3 light_1 = vec3(-5, -4, 0);
const vec3 light_2 = vec3(0, 10, -16);

const float alphaX = 1;
const float alphaY = 0.1;

const float specularIntensity = 0.2;
const float diffuseIntensity = 0.8;

vec3 computeBlinnPhong(vec3 N, vec3 T, vec3 L, vec3 V, vec3 diffuseColor, vec3 lightColor, float roughness)
{
	vec3 B = normalize(cross(N, T));
	vec3 H = normalize(V+L);
	float NdotL = max(0.0, dot(N,L));
	float NdotH = dot(N, H);
	float HdotT = dot(H, T);
	float HdotB = dot(H, B);

	float specDistrib = 1.0 - (HdotT * HdotT) / alphaX - (HdotB * HdotB) / alphaY;
	specDistrib = max(0.0, specDistrib);
	specDistrib = pow(specDistrib, roughness);

	vec3 specularTerm = vec3(specularIntensity * NdotL * specDistrib * lightColor);
	vec3 diffuseTerm = diffuseIntensity * NdotL * diffuseColor * lightColor;
	return diffuseTerm + specularTerm;
}

void main()
{
	vec3 normalMap = 2 * texture(normalMap, UV).xyz - 1;
	vec3 N = normalMap.r * Tangent + normalMap.g * Binormal + normalMap.b * Normal;
	vec3 V = normalize(-Position);
	vec3 T = Tangent;
	vec3 R = -reflect(V, N);			

	vec3 L0 = normalize(light_0 - Position);
	vec3 L1 = normalize(light_1 - Position);
	vec3 L2 = normalize(light_2 - Position);

	vec3 diffuse = texture(edgeTexture, UV).xyz * Color;
	vec3 reflection = texture(environmentMap, R).xyz;
		
	vec3 finalColor = vec3(0.0);
	finalColor += computeBlinnPhong(N, T, L0, V, diffuse, lightColor, roughness);
	finalColor += computeBlinnPhong(N, T, L1, V, diffuse, lightColor, roughness);
	finalColor += computeBlinnPhong(N, T, L2, V, diffuse, lightColor, roughness);
	finalColor += reflection * (1.0 - roughness);
					  
	FragColor = vec4(finalColor, 1.0);
}


