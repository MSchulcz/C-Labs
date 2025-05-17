#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <ctime>
#include <map>


class GameException : public std::runtime_error {
public:
    explicit GameException(const std::string& message) : std::runtime_error(message) {}
};


template<typename T>
class Logger {
private:
    std::ofstream logFile;
public:
    Logger(const std::string& filename) {
        logFile.open(filename, std::ios::app);
        if (!logFile.is_open()) {
            throw GameException("Failed to open log file!");
        }
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void log(const T& message) {
        time_t now = time(0);
        char* dt = ctime(&now);
        dt[strlen(dt)-1] = '\0'; // удаляем символ новой строки
        logFile << "[" << dt << "] " << message << std::endl;
        std::cout << message << std::endl;
    }
};


class Inventory {
private:
    std::map<std::string, int> items;
public:
    void addItem(const std::string& item, int count = 1) {
        items[item] += count;
    }

    void removeItem(const std::string& item, int count = 1) {
        if (items.find(item) == items.end() || items[item] < count) {
            throw GameException("Not enough " + item + " in inventory!");
        }
        items[item] -= count;
        if (items[item] <= 0) {
            items.erase(item);
        }
    }

    void display() const {
        if (items.empty()) {
            std::cout << "Inventory is empty." << std::endl;
            return;
        }
        std::cout << "Inventory:" << std::endl;
        for (const auto& item : items) {
            std::cout << "- " << item.first << ": " << item.second << std::endl;
        }
    }

    bool hasItem(const std::string& item) const {
        return items.find(item) != items.end();
    }
};


class Monster {
protected:
    std::string name;
    int health;
    int attack;
    int defense;
public:
    Monster(const std::string& n, int h, int a, int d) 
        : name(n), health(h), attack(a), defense(d) {}

    virtual ~Monster() = default;

    virtual void attackTarget(Character& target) {
        int damage = attack - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!" << std::endl;
        } else {
            std::cout << name << " attacks " << target.getName() << ", but it has no effect!" << std::endl;
        }
    }

    void takeDamage(int damage) {
        health -= damage;
        if (health <= 0) {
            throw GameException(name + " has been defeated!");
        }
    }

    virtual void displayInfo() const {
        std::cout << "Monster: " << name << ", HP: " << health 
                  << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
};


class Goblin : public Monster {
public:
    Goblin() : Monster("Goblin", 30, 8, 3) {}
};

class Dragon : public Monster {
public:
    Dragon() : Monster("Dragon", 100, 20, 10) {}
};

class Skeleton : public Monster {
public:
    Skeleton() : Monster("Skeleton", 40, 10, 5) {
        // Скелеты имеют бонус к атаке после первого удара
        attack += 5;
    }

    void attackTarget(Character& target) override {
        Monster::attackTarget(target);
        // После первой атаки бонус исчезает
        if (attack > 15) {
            attack -= 5;
            std::cout << name << "'s bonus attack fades!" << std::endl;
        }
    }
};


class Character {
private:
    std::string name;
    int maxHealth;
    int health;
    int attack;
    int defense;
    int level;
    int experience;
    Inventory inventory;

public:
    Character(const std::string& n, int h, int a, int d)
        : name(n), maxHealth(h), health(h), attack(a), defense(d), level(1), experience(0) {}

    void attackEnemy(Monster& enemy) {
        int damage = attack - enemy.getDefense();
        if (damage > 0) {
            enemy.takeDamage(damage);
            std::cout << name << " attacks " << enemy.getName() << " for " << damage << " damage!" << std::endl;
        } else {
            std::cout << name << " attacks " << enemy.getName() << ", but it has no effect!" << std::endl;
        }
    }

    void takeDamage(int damage) {
        health -= damage;
        if (health <= 0) {
            throw GameException(name + " has been defeated!");
        }
    }

    void heal(int amount) {
        health += amount;
        if (health > maxHealth) health = maxHealth;
        std::cout << name << " heals for " << amount << " HP!" << std::endl;
    }

    void usePotion() {
        if (inventory.hasItem("Health Potion")) {
            inventory.removeItem("Health Potion");
            heal(30);
        } else {
            throw GameException("No health potions in inventory!");
        }
    }

    void gainExperience(int exp) {
        experience += exp;
        if (experience >= 100) {
            levelUp();
        }
    }

    void levelUp() {
        level++;
        experience -= 100;
        maxHealth += 20;
        health = maxHealth;
        attack += 5;
        defense += 3;
        std::cout << name << " leveled up to level " << level << "!" << std::endl;
    }

    void addToInventory(const std::string& item, int count = 1) {
        inventory.addItem(item, count);
    }

