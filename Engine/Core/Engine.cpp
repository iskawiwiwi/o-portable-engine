#include "Engine.h"
#include <SDL2/SDL_opengl.h>
#include <iostream>

Engine::Engine() {}

Engine::~Engine() {
    if (glContext) SDL_GL_DeleteContext(glContext);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Engine::Init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!window) return false;

    glContext = SDL_GL_CreateContext(window);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    isRunning = true;
    lastTime = SDL_GetPerformanceCounter();
    return true;
}

void Engine::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
    }

    // Ввод мыши для редактора
    Uint32 mouseState = SDL_GetMouseState(nullptr, nullptr);
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    int cellX = mouseX / CELL_SIZE;
    int cellY = mouseY / CELL_SIZE;

    if (cellX >= 0 && cellX < GRID_WIDTH && cellY >= 0 && cellY < GRID_HEIGHT) {
        if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            grid.cells[cellY][cellX].isEmpty = false;
        } else if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
            grid.cells[cellY][cellX].isEmpty = true;
        }
    }
}

void Engine::Update(float dt) {
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    player.Update(dt, keyboardState, grid);
}

void Engine::Render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    grid.Draw();
    player.Draw();

    SDL_GL_SwapWindow(window);
}

void Engine::Run() {
    while (isRunning) {
        Uint64 currentTime = SDL_GetPerformanceCounter();
        float dt = static_cast<float>(currentTime - lastTime) / static_cast<float>(SDL_GetPerformanceFrequency());
        lastTime = currentTime;

        if (dt > 0.05f) dt = 0.05f;

        ProcessInput();
        Update(dt);
        Render();

        SDL_Delay(1);
    }
}
