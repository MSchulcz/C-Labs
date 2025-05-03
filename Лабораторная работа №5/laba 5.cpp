#include <iostream>
#include <vector>
#include <queue>
#include <memory>

// Базовый класс Entity (для примера GameManager)
class Entity {
public:
    virtual ~Entity() = default;
    virtual void displayInfo() const = 0;
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
};

// Шаблонный класс GameManager
template <typename T>
class GameManager {
private:
    std::vector<T> entities;

public:
    void addEntity(const T& entity) {
        entities.push_back(entity);
    }

    void displayAll() const {
        for (const auto& entity : entities) {
            entity->displayInfo();
        }
    }
};

// Шаблонный класс Queue
template <typename T>
class Queue {
private:
    std::queue<T> items;

public:
    void push(const T& item) {
        items.push(item);
    }

    void pop() {
        if (!items.empty()) {
            items.pop();
        }
    }

    void display() const {
        std::queue<T> temp = items;
        while (!temp.empty()) {
            std::cout << temp.front() << " ";
            temp.pop();
        }
        std::cout << std::endl;
    }
};

int main() {
    // Пример работы GameManager (с умными указателями)
    GameManager<std::unique_ptr<Entity>> manager;
    manager.addEntity(std::make_unique<Player>("Hero", 100, 1));
    manager.addEntity(std::make_unique<Enemy>("Goblin", 50, "Goblin"));
    std::cout << "GameManager output:" << std::endl;
    manager.displayAll();

    // Пример работы Queue с числами
    Queue<int> intQueue;
    intQueue.push(10);
    intQueue.push(20);
    intQueue.push(30);
    std::cout << "\nInteger Queue:" << std::endl;
    intQueue.display();
    intQueue.pop();
    std::cout << "After pop:" << std::endl;
    intQueue.display();

    // Пример работы Queue со строками
    Queue<std::string> stringQueue;
    stringQueue.push("Hello");
    stringQueue.push("World");
    stringQueue.push("C++");
    std::cout << "\nString Queue:" << std::endl;
    stringQueue.display();
    stringQueue.pop();
    std::cout << "After pop:" << std::endl;
    stringQueue.display();

    return 0;
}