#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <random>
#include <string>

using namespace std;

// Класс персонажа
class Character {
private:
    string name;
    int health;
    int attack;
    int defense;
    mutex mtx;

public:
    Character(const string& name, int health, int attack, int defense)
        : name(name), health(health), attack(attack), defense(defense) {}

    void takeDamage(int damage) {
        lock_guard<mutex> lock(mtx);
        int actualDamage = max(1, damage - defense);
        health -= actualDamage;
        if (health < 0) health = 0;
    }

    bool isAlive() const {
        lock_guard<mutex> lock(mtx);
        return health > 0;
    }

    int getAttack() const {
        lock_guard<mutex> lock(mtx);
        return attack;
    }

    void displayInfo() const {
        lock_guard<mutex> lock(mtx);
        cout << name << " - Health: " << health << ", Attack: " << attack << ", Defense: " << defense << endl;
    }

    string getName() const {
        return name;
    }

    int getHealth() const {
        lock_guard<mutex> lock(mtx);
        return health;
    }
};

// Класс монстра
class Monster {
private:
    string name;
    int health;
    int attack;
    int defense;
    mutex mtx;

public:
    Monster(const string& name, int health, int attack, int defense)
        : name(name), health(health), attack(attack), defense(defense) {}

    void takeDamage(int damage) {
        lock_guard<mutex> lock(mtx);
        int actualDamage = max(1, damage - defense);
        health -= actualDamage;
        if (health < 0) health = 0;
    }

    bool isAlive() const {
        lock_guard<mutex> lock(mtx);
        return health > 0;
    }

    int getAttack() const {
        lock_guard<mutex> lock(mtx);
        return attack;
    }

    void displayInfo() const {
        lock_guard<mutex> lock(mtx);
        cout << name << " - Health: " << health << ", Attack: " << attack << ", Defense: " << defense << endl;
    }

    string getName() const {
        return name;
    }

    int getHealth() const {
        lock_guard<mutex> lock(mtx);
        return health;
    }
};

// Глобальные переменные для хранения монстров
vector<Monster> monsters;
mutex monstersMutex;

// Функция для генерации случайных монстров
void generateMonsters() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> healthDist(30, 100);
    uniform_int_distribution<> attackDist(5, 20);
    uniform_int_distribution<> defenseDist(1, 10);
    vector<string> names = {"Goblin", "Orc", "Troll", "Skeleton", "Zombie", "Dragon"};

    while (true) {
        this_thread::sleep_for(chrono::seconds(3)); // Новый монстр каждые 3 секунды

        uniform_int_distribution<> nameDist(0, names.size() - 1);
        string name = names[nameDist(gen)];
        int health = healthDist(gen);
        int attack = attackDist(gen);
        int defense = defenseDist(gen);

        lock_guard<mutex> lock(monstersMutex);
        monsters.emplace_back(name, health, attack, defense);
        cout << "New monster generated: " << name << " (HP: " << health 
             << ", ATK: " << attack << ", DEF: " << defense << ")\n";
    }
}

// Функция для боя между персонажем и монстром
void battle(Character& hero, Monster& monster) {
    while (hero.isAlive() && monster.isAlive()) {
        // Персонаж атакует монстра
        monster.takeDamage(hero.getAttack());
        cout << hero.getName() << " attacks " << monster.getName() << "!\n";
        
        // Проверяем, жив ли еще монстр
        if (!monster.isAlive()) {
            cout << monster.getName() << " has been defeated!\n";
            break;
        }

        // Монстр атакует персонажа
        hero.takeDamage(monster.getAttack());
        cout << monster.getName() << " attacks " << hero.getName() << "!\n";

        // Выводим текущее состояние
        hero.displayInfo();
        monster.displayInfo();
        cout << "----------------------\n";

        // Пауза между раундами боя
        this_thread::sleep_for(chrono::seconds(1));
    }

    if (hero.isAlive()) {
        cout << hero.getName() << " won the battle!\n";
    } else {
        cout << hero.getName() << " has been defeated by " << monster.getName() << "!\n";
    }
}

int main() {
    // Создаем персонажа
    Character hero("Hero", 100, 15, 5);
    cout << "Hero created:\n";
    hero.displayInfo();
    cout << endl;

    // Запускаем генератор монстров в отдельном потоке
    thread monsterGenerator(generateMonsters);
    monsterGenerator.detach();

    // Основной игровой цикл
    while (hero.isAlive()) {
        this_thread::sleep_for(chrono::seconds(1));

        // Проверяем наличие монстров
        monstersMutex.lock();
        if (!monsters.empty()) {
            // Берем первого монстра из списка
            Monster& currentMonster = monsters.front();
            monstersMutex.unlock();

            cout << "\n=== BATTLE START ===\n";
            cout << hero.getName() << " vs " << currentMonster.getName() << "\n";
            hero.displayInfo();
            currentMonster.displayInfo();
            cout << "----------------------\n";

            // Запускаем бой в отдельном потоке
            thread fight(battle, ref(hero), ref(currentMonster));
            fight.join();

            // Удаляем побежденного монстра
            monstersMutex.lock();
            if (!monsters.empty() && !monsters.front().isAlive()) {
                monsters.erase(monsters.begin());
            }
            monstersMutex.unlock();

            if (!hero.isAlive()) {
                break;
            }
        } else {
            monstersMutex.unlock();
            cout << "No monsters to fight. Waiting...\n";
        }
    }

    cout << "\nGame over!\n";
    return 0;
}