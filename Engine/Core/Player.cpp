#include "Player.h"
#include <SDL2/SDL_opengl.h>

void Player::Update(float dt, const Uint8* keyboardState, Grid& grid) {
    float moveX = 0.0f;
    float moveY = 0.0f;

    if (keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT])  moveX -= 1.0f;
    if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT]) moveX += 1.0f;
    if (keyboardState[SDL_SCANCODE_W] || keyboardState[SDL_SCANCODE_UP])    moveY -= 1.0f;
    if (keyboardState[SDL_SCANCODE_S] || keyboardState[SDL_SCANCODE_DOWN])  moveY += 1.0f;

    if (moveX != 0.0f && moveY != 0.0f) {
        moveX *= 0.7071f;
        moveY *= 0.7071f;
    }

    // Движение по X
    float nextX = x + moveX * speed * dt;
    if (!grid.CheckCollision(nextX, y, width, height)) {
        x = nextX;
    }

    // Движение по Y
    float nextY = y + moveY * speed * dt;
    if (!grid.CheckCollision(x, nextY, width, height)) {
        y = nextY;
    }
}

void Player::Draw() {
    glColor3f(0.2f, 0.9f, 0.3f);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
    glEnd();
}
