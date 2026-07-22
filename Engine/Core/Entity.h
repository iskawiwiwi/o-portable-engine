#pragma once
#include "Registry.h"

// БАЗОВЫЙ КЛАСС (Скелет)
class Entity {
public:
    uint32_t id;
    Registry* reg; // Ссылка на подвал
    float x = 0.0f;
    float y = 0.0f;
    float width = 16.0f;
    float height = 16.0f;

    Entity(uint32_t id, Registry* reg);
    virtual ~Entity() = default;

    uint32_t GetId() const { return id; }

    // Удобные функции-обертки для тебя (Фасад)
    void SetPos(float x, float y);
    void SetColor(float r, float g, float b);
    void SetSize(float w, float h);

    // Виртуальная функция! Именно её ты будешь переписывать в своих префабах
    virtual void Update(float dt) {}
};
