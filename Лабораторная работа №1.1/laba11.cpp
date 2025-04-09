#include <iostream>
#include <string>

class Character {
private:
    std::string name;  // Приватное поле: имя персонажа
    int health;        // Приватное поле: уровень здоровья
    int attack;        // Приватное поле: уровень атаки
    int defense;       // Приватное поле: уровень защиты

public:
    // Конструктор для инициализации данных
    Character(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {}

    // Метод для получения уровня здоровья
    int getHealth() const {
        return health;
    }

    // Метод для вывода информации о персонаже
    void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health
                  << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }

    // Метод для атаки другого персонажа
    void attackEnemy(Character& enemy) {
        int damage = attack - enemy.defense;
        if (damage > 0) {
            enemy.health -= damage;
            std::cout << name << " attacks " << enemy.name << " for " << damage << " damage!" << std::endl;
        } else {
            std::cout << name << " attacks " << enemy.name << ", but it has no effect!" << std::endl;
        }
    }

    // Метод для лечения персонажа (не более 100 HP)
    void heal(int amount) {
        health += amount;
        if (health > 100) health = 100;
        std::cout << name << " healed for " << amount << " HP. Current HP: " << health << std::endl;
    }

    // Метод для получения урона (не менее 0 HP)
    void takeDamage(int amount) {
        health -= amount;
        if (health < 0) health = 0;
        std::cout << name << " took " << amount << " damage. Current HP: " << health << std::endl;
    }
};

int main() {
    // Создаем объекты персонажей
    Character hero("Hero", 100, 20, 10);
    Character monster("Goblin", 50, 15, 5);

    // Выводим информацию о персонажах
    std::cout << "Initial status:" << std::endl;
    hero.displayInfo();
    monster.displayInfo();
    std::cout << std::endl;

    // Демонстрация атаки
    std::cout << "Combat:" << std::endl;
    hero.attackEnemy(monster);
    monster.displayInfo();
    std::cout << std::endl;

    // Демонстрация лечения
    std::cout << "Healing:" << std::endl;
    hero.heal(15);
    monster.heal(10);
    std::cout << std::endl;

    // Демонстрация получения урона
    std::cout << "Taking damage:" << std::endl;
    hero.takeDamage(30);
    monster.takeDamage(20);
    std::cout << std::endl;

    // Финальный статус
    std::cout << "Final status:" << std::endl;
    hero.displayInfo();
    monster.displayInfo();

    return 0;
}
