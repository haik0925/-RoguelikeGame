#ifndef GAME_H
#define GAME_H
#include <glew.h>
#include "game_math.h"
#include "definitions.h"
#include "world.h"
#include <vector>
#include <unordered_map>
#pragma warning(push)
#pragma warning(disable: 4512)
#pragma warning(disable: 4351)

struct Input
{
    union KeyState
    {
        struct
        {
            bool rotate_left;
            bool rotate_right;
            bool move_left;
            bool move_right;
            bool move_front;
            bool move_back;
        };
        bool keys[6];
    };
    KeyState down;
    KeyState pressed;
};

struct Camera
{
    Vec3 position = {};
    float rotation = 0.0f;
    float fov = 70.0f;
    float far_plane = 15.0f;
    float near_plane = 0.1f;

    Mat4 GetView();
    Mat4 GetProjection(float ratio);
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

    std::vector<Enemy> enemies;

    int camera_prev_x = 0;
    int camera_prev_z = 0;
    int camera_x = 0;
    int camera_z = 0;
    float camera_prev_rotation = 0.0f;
    float camera_next_rotation = 0.0f;
    MoveState move_state = MoveState_Idle;
    Direction player_dir = Direction_Front;
    float move_speed = 5.0f;
    float move_t = 0.0f;
    float rotate_t = 0.0f;

    float action_time_limit = 5.0f;

    GameState();
    ~GameState();

    void Update(float dt, const Input& input);
    void Render(float screenRatio);
    void RenderTile(const Tile& tile, int model_location);
    void RenderEnemy(const Enemy& enemy, float face_angle, int model_location);
};

#pragma warning(pop)
#endif//GAME_H
