#version 330

uniform sampler2D texture1;
uniform bool has_texture;

in vec2 frag_coord;

out vec4 out_color;

uniform vec3 object_color;

// Constants
float cutoff = 0.1;

void main()
{
	vec4 texturedColor = texture(texture1, frag_coord);

	if(has_texture) {
		out_color = texturedColor;
	} else {
		out_color = vec4(object_color, 1.f);
	}

	if (out_color.a < cutoff)
	{
		discard;
	}
}