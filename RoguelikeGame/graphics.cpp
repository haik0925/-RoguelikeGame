#include "graphics.h"
#include "definitions.h"
#include "platform.h"
#include <cstdlib>

bool OpenGLCreateShader(GLenum shader_type, const char* shader_filename, GLuint* out_shader_id)
{
    LOCAL_PERSIST GLenum VERTEX_SHADER_TYPE = GL_VERTEX_SHADER;
    LOCAL_PERSIST GLenum FRAGMENT_SHADER_TYPE = GL_FRAGMENT_SHADER;
    *out_shader_id = glCreateShader(shader_type);
    auto shader_source = static_cast<GLchar*>(PlatformReadFileMemory(shader_filename));
    if (shader_source)
    {
        glShaderSource(*out_shader_id, 1, &shader_source, nullptr);
        glCompileShader(*out_shader_id);
        free(shader_source);
        GLint succeeded = 0;
        glGetShaderiv(*out_shader_id, GL_COMPILE_STATUS, &succeeded);
        if (succeeded)
        {
            return true;
        }
        else
        {
            char error_message[512] = {};
            glGetShaderInfoLog(*out_shader_id, 512, 0, error_message);
            DEBUG_LOG("%s\n", error_message);
        }
    }
    else
    {
        DEBUG_LOG("Failed to load shader source file.\n");
    }
    glDeleteShader(*out_shader_id);
    *out_shader_id = 0;
    return false;
};

bool OpenGLCreateProgram(const char* vert_name, const char* frag_name, GLuint* program_id)
{
    GLuint vert_id = 0, frag_id = 0;
    if(OpenGLCreateShader(GL_VERTEX_SHADER, vert_name, &vert_id))
    {
        if(OpenGLCreateShader(GL_FRAGMENT_SHADER, frag_name, &frag_id))
        {
            *program_id = glCreateProgram();
            glAttachShader(*program_id, vert_id);
            glAttachShader(*program_id, frag_id);
            glLinkProgram(*program_id);
            glDeleteProgram(vert_id);
            glDeleteProgram(frag_id);
            int success = 0;
            glGetProgramiv(*program_id, GL_LINK_STATUS, &success);
            if(success)
            {
                return true;
            }
            else
            {
                char error_message[512] = {};
                glGetProgramInfoLog(*program_id, 512, 0, error_message);
                DEBUG_LOG("%s\n", error_message);
                glDeleteProgram(*program_id);
                *program_id = 0;
                return false;
            }
        }
        else
        {
            glDeleteShader(vert_id);
            return false;
        }
    }
    else
    {
        return false;
    }
}