    void displayInfo() const {
        std::cout << "Character: " << name << ", HP: " << health << "/" << maxHealth
                  << ", Attack: " << attack << ", Defense: " << defense
                  << ", Level: " << level << ", Experience: " << experience << "/100" << std::endl;
    }

    void displayInventory() const {
        inventory.display();
    }

    // Методы для сохранения и загрузки
    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file) {
            throw GameException("Failed to save game!");
        }
        file << name << "\n" << maxHealth << "\n" << health << "\n" 
             << attack << "\n" << defense << "\n" << level << "\n" << experience << "\n";
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw GameException("Failed to load game!");
        }
        file >> name >> maxHealth >> health >> attack >> defense >> level >> experience;
    }

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getLevel() const { return level; }
    int getExperience() const { return experience; }
};


class Game {
private:
    std::unique_ptr<Character> player;
    Logger<std::string> logger;
    bool gameRunning;

    void createCharacter() {
        std::string name;
        std::cout << "Enter your character's name: ";
        std::cin >> name;
        player = std::make_unique<Character>(name, 100, 10, 5);
        player->addToInventory("Health Potion", 3);
        logger.log("Character " + name + " created.");
    }

    void battle(std::unique_ptr<Monster> monster) {
        logger.log("A wild " + monster->getName() + " appears!");
        
        while (true) {
            // Ход игрока
            try {
                playerTurn(*monster);
                if (monster->getHealth() <= 0) {
                    int exp = 20 + monster->getAttack() * 2;
                    player->gainExperience(exp);
                    logger.log("You defeated the " + monster->getName() + " and gained " + std::to_string(exp) + " XP!");
                    return;
                }
            } catch (const GameException& e) {
                logger.log(e.what());
                gameRunning = false;
                return;
            }

            // Ход монстра
            try {
                monster->attackTarget(*player);
                if (player->getHealth() <= 0) {
                    logger.log("Game Over! You were defeated by the " + monster->getName() + ".");
                    gameRunning = false;
                    return;
                }
            } catch (const GameException& e) {
                logger.log(e.what());
                return;
            }
        }
    }

    void playerTurn(Monster& monster) {
        int choice;
        std::cout << "\n=== Your Turn ===" << std::endl;
        player->displayInfo();
        monster.displayInfo();
        std::cout << "1. Attack\n2. Use Health Potion\n3. Check Inventory\n4. Flee\nChoose an action: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                player->attackEnemy(monster);
                break;
            case 2:
                try {
                    player->usePotion();
                } catch (const GameException& e) {
                    logger.log(e.what());
                    playerTurn(monster); // Повторный ход
                }
                break;
            case 3:
                player->displayInventory();
                playerTurn(monster); // Повторный ход
                break;
            case 4:
                if (rand() % 2 == 0) {
                    logger.log("You successfully fled from battle!");
                    return;
                } else {
                    logger.log("You failed to flee!");
                }
                break;
            default:
                std::cout << "Invalid choice!" << std::endl;
                playerTurn(monster); // Повторный ход
        }
    }

    std::unique_ptr<Monster> generateMonster() {
        int monsterType = rand() % 3;
        switch (monsterType) {
            case 0: return std::make_unique<Goblin>();
            case 1: return std::make_unique<Dragon>();
            case 2: return std::make_unique<Skeleton>();
            default: return std::make_unique<Goblin>();
        }
    }

public:
    Game() : logger("game_log.txt"), gameRunning(false) {
        srand(time(0));
    }

    void start() {
        gameRunning = true;
        createCharacter();
        logger.log("Game started!");

        while (gameRunning) {
            std::cout << "\n=== Main Menu ===" << std::endl;
            std::cout << "1. Explore\n2. Rest\n3. Check Stats\n4. Check Inventory\n5. Save Game\n6. Load Game\n7. Quit\nChoose an action: ";
            int choice;
            std::cin >> choice;

            try {
                switch (choice) {
                    case 1: {
                        if (rand() % 3 == 0) {
                            battle(generateMonster());
                        } else {
                            logger.log("You explore but find nothing interesting.");
                        }
                        break;
                    }
                    case 2:
                        player->heal(20);
                        logger.log("You rest and recover some health.");
                        break;
                    case 3:
                        player->displayInfo();
                        break;
                    case 4:
                        player->displayInventory();
                        break;
                    case 5:
                        player->saveToFile("save.txt");
                        logger.log("Game saved successfully.");
                        break;
                    case 6:
                        player->loadFromFile("save.txt");
                        logger.log("Game loaded successfully.");
                        break;
                    case 7:
                        gameRunning = false;
                        logger.log("Game ended.");
                        break;
                    default:
                        std::cout << "Invalid choice!" << std::endl;
                }
            } catch (const GameException& e) {
                logger.log(e.what());
            }
        }
    }
};

int main() {
    try {
        Game game;
        game.start();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
