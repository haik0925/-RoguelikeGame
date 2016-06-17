#ifndef ENTITY_H
#define ENTITY_H
#include "game_math.h"

struct Dungeon
{
    int width = 0;
    int height = 0;
    int size = 0;
    int* tile_grid = nullptr;

    Dungeon() = default;
    Dungeon(int width, int height, int* tile_grid);

    ~Dungeon()
    {
        delete[] tile_grid;
        tile_grid = nullptr;
    }

    int GetTile(int x, int y)
    {
        return tile_grid[y * width + x];
    }
};

void WorldToTile(float tile_size, float world_x, float world_z, int* tile_x, int* tile_y);
void TileToWorld(float tile_size, int tile_x, int tile_y, float* world_x, float* world_z);

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

#endif//ENTITY_H