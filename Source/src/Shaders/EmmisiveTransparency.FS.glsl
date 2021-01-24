#version 330

// Constants
const int max_light_sources = 64;
const int Directional = 0;
const int Point = 1;
const int Spot = 2;
const float alpha_cutoff = 0.1f;

// Structures
struct Material {
	vec3 emmisive;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
struct Light {
	// Light properties
	int type;			// Directional, Point or Spot 
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// Light attenuation
	float constant;
    float linear;
    float quadratic;

	// Spot light cutoff
    float cutOff;
    float outerCutOff;
};

uniform sampler2D texture1;
uniform bool has_texture;

// Values from the vertex shader
in vec3 world_position;	// fragment position
in vec3 world_normal;	// normal vector
in vec2 frag_coord;		// texture coordinate

uniform vec3 eye_position; // or view position

// Uniforms for light properties
uniform int lights_count;
uniform Light lights[max_light_sources];

// Uniforms for object properties
uniform Material material;

out vec4 out_color;

vec3 compute_lighting(Light light, Material material, vec3 color, vec3 normal, vec3 viewDir) {
	// Direction of the light
	vec3 lightDir;	// L
	if(light.type == Directional) {
		lightDir = normalize(-light.direction);
	} else {
		lightDir = normalize(light.position - world_position);
	}

	// Ambient
	vec3 ambient_light = light.ambient * color;

	// Diffuse
	float diffuse_value = max(dot(lightDir, normal), 0.f);
	vec3 diffuse_light = (diffuse_value * light.diffuse) * color;
	//vec3 diffuse_light = (diffuse_value * light.diffuse) * material.diffuse;
	
	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float specular_value = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	vec3 specular_light = (specular_value * light.specular);

	// Spotlight (soft edges)
	if(light.type == Spot) {
		float theta = dot(lightDir, normalize(-light.direction));
		float epsilon = light.cutOff - light.outerCutOff;
		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.f, 1.f);

		diffuse_light *= intensity;
		specular_light *= intensity;
	}
	
	// Attenuation
	if(light.type != Directional) {
		float d	= distance(light.position, world_position);
		float attenuation = 1.0f / (light.constant + light.linear * d + light.quadratic * (d * d));

		diffuse_light *= attenuation;
		specular_light *= attenuation;
	}
	
	return (ambient_light + diffuse_light + specular_light);
}

void main()
{	
	vec4 color_rgba;
	if(has_texture) {
		color_rgba = texture(texture1, frag_coord).rgba;
	} else {
		color_rgba = vec4(material.ambient, 1.f);
	}

	// Invisible parts in the texture will be transparent
	if (color_rgba.a < alpha_cutoff)
	{
//		discard;
		color_rgba = vec4(material.emmisive, 1.f);
	}

	vec3 color = color_rgba.rgb;

	vec3 normal = normalize(world_normal);	// N (or view direction)
	vec3 viewDir = normalize(eye_position - world_position);	// V

	vec3 result = vec3(0);
	for(int i = 0; i < lights_count; ++i) {
		result += compute_lighting(lights[i], material, color, normal, viewDir);
	}
	out_color = vec4(result, 1.0f);
}