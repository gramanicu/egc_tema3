#version 330

uniform sampler2D texture1;
uniform bool has_texture;
in vec2 frag_coord;		// texture coordinate

out vec4 out_color;

void main()
{
	vec4 color_rgba;
	if (has_texture) {
		color_rgba = texture(texture1, frag_coord).rgba;
	}
	else {
		color_rgba = vec4(0.f, 0.f, 0.f, 1.f);
	}

	out_color = color_rgba;
}