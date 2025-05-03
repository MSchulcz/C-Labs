#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Inventory {
private:
    std::vector<std::unique_ptr<std::string>> items; // Динамический массив строк

public:
    // Добавление предмета в инвентарь
    void addItem(const std::string& item) {
        items.push_back(std::make_unique<std::string>(item));
    }

    // Отображение содержимого инвентаря
    void displayInventory() const {
        if (items.empty()) {
            std::cout << "Inventory is empty." << std::endl;
            return;
        }

        std::cout << "Inventory items:" << std::endl;
        for (size_t i = 0; i < items.size(); ++i) {
            std::cout << "- " << *items[i] << std::endl;
        }
    }
};

int main() {
    // Пример использования std::unique_ptr с полиморфизмом (из примера)
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

    // Массив умных указателей на Entity
    std::unique_ptr<Entity> entities[] = {
        std::make_unique<Player>("Hero", 100, 1),
        std::make_unique<Enemy>("Goblin", 50, "Goblin")
    };

    // Полиморфизм и автоматическое управление памятью
    for (const auto& entity : entities) {
        entity->displayInfo();
    }

    // Демонстрация работы Inventory
    Inventory inventory;
    inventory.addItem("Sword");
    inventory.addItem("Health Potion");
    inventory.addItem("Shield");

    inventory.displayInventory();

    return 0;
}