#include "world.h"
#include <cstring>

Dungeon::Dungeon(int width, int height, int* tile_grid)
    : width(width)
    , height(height)
{
    this->size = this->width * this->height;
    this->tile_grid = new int[this->size]{};
    memcpy(this->tile_grid, tile_grid, this->size * sizeof(int));
}

void WorldToTile(float tile_size, float world_x, float world_z, int* tile_x, int* tile_y)
{
    *tile_x = (int)(world_x / tile_size);
    *tile_y = (int)(world_z / tile_size);
}
void TileToWorld(float tile_size, int tile_x, int tile_y, float* world_x, float* world_z)
{
    *world_x = tile_x * tile_size;
    *world_z = tile_y * tile_size;
}