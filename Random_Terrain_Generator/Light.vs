#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 1) in vec2 texCoord;

uniform mat4 mv_matrix;
uniform mat4 mvp_matrix;
uniform mat4 normal_matrix;

out vec2 tex;
out vec3 varying_normal;
out vec3 varyingLightDir;
out vec3 varyingVertPos;
out vec3 varyingHalfVector;

struct Light
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
};

uniform Light light;

void main(void)
{
	varyingVertPos = (mv_matrix * vec4(position, 1.0f)).xyz;

	gl_Position = mvp_matrix * vec4(position, 1.0);
	tex = texCoord;

	varying_normal = (normal_matrix * vec4(normal, 1.0f)).xyz;
	
	vec3 light_pos = light.position;

	varyingLightDir = normalize(light_pos - varyingVertPos);	
	varyingHalfVector = normalize(varyingLightDir - varyingVertPos).xyz;
}