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

    explicit
    Entity(const Vec3& position = Vec3(),
           const Vec3& scale = Vec3(1.0f, 1.0f, 1.0f),
           const Vec3& rotation = Vec3())
         : position(position)
         , scale(scale)
         , rotation(rotation)
    {}
};

struct Color3
{
    float r;
    float g;
    float b;
};

struct OpaqueSprite
{
    int texture_id;
    //Color3 color;//TODO: Not used yet.
};

struct Color4
{
    float r;
    float g;
    float b;
    float a;
};

struct TranslucentSprite
{
    int texture_id;
    //Color4 color;//TODO: Not used yet.
};

#endif//ENTITY_H