#version 330

uniform mat4 transformation;

layout(location = 0) in vec3 position;

out vec2 texCoord;

void main()
{
	gl_Position = transformation * vec4(position, 1.0);
	texCoord =  vec2(1.0, 1.0) - (position.xy + vec2(1.0, 1.0)) * 0.5;
}