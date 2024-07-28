#include <raylib.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <algorithm>

class Component {
public:
    virtual ~Component() = default;
};

class Entity {
private:
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;

public:
    template<typename T, typename... Args>
    void addComponent(Args&&... args) {
        components[typeid(T)] = std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    T* getComponent() {
        auto it = components.find(typeid(T));
        return it != components.end() ? static_cast<T*>(it->second.get()) : nullptr;
    }

    template<typename T>
    void removeComponent() {
        components.erase(typeid(T));
    }
};

class World {
private:
    std::vector<std::unique_ptr<Entity>> entities;

public:
    Entity* createEntity() {
        entities.push_back(std::make_unique<Entity>());
        return entities.back().get();
    }

    void removeEntity(Entity* entity) {
        auto it = std::find_if(entities.begin(), entities.end(),
                               [entity](const auto& e) { return e.get() == entity; });
        if (it != entities.end()) {
            entities.erase(it);
        }
    }

    std::vector<Entity*> getEntities() {
      std::vector<Entity*> result;
        for (const auto& entity : entities) {
            result.push_back(entity.get());
        }
        return result;
    }
};

struct PositionComponent : Component {
    float x, y;
    PositionComponent(float x, float y) : x(x), y(y) {}
};

struct SpriteComponent : Component {
    Color color;
    SpriteComponent(Color color) : color(color) {}
};

void renderSystem(World& world) {
    for (auto entity : world.getEntities()) {
        auto position = entity->getComponent<PositionComponent>();
        auto sprite = entity->getComponent<SpriteComponent>();
        if (position && sprite) {
            DrawCircle(position->x, position->y, 10, sprite->color);
        }
    }
}

int main() {
    InitWindow(800, 600, "ECS Raylib Example");
    SetTargetFPS(60);

    World world;

    auto entity1 = world.createEntity();
    entity1->addComponent<PositionComponent>(100, 100);
    entity1->addComponent<SpriteComponent>(RED);

    auto entity2 = world.createEntity();
    entity2->addComponent<PositionComponent>(200, 200);
    entity2->addComponent<SpriteComponent>(BLUE);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        renderSystem(world);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
