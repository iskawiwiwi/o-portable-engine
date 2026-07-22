#include "Engine.h"
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <cmath>

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
        
        // --- 1. Одиночные нажатия клавиш (Тоггл Аллайна) ---
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_g) { 
                useGridAlign = !useGridAlign; // Включаем/Выключаем прилипание
                std::cout << "Grid Align is now: " << (useGridAlign ? "ON" : "OFF") << std::endl;
            }
        }
    }

    // --- 2. НЕПРЕРЫВНОЕ ЗАЖАТИЕ МЫШИ (Режим "Кисти") ---
    Uint32 mouseState = SDL_GetMouseState(nullptr, nullptr);
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    int cellX = mouseX / CELL_SIZE;
    int cellY = mouseY / CELL_SIZE;

    // Проверяем, зажат ли Shift
    bool isShiftPressed = (SDL_GetModState() & KMOD_SHIFT) != 0;

    // Проверяем, что мышка не ушла за пределы окна
    if (cellX >= 0 && cellX < GRID_WIDTH && cellY >= 0 && cellY < GRID_HEIGHT) {
        
        // ЕСЛИ ЗАЖАТА ЛЕВАЯ КНОПКА МЫШИ (Рисуем)
        if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            
            if (isShiftPressed) {
                // --- РИСУЕМ СУЩНОСТИ (Shift + ЛКМ) ---
                float spawnX, spawnY;
                
                // Проверяем наш тоггл: прилипать к сетке или нет?
                if (useGridAlign) {
                    grid.Align(mouseX, mouseY, spawnX, spawnY);
                } else {
                    spawnX = mouseX;
                    spawnY = mouseY;
                }

                // ЗАЩИТА ОТ СПАМА КЛОНОВ: 
                // Проверяем, нет ли уже сущности ровно в этих координатах
                bool alreadyExists = false;
                for (const Entity& e : entities) {
                    if (std::abs(e.x - spawnX) < 1.0f && std::abs(e.y - spawnY) < 1.0f) {
                        alreadyExists = true;
                        break;
                    }
                }

                // Если место свободно — спавним!
                if (!alreadyExists) {
                    Entity newEntity(spawnX, spawnY, 16, 16, 0.2f, 0.5f, 0.9f);
                    entities.push_back(newEntity);
                }
            } else {
                // --- РИСУЕМ СТЕНЫ (Просто ЛКМ) ---
                grid.cells[cellY][cellX].isEmpty = false;
            }
        } 
        
        // ЕСЛИ ЗАЖАТА ПРАВАЯ КНОПКА МЫШИ (Стираем)
        else if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
            
            if (isShiftPressed) {
                // --- СТИРАЕМ СУЩНОСТИ (Shift + ПКМ) ---
                // Ищем сущность, на которую наведена мышка, и удаляем её
                auto it = entities.begin();
                while (it != entities.end()) {
                    // Если курсор мыши находится внутри квадрата сущности
                    if (mouseX >= it->x && mouseX <= it->x + it->width &&
                        mouseY >= it->y && mouseY <= it->y + it->height) {
                        it = entities.erase(it); // Удаляем из списка
                    } else {
                        ++it;
                    }
                }
            } else {
                // --- СТИРАЕМ СТЕНЫ (Просто ПКМ) ---
                grid.cells[cellY][cellX].isEmpty = true;
            }
        }
    }
}

void Engine::Update(float dt) {
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    player.Update(dt, keyboardState);
}

void Engine::Render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    grid.Draw();
    
    for (Entity& e : entities) {
        e.Draw();
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
