#include "game.h"
#include "graphics.h"
#define STB_IMAGE_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable: 4244)
#include <stb_image.h>
#pragma warning(pop)
#include <cmath>

Mat4 Camera::GetView()
{
    float rotation_radian = ToRadian(rotation);
    Vec3 right(1.0f * cos(rotation_radian), 0.0f, 1.0f * sin(rotation_radian));
    Vec3 up(0.0f, 1.0f, 0.0f);
    Vec3 look = CrossProduct(right, up);

    auto adjusted_pos = position;
    float offset = 1.0f;
    adjusted_pos.x -= look.x * offset;
    adjusted_pos.y -= look.y * offset;
    adjusted_pos.z -= look.z * offset;
    Mat4 result =
    {
        right.x, right.y, right.z, -DotProduct(adjusted_pos, right),
        up.x, up.y, up.z, -DotProduct(adjusted_pos, up),
        look.x, look.y, look.z, -DotProduct(adjusted_pos, look),
        0.0f, 0.0f, 0.0f, 1.0f
    };

    return result;
}

Mat4 Camera::GetProjection(float ratio)
{
    float d = 1 / tan(ToRadian(fov) / 2.0f);
    Mat4 result =
    {
        d / ratio, 0.f, 0.f, 0.f,
        0.f, d, 0.f, 0.f,
        0.f, 0.f, 1.0f / (far_plane - near_plane), -near_plane / (far_plane - near_plane),
        0.f, 0.f, 1.0f, 0.f,
    };

    return result;
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

GameState::GameState()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    float temp[] =
    {
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    for (int i = 0; i < ARRAY_SIZE(quad); ++i)
        quad[i] = temp[i];

    indices[0] = 3; indices[1] = 0; indices[2] = 1;
    indices[3] = 1; indices[4] = 2; indices[5] = 3;

    OpenGLCreateProgram("Shader/default.vert", "Shader/default.frag", &shader_program);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), (float*)quad, GL_STATIC_DRAW);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    auto LoadTexture = [this](const char* filename)
    {
        int sheet_w;
        int sheet_h;
        int sheet_bpp;
        auto* rgb = stbi_load(filename, &sheet_w, &sheet_h, &sheet_bpp, 4);
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sheet_w, sheet_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgb);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(rgb);
        glBindTexture(GL_TEXTURE_2D, 0);
        textures.push_back(texture);
    };

    LoadTexture("Texture/floor.png");
    LoadTexture("Texture/wall.png");
    LoadTexture("Texture/enemy.png");

#if 0
    {
        int sheet_w;
        int sheet_h;
        int sheet_bpp;
        auto* rgb = stbi_load("Texture/floor.png", &sheet_w, &sheet_h, &sheet_bpp, 3);
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sheet_w, sheet_h, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(rgb);
        glBindTexture(GL_TEXTURE_2D, 0);
        textures.push_back(texture);
    }

    {
        int sheet_w;
        int sheet_h;
        int sheet_bpp;
        auto* rgb = stbi_load("Texture/wall.png", &sheet_w, &sheet_h, &sheet_bpp, 3);
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sheet_w, sheet_h, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(rgb);
        glBindTexture(GL_TEXTURE_2D, 0);
        textures.push_back(texture);
    }

    {
        int sheet_w;
        int sheet_h;
        int sheet_bpp;
        auto* rgb = stbi_load("Texture/enemy.png", &sheet_w, &sheet_h, &sheet_bpp, 3);
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sheet_w, sheet_h, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(rgb);
        glBindTexture(GL_TEXTURE_2D, 0);
        textures.push_back(texture);
    }
#endif


    int grid[100] =
    {
        1,1,1,1,1,1,1,1,1,1,
        1,1,0,1,0,0,0,0,0,1,
        1,1,0,1,0,0,0,0,0,1,
        1,1,0,1,0,0,0,0,0,1,
        1,1,0,1,0,0,0,0,0,1,
        2,1,0,1,0,0,0,0,0,1,
        1,1,0,1,1,1,1,1,0,1,
        1,1,0,0,0,0,1,1,0,1,
        1,1,0,0,0,0,1,1,0,1,
        1,1,1,1,1,1,1,1,1,1,
    };
    InitDungeon(&dungeon, 10, 10, grid);

    floors.reserve(100);
#if 0
    for (int i = 0; i < ARRAY_SIZE(tilemap); ++i)
    {
        tilemap[i] = grid[i];
        if (tilemap[i] == 1)
        {
            Tile floor = {};
            TileToWorld(Tile_Size, i % 10, i / 10, &floor.position.x, &floor.position.z);
            //floor.position.x = Tile_Size * (i % 10);
            floor.position.y = -(Tile_Size / 2.0f);
            //floor.position.z = Tile_Size * (i / 10);
            floor.scale.Set(Tile_Size, Tile_Size, 1.0f);
            floor.rotation.x = 90.0f;
            floors.push_back(floor);
        }
    }
