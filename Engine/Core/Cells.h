#pragma once
#include <vector>
#include <SDL2/SDL_opengl.h>

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 700;
const int CELL_SIZE = 20;
const int GRID_WIDTH = SCREEN_WIDTH / CELL_SIZE;
const int GRID_HEIGHT = SCREEN_HEIGHT / CELL_SIZE;

struct Cell {
    bool isEmpty = true;
    float r = 0.8f, g = 0.3f, b = 0.3f;
};

class Grid {
public:
    std::vector<std::vector<Cell>> cells;

    Grid();
    void Clear();
    bool CheckCollision(float x, float y, float w, float h);
    void Draw();
};
