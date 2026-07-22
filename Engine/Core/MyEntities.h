#pragma once
#include "Entity.h"
#include <cmath>

// ТВОЙ СОБСТВЕННЫЙ КЛАСС №1: Прыгающий враг
class SlimeEnemy : public Entity {
private:
    float timeAlive = 0.0f; // Уникальная переменная этого класса
    float startY;

public:
    // Конструктор (Событие Create)
    SlimeEnemy(uint32_t id, Registry* reg, float x, float y) : Entity(id, reg) {
        SetPos(x, y);
        SetColor(0.2f, 0.9f, 0.3f); // Зеленый слайм
        startY = y;
    }

    // Событие Step
    void Update(float dt) override {
        timeAlive += dt;
        
        // Достаем X из ECS и заставляем его прыгать по синусоиде!
        float currentX = reg->transforms[id].x;
        SetPos(currentX, startY + std::sin(timeAlive * 10.0f) * 10.0f);
    }
};

// ТВОЙ СОБСТВЕННЫЙ КЛАСС №2: Патрулирующий бот
class PatrolBot : public Entity {
private:
    float speed = 50.0f;

public:
    PatrolBot(uint32_t id, Registry* reg, float x, float y) : Entity(id, reg) {
        SetPos(x, y);
        SetColor(0.9f, 0.2f, 0.2f); // Красный бот
    }

    void Update(float dt) override {
        float x = reg->transforms[id].x;
        float y = reg->transforms[id].y;
        
        x += speed * dt;
        
        // Если ушел слишком далеко, разворачиваем
        if (x > 500.0f || x < 10.0f) speed = -speed; 

        SetPos(x, y);
    }
};
