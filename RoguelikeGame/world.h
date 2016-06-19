#ifndef WORLD_H
#define WORLD_H
#include "handle.h"
#include "definitions.h"

enum TileType : u8
{
    Tile_Empty = 0,
    Tile_Floor = 1,
};

struct Dungeon
{
    int width = 0;
    int height = 0;
    int size = 0;
    //Tile* tile_grid = nullptr;
    TileType* tile_type_grid = nullptr;
    bool*     existence_grid = nullptr;
    Handle*   entity_grid = nullptr;

    Dungeon() = default;
    Dungeon(int width, int height, TileType* tile_type_grid);

    ~Dungeon()
    {
        delete[] tile_type_grid; tile_type_grid = nullptr;
        delete[] existence_grid; existence_grid = nullptr;
        delete[] entity_grid; entity_grid = nullptr;
    }

    TileType GetTile(int tile_x, int tile_y)
    {
        ASSERT(IsInside(tile_x, tile_y));
        return tile_type_grid[tile_y * width + tile_x];
    }

    bool IsEntityExist(int tile_x, int tile_y)
    {
        ASSERT(IsInside(tile_x, tile_y));
        return existence_grid[tile_y * width + tile_x];
    }

    Handle GetEntityHandle(int tile_x, int tile_y)
    {
        ASSERT(IsInside(tile_x, tile_y));
        return entity_grid[tile_y * width + tile_x];
    }

    void MoveEntity(int from_tile_x, int from_tile_y, int to_tile_x, int to_tile_y)
    {
        ASSERT(IsInside(from_tile_x, from_tile_y));
        ASSERT(IsInside(to_tile_x, to_tile_y));

        int from_index = from_tile_y * width + from_tile_x;
        int to_index = to_tile_y * width + to_tile_x;
        existence_grid[from_index] = false;
        existence_grid[to_index] = true;
        entity_grid[to_index] = entity_grid[from_index];
    }

    bool IsInside(int tile_x, int tile_y)
    {
        return((tile_x >= 0) && (tile_x < width) &&
               (tile_y >= 0) && (tile_y < height));
    }
};

void WorldToTile(float tile_size, float world_x, float world_z, int* tile_x, int* tile_y);
void TileToWorld(float tile_size, int tile_x, int tile_y, float* world_x, float* world_z);
int TileToIndex(int tile_x, int tile_y, int tilemap_width);

#endif//WORLD_H