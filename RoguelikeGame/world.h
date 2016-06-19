#ifndef WORLD_H
#define WORLD_H

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

    int GetTile(int tile_x, int tile_y)
    {
        return tile_grid[tile_y * width + tile_x];
    }

    bool IsInside(int tile_x, int tile_y)
    {
        return((tile_x >= 0) && (tile_x < width) &&
               (tile_y >= 0) && (tile_y < height));
    }
};

void WorldToTile(float tile_size, float world_x, float world_z, int* tile_x, int* tile_y);
void TileToWorld(float tile_size, int tile_x, int tile_y, float* world_x, float* world_z);

#endif//WORLD_H