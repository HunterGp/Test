#version 330

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

layout(location = 0) in vec3 InPos;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec3 InTangent;
layout(location = 3) in vec2 InUV;
layout(location = 4) in vec3 InColor;

out vec3 Position;
out vec3 Normal;
out vec3 Tangent;
out vec3 Binormal;
out vec2 UV;
out vec3 Color;

void main()
{
	gl_Position = Projection * View * Model * vec4(InPos, 1.0);
	Position = vec3(View * Model * vec4(InPos, 1.0));
	Normal = normalize(mat3(Model) * InNormal);
	Tangent = normalize(mat3(Model) * InTangent);
	Binormal = normalize(cross(Normal, Tangent));
	UV = InUV;
	Color = InColor;
}