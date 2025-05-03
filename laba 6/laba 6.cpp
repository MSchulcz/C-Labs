#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <stdexcept>

// Базовый класс Entity
class Entity {
public:
    virtual ~Entity() = default;
    virtual void displayInfo() const = 0;
    virtual int getHealth() const = 0;
};

class Player : public Entity {
    std::string name;
    int health;
    int level;

public:
    Player(const std::string& n, int h, int l) : name(n), health(h), level(l) {}
    void displayInfo() const override {
        std::cout << "Player: " << name << ", HP: " << health << ", Level: " << level << std::endl;
    }
    int getHealth() const override { return health; }
};

class Enemy : public Entity {
    std::string name;
    int health;
    std::string type;

public:
    Enemy(const std::string& n, int h, const std::string& t) : name(n), health(h), type(t) {}
    void displayInfo() const override {
        std::cout << "Enemy: " << name << ", HP: " << health << ", Type: " << type << std::endl;
    }
    int getHealth() const override { return health; }
};

// Шаблонный класс GameManager с обработкой исключений
template <typename T>
class GameManager {
private:
    std::vector<T> entities;

public:
    void addEntity(const T& entity) {
        if (entity->getHealth() <= 0) {
            throw std::invalid_argument("Entity has invalid health (HP <= 0)");
        }
        entities.push_back(entity);
    }

    void displayAll() const {
        for (const auto& entity : entities) {
            entity->displayInfo();
        }
    }
};

// Шаблонный класс Queue с обработкой исключений
template <typename T>
class Queue {
private:
    std::queue<T> items;

public:
    void push(const T& item) {
        items.push(item);
    }

    void pop() {
        if (items.empty()) {
            throw std::runtime_error("Cannot pop from empty queue");
        }
        items.pop();
    }

    void display() const {
        if (items.empty()) {
            throw std::runtime_error("Queue is empty");
        }
        
        std::queue<T> temp = items;
        while (!temp.empty()) {
            std::cout << temp.front() << " ";
            temp.pop();
        }
        std::cout << std::endl;
    }
};

int main() {
    // Тестирование GameManager с исключениями
    try {
        GameManager<std::unique_ptr<Entity>> manager;
        manager.addEntity(std::make_unique<Player>("Hero", 100, 1));
        manager.addEntity(std::make_unique<Enemy>("Goblin", 50, "Goblin"));
        manager.addEntity(std::make_unique<Player>("Invalid", -50, 1)); // Вызовет исключение
    } catch (const std::invalid_argument& e) {
        std::cerr << "GameManager error: " << e.what() << std::endl;
    }

    // Тестирование Queue с исключениями
    try {
        Queue<int> intQueue;
        intQueue.push(10);
        intQueue.push(20);
        intQueue.display();
        
        intQueue.pop();
        intQueue.pop();
        intQueue.pop(); // Вызовет исключение
    } catch (const std::runtime_error& e) {
        std::cerr << "Queue error: " << e.what() << std::endl;
    }

    try {
        Queue<std::string> stringQueue;
        stringQueue.display(); // Вызовет исключение (очередь пуста)
    } catch (const std::runtime_error& e) {
        std::cerr << "Queue error: " << e.what() << std::endl;
    }

    return 0;
}