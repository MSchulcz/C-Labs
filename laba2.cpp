#include <iostream>
#include <string>

class Character {
private:
    std::string name;
    int health;
    int attack;
    int defense;

public:
    // Конструктор
    Character(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {
        std::cout << "Character " << name << " created!\n";
    }

    // Деструктор
    ~Character() {
        std::cout << "Character " << name << " destroyed!\n";
    }

    void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health
                  << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }
};

class Monster {
private:
    std::string name;
    int health;
    int attack;
    int defense;

public:
    // Конструктор
    Monster(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {
        std::cout << "Monster " << name << " created!\n";
    }

    // Деструктор
    ~Monster() {
        std::cout << "Monster " << name << " destroyed!\n";
    }

    void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health
                  << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }
};

class Weapon {
private:
    std::string name;
    int damage;
    float weight;

public:
    // Конструктор
    Weapon(const std::string& n, int d, float w)
        : name(n), damage(d), weight(w) {
        std::cout << "Weapon " << name << " created!\n";
    }

    // Деструктор
    ~Weapon() {
        std::cout << "Weapon " << name << " destroyed!\n";
    }

    void displayInfo() const {
        std::cout << "Weapon: " << name << ", Damage: " << damage 
                  << ", Weight: " << weight << " kg" << std::endl;
    }
};

int main() {
    // Создание персонажей
    Character hero("Hero", 100, 20, 10);
    Monster goblin("Goblin", 50, 15, 5);

    // Создание оружия
    Weapon sword("Steel Sword", 15, 2.5);
    Weapon axe("Battle Axe", 25, 5.0);
    Weapon bow("Long Bow", 10, 1.2);

    // Вывод информации
    std::cout << "\n=== Character Info ===" << std::endl;
    hero.displayInfo();

    std::cout << "\n=== Monster Info ===" << std::endl;
    goblin.displayInfo();

    std::cout << "\n=== Weapons Info ===" << std::endl;
    sword.displayInfo();
    axe.displayInfo();
    bow.displayInfo();

    return 0;
}
