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

struct Camera
{
    Vec3 position = {};
    float rotation = 0.0f;
    //Vec2 direction = {};
    float fov = 90.0f;
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
    //GLuint texture = 0;

    Camera camera;
    Mat4 view;
    Mat4 projection;

    std::vector<Tile> floors;
    std::vector<Tile> walls;
    //Wall walls[3];

    const int Map_Size = 10;
    const float Tile_Size = 2.0f;
    int tilemap[100] = {};

    int camera_x;
    int camera_z;
    MoveState move;
    Direction dir;

    GameState();
    ~GameState();

    void Update(float dt, const Input& input);
    void Render(float screenRatio);
    void RenderTile(const Tile& tile, int model_location);
};

#endif//GAME_H
