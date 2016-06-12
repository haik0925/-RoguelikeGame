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
        *window = SDL_CreateWindow("OldSchoolFPS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
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
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;
    if (Win32Init(1024, 768, &window, &context))
    {
        {
            GameState game_state;

            // Timer
            std::chrono::time_point<std::chrono::system_clock> last_time, current_time;
            last_time = std::chrono::system_clock::now();
            current_time = std::chrono::system_clock::now();
            float dt = 0.0f;

            Input input;

            bool running = true;
            while (running)
            {
                current_time = std::chrono::system_clock::now();
                std::chrono::duration<float> duration = current_time - last_time;
                dt = duration.count();

                input.pressed.left = false; input.pressed.right = false;
                input.pressed.up = false; input.pressed.down = false;

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
                        case SDLK_LEFT:
                            if (e.key.repeat == 0)
                                input.pressed.left = true;
                            input.down.left = true;
                            break;
                        case SDLK_RIGHT:
                            if (e.key.repeat == 0)
                                input.pressed.right = true;
                            input.down.right = true;
                            break;
                        case SDLK_UP:
                            if (e.key.repeat == 0)
                                input.pressed.up = true;
                            input.down.up = true;
                            break;
                        case SDLK_DOWN:
                            if (e.key.repeat == 0)
                                input.pressed.down = true;
                            input.down.down = true;
                            break;
                        }
                        break;

                    case SDL_KEYUP:
                        switch (e.key.keysym.sym)
                        {
                        case SDLK_LEFT:
                            input.down.left = false;
                            break;
                        case SDLK_RIGHT:
                            input.down.right = false;
                            break;
                        case SDLK_UP:
                            input.down.up = false;
                            break;
                        case SDLK_DOWN:
                            input.down.down = false;
                            break;
                        }
                        break;
                    }
                }

                game_state.Update(dt, input);

                Win32Dimension dim(window);

                glViewport(0, 0, dim.width, dim.height);
                game_state.Render((float)dim.width / (float)dim.height);

                SDL_GL_SwapWindow(window);

                last_time = current_time;
            }
        }

        Win32Shutdown(window, context);
    }
    return 0;
}
