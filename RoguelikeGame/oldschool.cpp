#include <Windows.h>
#define GLEW_STATIC
#include <glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <cstdio>// sprintf
#include <cmath>// sin, cos
#include <chrono>
#include <cstdint>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#define INTERNAL static
#define GLOBAL static
#define LOCAL_PERSIST static

#define BUFFER_LENGTH 512

#if OLDSCHOOL_DEBUG
#define DEBUG_LOG(format, ...)\
{\
    char buf[BUFFER_LENGTH];\
    sprintf(buf, format, __VA_ARGS__);\
    OutputDebugString(buf);\
}
#else
#define DEBUG_LOG(format, ...)
#endif

GLOBAL bool global_running = false;

struct MemoryArena
{
    u64 transient_storage_size;
    void* transient_storage;
    u64 permanent_storage_size;
    void* permanent_storage;
};

struct Vec2
{
    float x;
    float y;

    void Set(float x, float y)
    {
        this->x = x;
        this->y = y;
    }
};

struct Color3
{
    float r;
    float g;
    float b;

    void Set(float r, float g, float b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }
};

struct Mat4
{
    float m[16];
};

Mat4 operator*(const Mat4& lhs, const Mat4& rhs)
{
    Mat4 result = {};

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            result.m[i * 4 + j] = lhs.m[i * 4] * rhs.m[j] + lhs.m[i * 4 + 1] * rhs.m[j + 4] + lhs.m[i * 4 + 2] * rhs.m[j + 8] + lhs.m[i * 4 + 3] * rhs.m[j + 12];

    return result;
}

Mat4 Identity()
{
    Mat4 out =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
    return out;
}

Mat4 Translation(float x, float y)
{
    Mat4 out =
    {
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };

    return out;
}

Mat4 Scale(float scaleX, float scaleY)
{
    Mat4 out =
    {
        scaleX, 0, 0, 0,
        0, scaleY, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };

    return out;
}

GLOBAL const float PI = 3.14f;

Mat4 Rotation(float radian)
{
    //float radian = degree * PI / 180.0f;
    float c = cos(radian);
    float s = sin(radian);
    Mat4 out =
    {
        c, -s, 0, 0,
        s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };

    return out;
}

struct Entity
{
    Mat4 model;
    Vec2 position;
    Vec2 scale;
    float rotation;
    Color3 color;
};

struct Size
{
    int width;
    int height;
};

INTERNAL
Size GetWindowSize(SDL_Window* window)
{
    //RECT screen_rect = {}; GetClientRect(window, &screen_rect);
    Size window_size;
    SDL_GetWindowSize(window, &window_size.width, &window_size.height);

    return window_size;
}
#if 0

INTERNAL
void ResizeWindowAndViewport(HWND window)
{
    RECT screen_rect = {}; GetClientRect(window, &screen_rect);
    int width = screen_rect.right - screen_rect.left;
    int height = screen_rect.bottom - screen_rect.top;
    glViewport(0, 0, width, height);
}
#endif

struct Input
{
    union Keys
    {
        struct
        {
            bool left;
            bool right;
            bool up;
            bool down;
        };
        bool keys[4];
    };
    Keys pressed;
    Keys down;
    Keys released;
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    auto ReadFileMemory = [](const char* filename) -> void*
    {
        HANDLE file_handle = CreateFile(filename,
            GENERIC_READ,
            FILE_SHARE_READ,
            0,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            0);
        if (file_handle != INVALID_HANDLE_VALUE)
        {
            LARGE_INTEGER file_size;
            if (GetFileSizeEx(file_handle, &file_size))
            {
                char* file_memory = static_cast<char*>(malloc(file_size.QuadPart + 1)); // +1 for null-terminated c-style string
                DWORD num_bytes_read;
                if (ReadFile(file_handle, file_memory, file_size.QuadPart, &num_bytes_read, nullptr))
                {
                    file_memory[num_bytes_read] = 0;
                    return file_memory;
                }
                else
                {
                    free(file_memory);
                }
            }
            CloseHandle(file_handle);
        }

        return nullptr;
    };

    auto LoadShader = [&ReadFileMemory](GLenum shader_type, const char* shader_filename, GLuint* out_shader_id) -> bool
    {
        LOCAL_PERSIST GLenum VERTEX_SHADER_TYPE = GL_VERTEX_SHADER;
        LOCAL_PERSIST GLenum FRAGMENT_SHADER_TYPE = GL_FRAGMENT_SHADER;
        *out_shader_id = glCreateShader(shader_type);
        auto shader_source = static_cast<GLchar*>(ReadFileMemory(shader_filename));
        if (shader_source)
        {
            glShaderSource(*out_shader_id, 1, &shader_source, nullptr);
            glCompileShader(*out_shader_id);
            free(shader_source);
            GLint succeeded = 0;
            glGetShaderiv(*out_shader_id, GL_COMPILE_STATUS, &succeeded);
            if (succeeded)
            {
                return true;
            }
            else
            {
                char error_message[BUFFER_LENGTH] = {};
                glGetShaderInfoLog(*out_shader_id, BUFFER_LENGTH, 0, error_message);
                DEBUG_LOG("%s\n", error_message);
            }
        }
        else
        {
            DEBUG_LOG("Failed to load shader source file.\n");
        }
        glDeleteShader(*out_shader_id);
        *out_shader_id = 0;
        return false;
    };

    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;

