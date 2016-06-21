#version 330 core

in vec3 Color;
in vec2 Tex_Coord;

//uniform vec3 color;
uniform sampler2D our_texture;

void main()
{
    gl_FragColor = texture(our_texture, Tex_Coord);
}
