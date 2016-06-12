#version 330 core

in vec3 Color;
in vec2 Tex_Coord;

//uniform vec3 color;
uniform sampler2D our_texture;

void main()
{
    //gl_FragColor = vec4(0.7, 0.7, 1.0, 1.0);
    gl_FragColor = texture(our_texture, Tex_Coord);
    //gl_FragColor = vec4(Color, 1.0);
}
