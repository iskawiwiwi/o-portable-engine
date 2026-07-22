#pragma once

// Базовый класс для всех объектов в игре
class Entity {
public:
    float x, y;
    float width, height;
    float r, g, b; // Цвет сущности

    // Конструктор: задаем позицию, размер и цвет при создании
    Entity(float startX, float startY, float w, float h, float red, float green, float blue);

    void Draw();
};
