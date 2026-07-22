#pragma once
#include <SDL2/SDL.h>
#include "Cells.h"
#include "Player.h"

class Engine {
private:
    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;
    bool isRunning = false;

    Grid grid;
    Player player;

    Uint64 lastTime = 0;

    void ProcessInput();
    void Update(float dt);
    void Render();

public:
    Engine();
    ~Engine();
    bool Init(const char* title, int width, int height);
    void Run();
};
