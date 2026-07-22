#include "Entity.h"

Entity::Entity(uint32_t id, Registry* reg) {
    this->id = id;
    this->reg = reg;
}

// Заметь: мы меняем данные НЕ в классе, а напрямую в массивах ECS!
void Entity::SetPos(float x, float y) {
    if (!reg || id >= reg->transforms.size()) return;

    reg->transforms[id].x = x;
    reg->transforms[id].y = y;
    this->x = x;
    this->y = y;
}

void Entity::SetColor(float r, float g, float b) {
    if (!reg || id >= reg->sprites.size()) return;

    reg->sprites[id].r = r;
    reg->sprites[id].g = g;
    reg->sprites[id].b = b;
}

void Entity::SetSize(float w, float h) {
    if (!reg || id >= reg->sprites.size()) return;

    reg->sprites[id].width = w;
    reg->sprites[id].height = h;
    this->width = w;
    this->height = h;
}
