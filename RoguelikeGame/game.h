#ifndef GAME_H
#define GAME_H
#include <glew.h>
#include "game_math.h"
#include "definitions.h"
#include "world.h"
#include "player.h"
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
    float fov = 60.0f;//TODO: Need to adjust fov value
    float far_plane = 5.0f;
    float near_plane = 0.1f;

    Mat4 GetView();
    Mat4 GetProjection(float ratio);
};

struct MoveAction
{
    int origin_tile_x = 0;
    int origin_tile_y = 0;
    int dest_tile_x = 0;
    int dest_tile_y = 0;
    Vec2 from;
    Vec2 to;
    //float duration = ;
    float speed = 5.0f;
    float t = 0.0f;
    bool is_active = false;
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

    const float Tile_Size = 2.0f;
    Dungeon dungeon;

    std::vector<Entity> floors;
    std::vector<Entity> walls;
    //std::vector<Entity> enemies;
    Entity enemy;

    //MoveAction player_move;
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

    float action_timer = 0.0f;
    float action_time_limit = 5.0f;
    int enemy_x;
    int enemy_y;
    Vec2 enemy_src_pos;
    Vec2 enemy_dest_pos;
    float enemy_move_t = 0.0f;
    bool enemy_is_moving = false;

    GameState();
    ~GameState();

    void Update(float dt, const Input& input);

    void Render(float screenRatio);

    void DebugUpdate(float dt, const Input& input);
};

#pragma warning(pop)
#endif//GAME_H
