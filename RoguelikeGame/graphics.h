#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <glew.h>

bool OpenGLCreateShader(GLenum shader_type, const char* shader_filename, GLuint* out_shader_id);
bool OpenGLCreateProgram(const char* vert_name, const char* frag_name, GLuint* program_id);

#endif//GRAPHICS_H