#endif
    for (int y = 0; y < dungeon.height; ++y)
    {
        for (int x = 0; x < dungeon.width; ++x)
        {
            int tile_value = dungeon.GetTile(x, y);
            if (tile_value > 0)
            {
                Tile floor = {};
                TileToWorld(Tile_Size, x, y, &floor.position.x, &floor.position.z);
                //floor.position.x = Tile_Size * (i % 10);
                floor.position.y = -(Tile_Size / 2.0f);
                //floor.position.z = Tile_Size * (i / 10);
                floor.scale.Set(Tile_Size, Tile_Size, 1.0f);
                floor.rotation.x = 90.0f;
                floors.push_back(floor);
            }

            if (tile_value == 2)
            {
                Enemy enemy = {};
                TileToWorld(Tile_Size, x, y, &enemy.position.x, &enemy.position.z);
                enemy.scale.Set(1.5f, 1.5f, 1.5f);
                enemies.push_back(enemy);
            }
        }
    }

    walls.reserve(100);
#if 0
    for (int i = 0; i < ARRAY_SIZE(tilemap); ++i)
    {
        if (tilemap[i] == 1)
        {
            if ((i % 10 == 9) || ((i + 1) < ARRAY_SIZE(tilemap)) && tilemap[i + 1] == 0)
            {
                Tile wall = {};
                wall.scale.Set(Tile_Size, Tile_Size, 1.0f);
                TileToWorld(Tile_Size, i % 10, i / 10, &wall.position.x, &wall.position.z);
                wall.position.x += Tile_Size * 0.5f;
                wall.rotation.y = 90.0f;
                walls.push_back(wall);
            }

            if ((i % 10 == 0) || (((i - 1) >= 0) && tilemap[i - 1] == 0))
            {
                Tile wall = {};
                wall.scale.Set(Tile_Size, Tile_Size, 1.0f);
                TileToWorld(Tile_Size, i % 10, i / 10, &wall.position.x, &wall.position.z);
                wall.position.x -= Tile_Size * 0.5f;
                wall.rotation.y = 90.0f;
                walls.push_back(wall);
            }

            if ((i / 10 == 9) || ((i + 10) < ARRAY_SIZE(tilemap)) && tilemap[i + 10] == 0)
            {
                Tile wall = {};
                wall.scale.Set(Tile_Size, Tile_Size, 1.0f);
                TileToWorld(Tile_Size, i % 10, i / 10, &wall.position.x, &wall.position.z);
                wall.position.z += Tile_Size * 0.5f;
                walls.push_back(wall);
            }

            if ((i / 10 == 0) || ((i - 10) >= 0) && tilemap[i - 10] == 0)
            {
                Tile wall = {};
                wall.scale.Set(Tile_Size, Tile_Size, 1.0f);
                TileToWorld(Tile_Size, i % 10, i / 10, &wall.position.x, &wall.position.z);
                wall.position.z -= Tile_Size * 0.5f;
                walls.push_back(wall);
            }


        }
    }
#endif

    for (int y = 0; y < dungeon.height; ++y)
    {
        for (int x = 0; x < dungeon.width; ++x)
        {
            if (dungeon.GetTile(x, y) > 0)
            {
                if ((x == dungeon.width - 1) || (((x + 1) < dungeon.width) && dungeon.GetTile(x + 1, y) == 0))
                {
                    Tile wall = {};
                    wall.scale.Set(Tile_Size, Tile_Size, 1.0f);
                    TileToWorld(Tile_Size, x, y, &wall.position.x, &wall.position.z);
                    wall.position.x += Tile_Size * 0.5f;
                    wall.rotation.y = 90.0f;
                    walls.push_back(wall);
                }

                if ((x == 0) || (((x - 1) >= 0) && dungeon.GetTile(x - 1, y) == 0))
                {
                    Tile wall = {};
                    wall.scale.Set(Tile_Size, Tile_Size, 1.0f);
                    TileToWorld(Tile_Size, x, y, &wall.position.x, &wall.position.z);
                    wall.position.x -= Tile_Size * 0.5f;
                    wall.rotation.y = 90.0f;
                    walls.push_back(wall);
                }

                if ((y == dungeon.height - 1) || ((y + 1) < dungeon.height - 1) && dungeon.GetTile(x, y + 1) == 0)
                {
                    Tile wall = {};
                    wall.scale.Set(Tile_Size, Tile_Size, 1.0f);
                    TileToWorld(Tile_Size, x, y, &wall.position.x, &wall.position.z);
                    wall.position.z += Tile_Size * 0.5f;
                    walls.push_back(wall);
                }

                if ((y == 0) || ((y - 1) >= 0) && dungeon.GetTile(x, y - 1) == 0)
                {
                    Tile wall = {};
                    wall.scale.Set(Tile_Size, Tile_Size, 1.0f);
                    TileToWorld(Tile_Size, x, y, &wall.position.x, &wall.position.z);
                    wall.position.z -= Tile_Size * 0.5f;
                    walls.push_back(wall);
                }

            }
        }
    }
}

