#include <iostream>
#include <string>

class Weapon {
private:
    std::string name;
    int damage;

public:
    Weapon(const std::string& n, int d) : name(n), damage(d) {}

    // Перегрузка оператора + для сложения урона
    Weapon operator+(const Weapon& other) const {
        std::string newName = name + " + " + other.name;
        int newDamage = damage + other.damage;
        return Weapon(newName, newDamage);
    }

    // Перегрузка оператора > для сравнения урона
    bool operator>(const Weapon& other) const {
        return damage > other.damage;
    }

    // Перегрузка оператора << для вывода информации
    friend std::ostream& operator<<(std::ostream& os, const Weapon& weapon) {
        os << "Weapon: " << weapon.name << ", Damage: " << weapon.damage;
        return os;
    }
};

int main() {
    // Демонстрация работы операторов для класса Character (из примера)
    Character hero1("Hero", 100, 20, 10);
    Character hero2("Hero", 100, 20, 10);
    Character hero3("Warrior", 150, 25, 15);

    if (hero1 == hero2) {
        std::cout << "Hero1 and Hero2 are the same!\n";
    }
    if (!(hero1 == hero3)) {
        std::cout << "Hero1 and Hero3 are different!\n";
    }

    std::cout << hero1 << std::endl;

    // Демонстрация работы операторов для класса Weapon
    Weapon sword("Sword", 50);
    Weapon bow("Bow", 30);
    Weapon axe("Axe", 60);

    // Использование оператора +
    Weapon combined = sword + bow;
    std::cout << combined << std::endl; // Вывод: Weapon: Sword + Bow, Damage: 80

    // Использование оператора >
    if (axe > sword) {
        std::cout << axe.name << " is stronger than " << sword.name << std::endl;
    } else {
        std::cout << axe.name << " is not stronger than " << sword.name << std::endl;
    }

    if (bow > combined) {
        std::cout << bow.name << " is stronger than combined weapon" << std::endl;
    } else {
        std::cout << "Combined weapon is stronger than " << bow.name << std::endl;
    }

    return 0;
}