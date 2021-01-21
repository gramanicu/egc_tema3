#version 330

// Structures
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform sampler2D texture1;
uniform bool has_texture;

// Get color value from vertex shader
in vec3 world_position;	// fragment position
in vec3 world_normal;	// normal vector
in vec2 frag_coord;		// texture coordinate

// Uniforms for light properties
uniform vec3 light_position;
uniform vec3 light_direction;
uniform vec3 eye_position; // or view position

uniform float material_kd;
uniform float material_ks;
uniform float material_shininess;

uniform vec3 object_color;	// the objects color

out vec4 out_color;

// Constants
float cutoff = 0.1f;
float ambient_factor = 0.75f;
vec3 specular_color = vec3(0.3);

float constant = 1.0f;
float linear = 0.0025f;
float quadratic = 0.0000125f;

float light_cutoff = 0.35f;
float light_out_cutoff = 0.32f;

void main()
{	
	vec4 color_rgba;
	if(has_texture) {
		color_rgba = texture(texture1, frag_coord).rgba;
	} else {
		color_rgba = vec4(object_color, 1.f);
	}

	// Invisible parts in the texture will have the objects color
	if (color_rgba.a < cutoff)
	{
		color_rgba = vec4(object_color, 1.f);
	}

	vec3 color = color_rgba.rgb;

	// - START LIGHTING COMPUTATION - 
	// Ambient
	vec3 ambient_light = ambient_factor * color;

	// Diffuse
	vec3 lightDir = normalize(light_position - world_position);		// L
	vec3 normal = normalize(world_normal);							// N (or view direction)
	float diffuse_value = max(dot(lightDir, normal), 0.f);
	vec3 diffuse_light = (diffuse_value * material_kd) * color;

	// Specular
	vec3 viewDir = normalize(eye_position - world_position);	// V
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	float specular_value = pow(max(dot(normal, halfwayDir), 0.0), material_shininess);
	vec3 specular_light = (specular_value * material_ks) * specular_color ;
	
	// Spotlight (soft edges)
	float theta = dot(lightDir, normalize(-light_direction));
	float epsilon = (light_cutoff - light_out_cutoff);
	float intensity = clamp((theta - light_out_cutoff) / epsilon, 0.f, 1.f);
	diffuse_light *= intensity;
	specular_light *= intensity;
	// - END LIGHTING COMPUTATION -

	// Compute final light value
	float d	= distance(light_position, world_position);
	float attenuation = 1.0f / (constant + linear * d + quadratic * (d * d));

	ambient_light *= attenuation;
	diffuse_light *= attenuation;
	specular_light *= attenuation;

	vec4 light = vec4(ambient_light + diffuse_light + specular_light, 1.0f);
	out_color = light;
	

}