#pragma once
#include <vector>
#include <cstdint>

// 1. КОМПОНЕНТЫ (Просто структуры с данными)
struct CTransform { 
    float x, y; 
};

struct CSprite { 
    float width, height;
    float r, g, b; 
};

// 2. РЕЕСТР (Хранилище массивов)
class Registry {
public:
    // Те самые плотные массивы данных, которые обожает процессор!
    std::vector<CTransform> transforms;
    std::vector<CSprite> sprites;
    std::vector<bool> active; // Существует ли сущность?

    // Функция выдает новый ID и выделяет место в массивах
    uint32_t CreateEntity() {
        uint32_t id = active.size();
        
        transforms.push_back({0.0f, 0.0f});
        sprites.push_back({16.0f, 16.0f, 1.0f, 1.0f, 1.0f}); // По умолчанию белый квадрат 16x16
        active.push_back(true);
        
        return id;
    }

    void DestroyEntity(uint32_t id) {
        if (id < active.size()) {
            active[id] = false;
        }
    }
};
