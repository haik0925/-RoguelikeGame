#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <glew.h>

struct Entity;

bool OpenGLCreateShader(GLenum shader_type, const char* shader_filename, GLuint* out_shader_id);
bool OpenGLCreateProgram(const char* vert_name, const char* frag_name, GLuint* program_id);
void OpenGLRenderSingleTextureEntities(const Entity* entities,
                                             int count,
                                             int model_location,
                                             int texture_id,
                                             int texture_location);

void OpenGLDrawOpaque(const Handle* handles, int count,
                      ComponentManager<Entity>& entities,
                      ComponentManager<OpaqueSprite>& opaque_sprites,
                      int model_location, int texture_location);

//TODO: This function has same code with DrawOpaque function.
//      Is it necessary to have separate function for translucent sprites?
void OpenGLDrawTranslucent(const Handle* handles, int count,
                           ComponentManager<Entity>& entities,
                           ComponentManager<TranslucentSprite>& translucent_sprites,
                           int model_location, int texture_location);


#endif//GRAPHICS_H
