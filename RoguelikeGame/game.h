#ifndef GAME_H
#define GAME_H
#include <glew.h>
#include "game_math.h"
#include "definitions.h"
#include <vector>

struct Input
{
    struct KeyState
    {
        bool left = false;
        bool right = false;
        bool up = false;
        bool down = false;
    };
    KeyState down;
    KeyState pressed;
};

struct Tile
{
    Vec3 position;
    Vec3 scale = Vec3(1.0f, 1.0f, 1.0f);
    Vec3 rotation;
    int texture_id = 0;
};

struct Dungeon
{
    int width = 0;
    int height = 0;
    int size = 0;
    int* tile_grid = nullptr;

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

void InitDungeon(Dungeon* dungeon, int width, int height, int* tile_grid)
{
    dungeon->width = width;
    dungeon->height = height;
    dungeon->size = dungeon->width * dungeon->height;
    dungeon->tile_grid = new int[dungeon->size]{};
    memcpy(dungeon->tile_grid, tile_grid, dungeon->size * sizeof(int));
}

void WorldToTile(int tile_size, float world_x, float world_z, int* tile_x, int* tile_y);
void TileToWorld(int tile_size, int tile_x, int tile_y, float* world_x, float* world_z);

struct Camera
{
    Vec3 position = {};
    float rotation = 0.0f;
    float fov = 70.0f;
    float far_plane = 100.f;
    float near_plane = 0.1f;

    Mat4 GetView();
    Mat4 GetProjection(float ratio);
};

enum MoveState
{
    MoveState_Moving,
    MoveState_Idle,
    MoveState_Rotating
};

enum Direction
{
    Direction_Front = 0,
    Direction_Right,
    Direction_Back,
    Direction_Left,
};

struct GameState
{
    float quad[32] = {};
    u32 indices[6] = {};
    GLuint shader_program = 0;
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    std::vector<GLuint> textures;

    Camera camera;
    Mat4 view;
    Mat4 projection;

    std::vector<Tile> floors;
    std::vector<Tile> walls;

    const int Map_Size = 10;
    const float Tile_Size = 2.0f;
    Dungeon dungeon;
    //int tilemap[100] = {};

    int camera_prev_x = 0;
    int camera_prev_z = 0;
    int camera_x = 0;
    int camera_z = 0;
    MoveState move_state = MoveState_Idle;
    Direction player_dir = Direction_Front;
    float move_speed = 5.0f;
    float move_t = 0.0f;

    float action_time_limit = 5.0f;

    GameState();
    ~GameState();

    void Update(float dt, const Input& input);
    void Render(float screenRatio);
    void RenderTile(const Tile& tile, int model_location);
};

#endif//GAME_H
