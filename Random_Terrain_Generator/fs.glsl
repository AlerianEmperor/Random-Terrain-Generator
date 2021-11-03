#version 430

out vec4 color;

uniform mat4 mvp_matrix;

in vec2 tex;

layout (binding = 0) uniform sampler2D s;

void main(void)
{
	color = texture(s, tex);
}