    auto InitWindow = [&window, &context](int window_width, int window_height) -> bool
    {
        if (SDL_Init(SDL_INIT_VIDEO) >= 0)
        {
            window = SDL_CreateWindow("OldSchoolFPS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
            if (window)
            {
                context = SDL_GL_CreateContext(window);
                if (context)
                {
                    glewExperimental = true;
                    if (glewInit() == GLEW_OK)
                    {
                        return true;
                    }
                    else
                    {
                        SDL_GL_DeleteContext(context);
                        context = nullptr;
                        return false;
                    }
                }
                else
                {
                    //const char* error = SDL_GetError();
                    SDL_DestroyWindow(window);
                    window = nullptr;
                    return false;
                }
            }
            else
            {
                SDL_Quit();
                return false;
            }
        }
        else
        {
            return false;
        }
    };

    auto ShutdownWindow = [&window, &context]()
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    };

    // Main start
    if (InitWindow(1280, 720))
    {
        GLuint vertex_shader_id = 0;
        bool is_vertex_shader_compiled = LoadShader(GL_VERTEX_SHADER, "Shader/default.vert", &vertex_shader_id);

        GLuint fragment_shader_id = 0;
        bool is_fragment_shader_compiled = LoadShader(GL_FRAGMENT_SHADER, "Shader/default.frag", &fragment_shader_id);

        bool is_shader_program_built = false;
        auto shader_program_id = glCreateProgram();
        if (is_vertex_shader_compiled && is_fragment_shader_compiled)
        {
            glAttachShader(shader_program_id, vertex_shader_id);
            glAttachShader(shader_program_id, fragment_shader_id);
            glLinkProgram(shader_program_id);
            int success = 0;
            glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
            if (success)
            {
                is_shader_program_built = true;
            }
            else
            {
                char error_message[BUFFER_LENGTH];
                glGetProgramInfoLog(shader_program_id, BUFFER_LENGTH, 0, error_message);
                DEBUG_LOG("%s\n", error_message);
            }
        }
        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);

