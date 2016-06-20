#include <Windows.h>
#include <glew.h>
#include <SDL\SDL.h>
#include <SDL\SDL_opengl.h>
#include <chrono>
#include "definitions.h"
#include "game.h"
#include "platform.h"

void* PlatformReadFileMemory(const char* filename)
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
            char* file_memory = static_cast<char*>(malloc((size_t)file_size.QuadPart + 1)); // +1 for null-terminated c-style string
            //char* file_memory = (char*)game_memory.transient_free_address;//static_cast<char*>(malloc((size_t)file_size.QuadPart + 1)); // +1 for null-terminated c-style string
            DWORD num_bytes_read;
            if (ReadFile(file_handle, file_memory, (DWORD)file_size.QuadPart, &num_bytes_read, nullptr))
            {
                file_memory[num_bytes_read] = 0;
                CloseHandle(file_handle);
                return file_memory;
            }
            else
            {
                ASSERT(false);
                //free(file_memory);
            }
        }
        else
        {
            ASSERT(false);
        }
    }
    else
    {
        ASSERT(false);
    }
    return nullptr;
}

struct Win32Dimension
{
    int width = 0;
    int height = 0;

    Win32Dimension(SDL_Window* window)
    {
        SDL_GetWindowSize(window, &width, &height);
    }
};

INTERNAL
bool Win32Init(int window_width, int window_height, SDL_Window** window, SDL_GLContext* context)
{
    if (SDL_Init(SDL_INIT_VIDEO) >= 0)
    {
        *window = SDL_CreateWindow("RoguelikeGame", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
        if (window)
        {
            *context = SDL_GL_CreateContext(*window);
            if (*context)
            {
                glewExperimental = true;
                if (glewInit() == GLEW_OK)
                {
                    return true;
                }
                else
                {
                    SDL_GL_DeleteContext(*context);
                    *context = nullptr;
                    return false;
                }
            }
            else
            {
                //const char* error = SDL_GetError();
                SDL_DestroyWindow(*window);
                *window = nullptr;
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
}

INTERNAL
void Win32Shutdown(SDL_Window* window, SDL_GLContext context)
{
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    LEAK_CHECKS();

    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;
    if (Win32Init(800, 600, &window, &context))
    {
        {
            //GameState game_state;
            GameState* game_state = new GameState;

            // Timer
            std::chrono::time_point<std::chrono::system_clock> last_time, current_time;
            last_time = std::chrono::system_clock::now();
            current_time = std::chrono::system_clock::now();
            float dt = 0.0f;

            Input input = {};

            bool running = true;
            while (running)
            {
                current_time = std::chrono::system_clock::now();
                std::chrono::duration<float> duration = current_time - last_time;
                dt = duration.count();

                for (bool& key : input.pressed.keys)
                    key = false;
                //input.pressed.left = false; input.pressed.right = false;
                //input.pressed.up = false; input.pressed.down = false;

                SDL_Event e;
                while (SDL_PollEvent(&e) != 0)
                {
                    switch (e.type)
                    {
                    case SDL_QUIT:
                        running = false;
                        break;

                    case SDL_KEYDOWN:
                        switch (e.key.keysym.sym)
                        {
                        case SDLK_q:
                            if (e.key.repeat == 0)
                                input.pressed.rotate_left = true;
                            input.down.rotate_left = true;
                            break;
                        case SDLK_e:
                            if (e.key.repeat == 0)
                                input.pressed.rotate_right = true;
                            input.down.rotate_right = true;
                            break;
                        case SDLK_a:
                            if (e.key.repeat == 0)
                                input.pressed.move_left = true;
                            input.down.move_left = true;
                            break;
                        case SDLK_d:
                            if (e.key.repeat == 0)
                                input.pressed.move_right = true;
                            input.down.move_right = true;
                            break;
                        case SDLK_w:
                            if (e.key.repeat == 0)
                                input.pressed.move_front = true;
                            input.down.move_front = true;
                            break;
                        case SDLK_s:
                            if (e.key.repeat == 0)
                                input.pressed.move_back = true;
                            input.down.move_back = true;
                            break;
                        }
                        break;

                    case SDL_KEYUP:
                        switch (e.key.keysym.sym)
                        {
                        case SDLK_q:
                            input.down.rotate_left = false;
                            break;
                        case SDLK_e:
                            input.down.rotate_right = false;
                            break;
                        case SDLK_a:
                            input.down.move_left = false;
                            break;
                        case SDLK_d:
                            input.down.move_right = false;
                            break;
                        case SDLK_w:
                            input.down.move_front = false;
                            break;
                        case SDLK_s:
                            input.down.move_back = false;
                            break;
                        }
                        break;
                    }
                }

                game_state->Update(dt, input);

                Win32Dimension dim(window);

                glViewport(0, 0, dim.width, dim.height);
                game_state->Render((float)dim.width / (float)dim.height);

                SDL_GL_SwapWindow(window);

                last_time = current_time;
            }

            delete game_state; game_state = nullptr;

        }

        Win32Shutdown(window, context);
    }
    return 0;
}
