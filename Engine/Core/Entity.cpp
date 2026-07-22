#include "Entity.h"
#include <SDL2/SDL_opengl.h>

Entity::Entity(float startX, float startY, float w, float h, float red, float green, float blue) {
    x = startX;
    y = startY;
    width = w;
    height = h;
    r = red;
    g = green;
    b = blue;
}

void Entity::Draw() {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
    glEnd();
}
