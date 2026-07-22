#include "Engine.h"
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <cmath>

Engine::Engine() {}

Engine::~Engine() {
    for (Entity* entity : entities) {
        delete entity;
    }
    entities.clear();

    if (glContext) {
        SDL_GL_DeleteContext(glContext);
        glContext = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

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

    if (!window) {
        std::cerr << "SDL window error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "OpenGL context error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        window = nullptr;
        SDL_Quit();
        return false;
    }

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

        // --- Одиночные нажатия клавиш ---
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_g) {
                useGridAlign = !useGridAlign;
                std::cout << "Grid Align is now: "
                          << (useGridAlign ? "ON" : "OFF")
                          << std::endl;
            }
        }
    }

    // --- НЕПРЕРЫВНОЕ ЗАЖАТИЕ МЫШИ ---
    int mouseX, mouseY;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

    int cellX = mouseX / CELL_SIZE;
    int cellY = mouseY / CELL_SIZE;

    bool isShiftPressed = (SDL_GetModState() & KMOD_SHIFT) != 0;

    if (cellX >= 0 && cellX < GRID_WIDTH &&
        cellY >= 0 && cellY < GRID_HEIGHT) {

        // ==========================================================
        // ЛЕВАЯ КНОПКА МЫШИ
        // ==========================================================
        if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {

            if (isShiftPressed) {
                // --- РИСУЕМ СУЩНОСТИ (Shift + ЛКМ) ---
                float spawnX, spawnY;

                if (useGridAlign)
                    grid.Align(mouseX, mouseY, spawnX, spawnY);
                else {
                    spawnX = mouseX;
                    spawnY = mouseY;
                }

                // Проверяем через ECS Registry
                bool alreadyExists = false;
                for (uint32_t i = 0; i < registry.active.size(); i++) {
                    if (registry.active[i] &&
                        std::abs(registry.transforms[i].x - spawnX) < 1.0f &&
                        std::abs(registry.transforms[i].y - spawnY) < 1.0f) {
                        alreadyExists = true;
                        break;
                    }
                }

                if (!alreadyExists) {
                    // Создаем ECS-сущность
                    uint32_t newId = registry.CreateEntity();

                    // Создаем игровой объект
                    entities.push_back(
                        new SlimeEnemy(newId, &registry, spawnX, spawnY)
                    );

                    // Для патрульного:
                    // entities.push_back(
                    //     new PatrolBot(newId, &registry, spawnX, spawnY)
                    // );
                }
            }
            else {
                // --- РИСУЕМ СТЕНЫ ---
                grid.cells[cellY][cellX].isEmpty = false;
            }
        }

        // ==========================================================
        // ПРАВАЯ КНОПКА МЫШИ
        // ==========================================================
        else if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {

            if (isShiftPressed) {
                // --- СТИРАЕМ СУЩНОСТИ ---
                auto it = entities.begin();
                while (it != entities.end()) {

                    if (mouseX >= (*it)->x &&
						mouseX <= (*it)->x + (*it)->width &&
						mouseY >= (*it)->y &&
						mouseY <= (*it)->y + (*it)->height) {

                        registry.DestroyEntity((*it)->GetId());
                        delete *it;
                        it = entities.erase(it);
                    }
                    else {
                        ++it;
                    }
                }
            }
            else {
                // --- СТИРАЕМ СТЕНЫ ---
                grid.cells[cellY][cellX].isEmpty = true;
            }
        }
    }
}

void Engine::Update(float dt) {
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    player.Update(dt, keyboardState);

    // 1. ООП ЛОГИКА: Мы вызываем Update() у твоих классов
    for (Entity* e : entities) {
        e->Update(dt); 
    }
}

void Engine::Render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    grid.Draw();
    
    // 2. ECS РЕНДЕР: Мы ВООБЩЕ НЕ ИСПОЛЬЗУЕМ классы! 
    // Видеокарта читает напрямую плотные сырые массивы из Подвала!
    for (uint32_t i = 0; i < registry.active.size(); i++) {
        if (!registry.active[i]) continue;
        
        CTransform& t = registry.transforms[i];
        CSprite& s = registry.sprites[i];

        glColor3f(s.r, s.g, s.b);
        glBegin(GL_QUADS);
            glVertex2f(t.x, t.y);
            glVertex2f(t.x + s.width, t.y);
            glVertex2f(t.x + s.width, t.y + s.height);
            glVertex2f(t.x, t.y + s.height);
        glEnd();
    }

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
