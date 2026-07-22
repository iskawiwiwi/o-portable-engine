#pragma once
#include <SDL2/SDL.h>
#include "Cells.h" // Подключаем сетку, чтобы уметь с ней сталкиваться

class Player {
public:
    float x = 100.0f;
    float y = 100.0f;
    float width = 16.0f;
    float height = 16.0f;
    float speed = 200.0f;

    void Update(float dt, const Uint8* keyboardState, Grid& grid);
    void Draw();
};
