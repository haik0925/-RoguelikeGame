#include "graphics.h"
#include "definitions.h"
#include "platform.h"
#include "world.h"
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

void OpenGLRenderSingleTextureEntities(const Entity* entities, int count, int model_location, int texture_id, int texture_location)
{
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glUniform1i(texture_location, 0);//GL_TEXTURE0

    for (int i = 0; i < count; ++i)
    {
        Mat4 model;
        const Entity& e = entities[i];

        float c = cos(ToRadian(e.rotation.x));
        float s = sin(ToRadian(e.rotation.x));
        Mat4 rotation_x =
        {
            1.f, 0.f, 0.f, 0.f,
            0.f, c, -s, 0.f,
            0.f, s, c, 0.f,
            0.f, 0.f, 0.f, 1.f
        };

        c = cos(ToRadian(e.rotation.y));
        s = sin(ToRadian(e.rotation.y));
        Mat4 rotation_y =
        {
            c, 0.f, -s, 0.f,
            0.f, 1.f, 0.f, 0.f,
            s, 0.f, c, 0.f,
            0.f, 0.f, 0.f, 1.f
        };

#if 0//TODO: Not used yet. Use this when has to be used.

        c = cos(ToRadian(e.rotation.y));
        s = sin(ToRadian(e.rotation.y));
        float c = cos(ToRadian(e.rotation.y));
        float s = sin(ToRadian(e.rotation.y));
        Mat4 rotation_z =
        {
            c, s, 0.f, 0.f,
            -s, c, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f
        };
#endif
        model =
            Translation(e.position.x, e.position.y, e.position.z) *
            rotation_x *
            rotation_y *
            Scale(e.scale.x, e.scale.y, e.scale.z);
        glUniformMatrix4fv(model_location, 1, GL_TRUE, model.m);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}