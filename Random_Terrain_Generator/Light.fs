#version 430

out vec4 frag_color;

uniform mat4 mvp_matrix;

in vec2 tex;
in vec3 varying_normal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;
in vec3 varyingHalfVector;

layout (binding = 0) uniform sampler2D s;

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
	vec4 color = texture(s, tex);

	vec3 L = varyingLightDir;
	vec3 N = normalize(varying_normal);
	vec3 H = varyingHalfVector;

	
	frag_color = color * ( light.ambient + light.diffuse * max(dot(L, N), 0.0f))
	+ light.specular * pow(max(dot(H, N) * 0.9f, 0.0f), 30.0f); //pow(max(dot(H, N), 0.0f), 2000.0f);
}