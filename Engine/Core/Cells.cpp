#include "Cells.h"
#include <algorithm>

Grid::Grid() {
    cells.resize(GRID_HEIGHT, std::vector<Cell>(GRID_WIDTH));
    Clear();
}

void Grid::Clear() {
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            cells[y][x].isEmpty = true;
        }
    }
}

void Grid::Align(float inX, float inY, float& outX, float& outY) {
    int cellX = static_cast<int>(inX) / CELL_SIZE;
    int cellY = static_cast<int>(inY) / CELL_SIZE;
    
    outX = cellX * CELL_SIZE;
    outY = cellY * CELL_SIZE;
}

void Grid::Draw() {
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            if (cells[y][x].isEmpty) {
                glColor3f(0.15f, 0.15f, 0.15f);
            } else {
                glColor3f(cells[y][x].r, cells[y][x].g, cells[y][x].b);
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
}
