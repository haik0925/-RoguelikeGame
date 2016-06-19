#ifndef ENTITY_H
#define ENTITY_H
#include "definitions.h"
#include "game_math.h"
#include "handle.h"

struct Entity
{
    Vec3 position;
    Vec3 scale;
    Vec3 rotation;
    int texture_id;

    explicit
    Entity(const Vec3& position = Vec3(),
         const Vec3& scale = Vec3(1.0f, 1.0f, 1.0f),
         const Vec3 rotation = Vec3(),
         int texture_id = 0)
         : position(position)
         , scale(scale)
         , rotation(rotation)
         , texture_id(texture_id)
    {}
};

struct Entities
    : ComponentManager<Entity>
{
};

#endif//ENTITY_H