GameState::~GameState()
{
    glDeleteProgram(shader_program);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void GameState::Update(float dt, const Input& input)
{

#if 1
    int movement = 0;
    if (move_state == MoveState_Idle)
    {
        if (input.pressed.move_front != input.pressed.move_back)
        {
            if (input.pressed.move_front)
                movement = 1;
            else//move_back
                movement = -1;

            //int moved_index = 0;
            int moved_x = camera_x;
            int moved_z = camera_z;
            switch (player_dir)
            {
            case Direction_Front:
                moved_z += movement;
                break;
            case Direction_Right:
                moved_x += movement;
                break;
            case Direction_Back:
                moved_z -= movement;
                break;
            case Direction_Left:
                moved_x -= movement;
                break;
            }
            if (moved_x >= 0 && moved_x < Map_Size && moved_z >= 0 && moved_z < Map_Size)
            {
                //if (tilemap[moved_z * Map_Size + moved_x] == 1)
                if (dungeon.GetTile(moved_x, moved_z) > 0)
                {
                    camera_prev_x = camera_x;
                    camera_prev_z = camera_z;
                    camera_x = moved_x;
                    camera_z = moved_z;
                    move_state = MoveState_Moving;
                }
            }
        }

        if (input.pressed.move_left != input.pressed.move_right)
        {
            if (input.pressed.move_right)
                movement = 1;
            else//move_right
                movement = -1;

            //int moved_index = 0;
            int moved_x = camera_x;
            int moved_z = camera_z;
            switch (player_dir)
            {
            case Direction_Front:
                moved_x += movement;
                break;
            case Direction_Right:
                moved_z -= movement;
                break;
            case Direction_Back:
                moved_x -= movement;
                break;
            case Direction_Left:
                moved_z += movement;
                break;
            }
            if (moved_x >= 0 && moved_x < Map_Size && moved_z >= 0 && moved_z < Map_Size)
            {
                if (dungeon.GetTile(moved_x, moved_z) > 0)
                {
                    camera_prev_x = camera_x;
                    camera_prev_z = camera_z;
                    camera_x = moved_x;
                    camera_z = moved_z;
                    move_state = MoveState_Moving;
                }
            }
        } 
    }

    switch (move_state)
    {
    case MoveState_Idle:
        move_t = 0.0f;
        break;
    case MoveState_Moving:
        move_t += move_speed * dt;
        if (move_t >= 1.0f)
        {
            move_t = 1.0f;
            move_state = MoveState_Idle;
        }
        if (camera_x != camera_prev_x)
            camera.position.x = (Tile_Size * ((camera_prev_x * (1.0f - move_t)) + (camera_x * move_t)));
        if (camera_z != camera_prev_z)
            camera.position.z = (Tile_Size * ((camera_prev_z * (1.0f - move_t)) + (camera_z * move_t)));
        break;
    }

#else

    float rotation_radian = ToRadian(camera.rotation);
    Vec3 go(-sin(rotation_radian), 0.0f, cos(rotation_radian));
    float speed = 10.0f;
    if(input.down.up)
    {
        camera.position.x += go.x * speed * dt;
        camera.position.y += go.y * speed * dt;
        camera.position.z += go.z * speed * dt;
    }
    if(input.down.down)
    {
        camera.position.x -= go.x * speed * dt;
        camera.position.y -= go.y * speed * dt;
        camera.position.z -= go.z * speed * dt;
    }
#endif

#define MOVE_MODE 0
#if MOVE_MODE == 0

    if (move_state == MoveState_Idle)
    {
        if (input.pressed.rotate_right != input.pressed.rotate_left)
        {
            if (input.pressed.rotate_right)
            {
                if (player_dir == Direction_Left)
                    player_dir = Direction_Front;
                else
                    player_dir = (Direction)(player_dir + 1);
                camera_next_rotation = camera.rotation - 90.0f;
            }
            else//left_pressed
            {
                if (player_dir == Direction_Front)
                    player_dir = Direction_Left;
                else
                    player_dir = (Direction)(player_dir - 1);
                camera_next_rotation = camera.rotation + 90.0f;
            }

            camera_prev_rotation = camera.rotation;
            rotate_t = 0.0f;
            move_state = MoveState_Rotating;
        }
    }

    //TODO: Need to merge to switch statement above
    if (move_state == MoveState_Rotating)
    {
        float rotate_speed = 5.0f;
        rotate_t += dt * rotate_speed;
        if (rotate_t >= 1.0f)
        {
            rotate_t = 1.0f;
            switch (player_dir)
            {
            case Direction_Front:
                camera.rotation = 0.0f;
                break;
            case Direction_Right:
                camera.rotation = -90.0f;
                break;
            case Direction_Back:
                camera.rotation = -180.0f;
                break;
            case Direction_Left:
                camera.rotation = -270.0f;
                break;
            }
            move_state = MoveState_Idle;
        }
        else
        {
            camera.rotation = camera_prev_rotation * (1.0f - rotate_t) + camera_next_rotation * rotate_t;
        }
    }

#else
    float rotation_speed = 180.0f;
    if(input.down.right)
    {
        camera.rotation -= rotation_speed * dt;
    }
    if(input.down.left)
    {
        camera.rotation += rotation_speed * dt;
    }
    if (camera.rotation >= 360.0f)
        camera.rotation -= 360.0f;
    else if (camera.rotation < 0.0f)
        camera.rotation += 360.0f;

    if (camera.rotation >= 315.0f || camera.rotation < 45.0f)
        player_dir = Direction_Front;
    else if (camera.rotation >= 45.0f && camera.rotation < 135.0f)
        player_dir = Direction_Left;
    else if (camera.rotation >= 135.0f && camera.rotation < 225.0f)
        player_dir = Direction_Back;
    else if (camera.rotation >= 225.0f && camera.rotation < 315.0f)
        player_dir = Direction_Right;
#endif


}

void GameState::Render(float screenRatio)
{

    Mat4 view = camera.GetView();
    Mat4 projection = camera.GetProjection(screenRatio);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto model_location = glGetUniformLocation(shader_program, "model");
    auto view_location = glGetUniformLocation(shader_program, "view");
    auto projection_location = glGetUniformLocation(shader_program, "projection");
    //auto color_location = glGetUniformLocation(shader_program, "color");
    glUseProgram(shader_program);
    glUniformMatrix4fv(view_location, 1, GL_TRUE, view.m);
    glUniformMatrix4fv(projection_location, 1, GL_TRUE, projection.m);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glUniform1i(glGetUniformLocation(shader_program, "our_texture"), 0);
    for (auto& floor : floors)
    {
        RenderTile(floor, model_location);
    }
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glUniform1i(glGetUniformLocation(shader_program, "our_texture"), 1);
    for (auto& wall : walls)
    {
        RenderTile(wall, model_location);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float camera_rotation = camera.rotation;
    for (auto& enemy : enemies)
    {
        RenderEnemy(enemy, camera_rotation, model_location);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GameState::RenderTile(const Tile& tile, int model_location)
{
    Mat4 model;
    Mat4 rotation = Identity();

    {
        float c = cos(ToRadian(tile.rotation.x));
        float s = sin(ToRadian(tile.rotation.x));
        Mat4 rotation_x =
        {
            1.f, 0.f, 0.f, 0.f,
            0.f, c, -s, 0.f,
            0.f, s, c, 0.f,
            0.f, 0.f, 0.f, 1.f
        };
        rotation *= rotation_x;
    }

    {
        float c = cos(ToRadian(tile.rotation.y));
        float s = sin(ToRadian(tile.rotation.y));
        Mat4 rotation_y = 
        {
            c, 0.f, -s, 0.f,
            0.f, 1.f, 0.f, 0.f,
            s, 0.f, c, 0.f,
            0.f, 0.f, 0.f, 1.f
        };
        rotation *= rotation_y;
    }

    model =
        Translation(tile.position.x, tile.position.y, tile.position.z) *
        rotation *
        Scale(tile.scale.x, tile.scale.y, tile.scale.z);
    glUniformMatrix4fv(model_location, 1, GL_TRUE, model.m);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void GameState::RenderEnemy(const Enemy& enemy, float face_angle, int model_location)
{
    float c = cos(ToRadian(face_angle));
    float s = sin(ToRadian(face_angle));
    Mat4 rotation = 
    {
        c, 0.f, -s, 0.f,
        0.f, 1.f, 0.f, 0.f,
        s, 0.f, c, 0.f,
        0.f, 0.f, 0.f, 1.f
    };

    Mat4 model =
        Translation(enemy.position.x, enemy.position.y, enemy.position.z) *
        rotation *
        Scale(enemy.scale.x, enemy.scale.y, enemy.scale.z);

    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glUniformMatrix4fv(model_location, 1, GL_TRUE, model.m);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
