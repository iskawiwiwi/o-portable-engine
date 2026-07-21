#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <vector>
#include <algorithm>

// Constants
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 700;
const int CELL_SIZE = 15;
const int GRID_WIDTH = SCREEN_WIDTH / CELL_SIZE;
const int GRID_HEIGHT = SCREEN_HEIGHT / CELL_SIZE;

// Cell structure
struct Cell {
    bool isEmpty = true;
    float r = 0.8f, g = 0.3f, b = 0.3f;
};

// Global Grid
std::vector<std::vector<Cell>> grid(GRID_HEIGHT, std::vector<Cell>(GRID_WIDTH));

// Initialize grid
void CreateCanvas() {
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            grid[y][x].isEmpty = true;
        }
    }
}

// Check if a bounding box overlaps with any filled cell
bool CheckCollision(float x, float y, float w, float h) {
    int startX = std::max(0, static_cast<int>(x / CELL_SIZE));
    int endX   = std::min(GRID_WIDTH - 1, static_cast<int>((x + w - 0.01f) / CELL_SIZE));
    int startY = std::max(0, static_cast<int>(y / CELL_SIZE));
    int endY   = std::min(GRID_HEIGHT - 1, static_cast<int>((y + h - 0.01f) / CELL_SIZE));

    for (int cy = startY; cy <= endY; ++cy) {
        for (int cx = startX; cx <= endX; ++cx) {
            if (!grid[cy][cx].isEmpty) {
                return true; // Врезались в стену!
            }
        }
    }
    return false;
}

// Player structure
struct Player {
    float x = 100.0f;
    float y = 100.0f;
    float width = 15.0f;  // Чуть меньше размера клетки (20), чтобы пролезать в проходы
    float height = 15.0f;
    float speed = 200.0f; // Пикселей в секунду

    void Update(float dt, const Uint8* keyboardState) {
        float moveX = 0.0f;
        float moveY = 0.0f;

        if (keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT])  moveX -= 1.0f;
        if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT]) moveX += 1.0f;
        if (keyboardState[SDL_SCANCODE_W] || keyboardState[SDL_SCANCODE_UP])    moveY -= 1.0f;
        if (keyboardState[SDL_SCANCODE_S] || keyboardState[SDL_SCANCODE_DOWN])  moveY += 1.0f;

        // Нормализация диагонального движения
        if (moveX != 0.0f && moveY != 0.0f) {
            moveX *= 0.7071f;
            moveY *= 0.7071f;
        }

        // 1. Движение по оси X и обработка коллизии
        float nextX = x + moveX * speed * dt;
        if (!CheckCollision(nextX, y, width, height)) {
            x = nextX;
        }

        // 2. Движение по оси Y и обработка коллизии
        float nextY = y + moveY * speed * dt;
        if (!CheckCollision(x, nextY, width, height)) {
            y = nextY;
        }
    }

    void Draw() {
        // Рисуем игрока ярко-зеленым квадратом
        glColor3f(0.2f, 0.9f, 0.3f);
        glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + width, y);
            glVertex2f(x + width, y + height);
            glVertex2f(x, y + height);
        glEnd();
    }
};

// Render function
void DrawCanvas(SDL_Window* window, Player& player) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT);

    // 1. Отрисовка сетки
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            if (grid[y][x].isEmpty) {
                glColor3f(0.15f, 0.15f, 0.15f); // Фон пустой клетки
            } else {
                glColor3f(grid[y][x].r, grid[y][x].g, grid[y][x].b); // Стена
            }

            float x1 = x * CELL_SIZE;
            float y1 = y * CELL_SIZE;
            float x2 = x1 + CELL_SIZE - 1.0f; 
            float y2 = y1 + CELL_SIZE - 1.0f;

            glBegin(GL_QUADS);
                glVertex2f(x1, y1);
                glVertex2f(x2, y1);
                glVertex2f(x2, y2);
                glVertex2f(x1, y2);
            glEnd();
        }
    }

    // 2. Отрисовка игрока поверх сетки
    player.Draw();

    SDL_GL_SwapWindow(window);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_Window* window = SDL_CreateWindow(
        "O Engine - Controller & Grid Editor Test",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!window) return 1;

    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    // Настройка 2D Орто-проекции
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    CreateCanvas();

    Player player;
    bool running = true;
    SDL_Event event;

    // Таймер для расчета Delta Time
    Uint64 lastTime = SDL_GetPerformanceCounter();

    while (running) {
        // 1. Вычисляем Delta Time (в секундах)
        Uint64 currentTime = SDL_GetPerformanceCounter();
        float dt = static_cast<float>(currentTime - lastTime) / static_cast<float>(SDL_GetPerformanceFrequency());
        lastTime = currentTime;

        // Защита от резких скачков dt (например при перетаскивании окна)
        if (dt > 0.05f) dt = 0.05f; 

        // 2. Обработка событий и кликов мыши (Редактор)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Зажатие кнопок мыши для «рисования» стен
        Uint32 mouseState = SDL_GetMouseState(nullptr, nullptr);
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        
        int cellX = mouseX / CELL_SIZE;
        int cellY = mouseY / CELL_SIZE;

        if (cellX >= 0 && cellX < GRID_WIDTH && cellY >= 0 && cellY < GRID_HEIGHT) {
            if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                grid[cellY][cellX].isEmpty = false; // Поставить стену
            } else if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
                grid[cellY][cellX].isEmpty = true;  // Стереть стену
            }
        }

        // 3. Обновление игрока (Ввод клавиатуры)
        const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
        player.Update(dt, keyboardState);

        // 4. Отрисовка
        DrawCanvas(window, player);

        // Ограничение ~60 FPS
        SDL_Delay(1);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