        float vertices[] = { -0.5f, 0, 0.5f, 0.0f };
        GLuint vao, vbo;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);//?
        glBindVertexArray(0);

        auto model_location = glGetUniformLocation(shader_program_id, "model");
        auto view_location = glGetUniformLocation(shader_program_id, "view");
        auto projection_location = glGetUniformLocation(shader_program_id, "projection");
        auto color_location = glGetUniformLocation(shader_program_id, "color");

        // Input init
        Input input = {};

        MemoryArena arena;
        arena.transient_storage_size = 10 * 1024 * 1024;
        arena.permanent_storage_size = 10 * 1024 * 1024;//10 megabytes
        arena.transient_storage = VirtualAlloc(nullptr,
            arena.transient_storage_size + arena.permanent_storage_size,
            MEM_COMMIT | MEM_RESERVE,
            PAGE_READWRITE);
        arena.permanent_storage = (u8*)arena.transient_storage + arena.transient_storage_size;

        Entity* player = (Entity*)arena.permanent_storage;
        player->model = Identity();
        player->position.Set(0.0f, 0.0f);
        player->scale.Set(10.0f, 10.0f);
        player->rotation = 3.14f / 2.0f;
        player->color.Set(1.0f, 0.0f, 0.0f);

        Entity* wall = (Entity*)arena.permanent_storage + 1;
        int wall_count = 0;

        wall[wall_count].model = Identity();
        wall[wall_count].position.Set(0.0f, 75.0f);
        wall[wall_count].scale.Set(100.0f, 100.0f);
        wall[wall_count].rotation = 0.0f;
        wall[wall_count].color.Set(1.0f, 1.0f, 1.0f);
        ++wall_count;

        wall[wall_count].model = Identity();
        wall[wall_count].position.Set(0.0f, -75.0f);
        wall[wall_count].scale.Set(100.0f, 100.0f);
        wall[wall_count].rotation = 35.0f;
        wall[wall_count].color.Set(0.0f, 1.0f, 0.0f);
        ++wall_count;

        wall[wall_count].model = Identity();
        wall[wall_count].position.Set(75.0f, 0.0f);
        wall[wall_count].scale.Set(100.0f, 100.0f);
        wall[wall_count].rotation = 90.0f;
        wall[wall_count].color.Set(0.0f, 0.0f, 1.0f);
        ++wall_count;

        Mat4 view =
        {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
        };

        auto window_size = GetWindowSize(window);

        float projection[] =
        {
            2.0f / window_size.width, 0, 0, 0,
            0, 2.0f / window_size.height, 0, 0,
            0, 0, 2.0f / 1000.0f, 0,
            0, 0, 0, 1,
        };

        float dirX = 0.0f, dirY = 0.0f;

        // Timer
        std::chrono::time_point<std::chrono::system_clock> last_time, current_time;
        last_time = std::chrono::system_clock::now();
        current_time = std::chrono::system_clock::now();
        float dt = 0.0f;

        // Main loop
        global_running = true;
        while (global_running)
        {
            current_time = std::chrono::system_clock::now();
            std::chrono::duration<float> duration = current_time - last_time;
            dt = duration.count();

            for (bool& key : input.pressed.keys) key = false;
            for (bool& key : input.released.keys) key = false;

            SDL_Event e;
            while (SDL_PollEvent(&e) != 0)
            {
                switch (e.type)
                {
                case SDL_QUIT:
                    global_running = false;
                    break;
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym)
                    {
                    case SDLK_UP:
                        input.pressed.up = true;
                        input.down.up = true;
                        break;
                    case SDLK_DOWN:
                        input.pressed.down = true;
                        input.down.down = true;
                        break;
                    case SDLK_LEFT:
                        input.pressed.left = true;
                        input.down.left = true;
                        break;
                    case SDLK_RIGHT:
                        input.pressed.right = true;
                        input.down.right = true;
                        break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (e.key.keysym.sym)
                    {
                    case SDLK_UP:
                        input.released.up = false;
                        input.down.up = false;
                        break;
                    case SDLK_DOWN:
                        input.released.down = false;
                        input.down.down = false;
                        break;
                    case SDLK_LEFT:
                        input.released.left = false;
                        input.down.left = false;
                        break;
                    case SDLK_RIGHT:
                        input.released.right = false;
                        input.down.right = false;
                        break;
                    }
                    break;
                }
            }

            enum Perspective
            {
                Player_2D,
                Camera_2D,
                Camera_3D
            } perspective = Camera_2D;

            if (input.down.left)
                player->rotation += 2.0f * dt;
            if (input.down.right)
                player->rotation -= 2.0f * dt;

            dirX = cos(player->rotation);
            dirY = sin(player->rotation);

            if (input.down.up)
            {
                player->position.x += 100.0f * dirX * dt;
                player->position.y += 100.0f * dirY * dt;
            }
            if (input.down.down)
            {
                player->position.x -= 100.0f * dirX * dt;
                player->position.y -= 100.0f * dirY * dt;
            }

            switch (perspective)
            {
            case Camera_2D:
                view = Rotation(-player->rotation + 3.14f / 2.0f) * Translation(-player->position.x, -player->position.y);
                break;
            }

            Mat4 offset =
            {
                1, 0, 0, 0.5f,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1,
            };


            window_size = GetWindowSize(window);
            projection[0] = 2.0f / window_size.width;
            projection[5] = 2.0f / window_size.height;

            glViewport(0, 0, window_size.width, window_size.height);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(shader_program_id);
            glUniformMatrix4fv(view_location, 1, GL_TRUE, view.m);
            glUniformMatrix4fv(projection_location, 1, GL_TRUE, projection);
            glBindVertexArray(vao);

            // player
            player->model = Translation(player->position.x, player->position.y) * Rotation(player->rotation) * Scale(player->scale.x, player->scale.y) * offset;
            glUniformMatrix4fv(model_location, 1, GL_TRUE, player->model.m);
            glUniform3fv(color_location, 1, (float*)&player->color);
            glDrawArrays(GL_LINES, 0, 2);

            // wall
            for (int i = 0; i < wall_count; ++i)
            {
                wall[i].model = Translation(wall[i].position.x, wall[i].position.y) * Rotation(wall[i].rotation) * Scale(wall[i].scale.x, wall[i].scale.y);
                glUniformMatrix4fv(model_location, 1, GL_TRUE, wall[i].model.m);
                glUniform3fv(color_location, 1, (float*)&wall[i].color);
                glDrawArrays(GL_LINES, 0, 2);
            }

            glBindVertexArray(0);

            //SwapBuffers(device_context);
            SDL_GL_SwapWindow(window);

            last_time = current_time;
        }

        VirtualFree(arena.transient_storage, arena.transient_storage_size + arena.permanent_storage_size, MEM_RELEASE);

        // OpenGL Shutdown
        glDeleteProgram(shader_program_id);
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        ShutdownWindow();
    }
}