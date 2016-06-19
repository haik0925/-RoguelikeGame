#include "world.h"
#include <cstring>

Dungeon::Dungeon(int width, int height, TileType* tile_type_grid)
    : width(width)
    , height(height)
{
    const auto size = this->size = this->width * this->height;
    this->tile_type_grid = new TileType[size]{};
    memcpy(this->tile_type_grid, tile_type_grid, size * sizeof(TileType));
    this->existence_grid = new bool[size]{};
    this->entity_grid = new Handle[size]{};
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
int TileToIndex(int tile_x, int tile_y, int tilemap_width)
{
    return (tile_y * tilemap_width + tile_x);
}