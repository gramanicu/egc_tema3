#version 330

// Constants
const float alpha_cutoff = 0.1f;

// Structures
struct Material {
	vec3 emmisive;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

uniform sampler2D texture1;
uniform bool has_texture;

// Values from the vertex shader
in vec3 world_position;	// fragment position
in vec3 world_normal;	// normal vector
in vec2 frag_coord;		// texture coordinate

uniform vec3 eye_position; // or view position

// Uniforms for object properties
uniform Material material;

void main()
{	
	vec3 color = texture(texture1, frag_coord).rgb;

	float brightness = dot(color * material.shininess, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.f) {
		BrightColor = vec4(color * material.shininess, 1.f);
	}
	else {
		BrightColor = vec4(0.f, 0.f, 0.f, 1.f);
	}

	FragColor = vec4(color, 1.0f);
}