#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>

// Базовый класс Entity
class Entity {
protected:
    std::string name;
    int health;
    int attack;
    int defense;

public:
    Entity(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {}

    // Виртуальный метод для атаки
    virtual void performAttack(Entity& target) {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        } else {
            std::cout << name << " attacks " << target.getName() << ", but it has no effect!\n";
        }
    }

    // Методы доступа к защищенным полям
    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    void takeDamage(int amount) { health -= amount; }

    // Виртуальный метод для вывода информации
    virtual void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health
                  << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }

    // Виртуальный метод для лечения
    virtual void heal(int amount) {
        health += amount;
        if (health > 100) health = 100;
        std::cout << name << " healed for " << amount << " HP. Current HP: " << health << std::endl;
    }

    virtual ~Entity() {}
};

// Класс Character с критическим ударом
class Character : public Entity {
public:
    Character(const std::string& n, int h, int a, int d)
        : Entity(n, h, a, d) {}

    void performAttack(Entity& target) override {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            // 20% шанс критического удара
            if (rand() % 100 < 20) {
                damage *= 2;
                std::cout << "Critical hit! ";
            }
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        } else {
            std::cout << name << " attacks " << target.getName() << ", but it has no effect!\n";
        }
    }

    void displayInfo() const override {
        std::cout << "Character: " << name << ", HP: " << health
                  << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }
};

// Класс Monster с ядовитой атакой
class Monster : public Entity {
public:
    Monster(const std::string& n, int h, int a, int d)
        : Entity(n, h, a, d) {}

    void performAttack(Entity& target) override {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            // 30% шанс ядовитой атаки
            if (rand() % 100 < 30) {
                damage += 5;
                std::cout << "Poisonous attack! ";
            }
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        } else {
            std::cout << name << " attacks " << target.getName() << ", but it has no effect!\n";
        }
    }

    void displayInfo() const override {
        std::cout << "Monster: " << name << ", HP: " << health
                  << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }
};

// Класс Boss с огненной атакой
class Boss : public Monster {
private:
    std::string specialAbility;

public:
    Boss(const std::string& n, int h, int a, int d, const std::string& ability)
        : Monster(n, h, a, d), specialAbility(ability) {}

    void performAttack(Entity& target) override {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            // 50% шанс огненной атаки
            if (rand() % 100 < 50) {
                damage += 10;
                std::cout << "Fire attack! ";
            }
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        } else {
            std::cout << name << " attacks " << target.getName() << ", but it has no effect!\n";
        }
    }

    void displayInfo() const override {
        Monster::displayInfo();
        std::cout << "Special Ability: " << specialAbility << std::endl;
    }
};

int main() {
    srand(static_cast<unsigned>(time(0))); // Инициализация генератора случайных чисел

    // Создание объектов
    Character hero("Hero", 100, 20, 10);
    Monster goblin("Goblin", 50, 15, 5);
    Boss dragon("Dragon", 200, 30, 20, "Fire Breath");

    // Массив указателей на базовый класс
    Entity* entities[] = { &hero, &goblin, &dragon };

    // Вывод информации о сущностях
    std::cout << "=== Initial Status ===" << std::endl;
    for (auto& entity : entities) {
        entity->displayInfo();
    }
    std::cout << std::endl;

    // Демонстрация полиморфизма в бою
    std::cout << "=== Combat ===" << std::endl;
    hero.performAttack(goblin);
    goblin.performAttack(hero);
    dragon.performAttack(hero);
    std::cout << std::endl;

    // Демонстрация лечения
    std::cout << "=== Healing ===" << std::endl;
    hero.heal(20);
    std::cout << std::endl;

    // Финальный статус
    std::cout << "=== Final Status ===" << std::endl;
    for (auto& entity : entities) {
        entity->displayInfo();
    }

    return 0;
}
