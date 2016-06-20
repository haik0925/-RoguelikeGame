#ifndef GAME_H
#define GAME_H
#include <glew.h>
#include "game_math.h"
#include "definitions.h"
#include "world.h"
#include "entity.h"
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

INTERNAL
inline
void Interpolate(float from, float to, float t, float* result)
{
    *result = from * (1.0f - t) + to * t;
}

struct TileMovement
{
    int tile_x = 0;
    int tile_y = 0;
    Vec2 from;
    Vec2 to;
    float t = 0.0f;
    const float speed = 5.0f;
    bool active = false;
    Dungeon* dungeon = nullptr;

    void SetPositionToMove(const float Tile_Size, int new_tile_x, int new_tile_y)
    {
        if (dungeon->IsEntityExist(new_tile_x, new_tile_y) == false)
        {
            dungeon->MoveEntity(tile_x, tile_y, new_tile_x, new_tile_y);
            TileToWorld(Tile_Size, tile_x, tile_y, &from.x, &from.y);
            TileToWorld(Tile_Size, new_tile_x, new_tile_y, &to.x, &to.y);
            tile_x = new_tile_x;
            tile_y = new_tile_y;
            t = 0.0f;
            active = true;
        }
    }

    // Returns true when the action is completed
    bool UpdateMovement(float dt, float* x, float* y)
    {
        bool result = false;

        t += (speed * dt);
        if (t >= 1.0f)
        {
            t = 1.0f;
            result = true;
        }
        Interpolate(from.x, to.x, t, x);
        Interpolate(from.y, to.y, t, y);
        active = !result;

        return result;
    }
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

    HandleManager handle_manager;
    ComponentManager<Entity>            entities;
    ComponentManager<OpaqueSprite>      opaque_sprites;
    ComponentManager<TranslucentSprite> translucent_sprites;
    ComponentManager<TileMovement>      tile_movements;

    /*
    std::vector<Handle> floors;
    std::vector<Handle> walls;
    */
    std::vector<Handle> enemies;
    std::vector<Handle> drawables_opaque;
    std::vector<Handle> drawables_translucent;

    Handle player;
    //TileMovement player_move;
    MoveState move_state = MoveState_Idle;
    float camera_prev_rotation = 0.0f;
    float camera_next_rotation = 0.0f;
    Direction player_dir = Direction_Front;
    int player_dir_x = 0;
    int player_dir_y = 1;
    float rotate_t = 0.0f;

    float action_timer = 0.0f;
    const float action_time_limit = 10.0f;
    //TileMovement enemy_move;

    GameState();
    ~GameState();

    void Update(float dt, const Input& input);

    void Render(float screenRatio);

    void DebugUpdate(float dt, const Input& input);
};

#pragma warning(pop)
#endif//GAME_H
