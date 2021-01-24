#version 330 core

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform float gamma;
uniform float exposure;

out vec4 FragColor;

void main()
{
    // Hdr tone mapping
    vec3 color = texture(screenTexture, TexCoords).rgb;
    vec3 bloom_color = texture(bloomTexture, TexCoords).rgb;
    color += bloom_color;

    vec3 result = vec3(1.0) - exp(-color * exposure);
    result = pow(result, vec3(1.0 / gamma));
    
    FragColor = vec4(result, 1.0);
}