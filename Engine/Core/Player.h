#pragma once
#include <SDL2/SDL.h>

class Player {
public:
    float x = 100.0f;
    float y = 100.0f;
    float width = 18.0f;
    float height = 18.0f;
    float speed = 200.0f;

    void Update(float dt, const Uint8* keyboardState);
    void Draw();
};
