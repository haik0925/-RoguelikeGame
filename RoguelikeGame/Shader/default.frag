#version 330 core

in vec3 Color;
in vec2 Tex_Coord;

uniform vec3 color;
uniform sampler2D our_texture;
uniform bool is_texture_enabled;

void main()
{
    if(is_texture_enabled)
        gl_FragColor = texture(our_texture, Tex_Coord) * vec4(color, 1.0);
    else
        gl_FragColor = vec4(color, 1.0);
}
