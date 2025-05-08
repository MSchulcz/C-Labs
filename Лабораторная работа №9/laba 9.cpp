#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <typeinfo>
#include <ctime>

// Шаблонный класс Logger для записи логов
template<typename T>
class Logger {
private:
    std::ofstream logFile;
    
public:
    Logger(const std::string& filename) {
        logFile.open(filename, std::ios::app);
        if (!logFile) {
            throw std::runtime_error("Failed to open log file");
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
        logFile << dt << ": " << message << std::endl;
    }
};

// Класс исключения для игровых ошибок
class GameException : public std::runtime_error {
public:
    GameException(const std::string& msg) : std::runtime_error(msg) {}
};

// Базовый класс для предметов инвентаря
class Item {
protected:
    std::string name;
    std::string description;
    
public:
    Item(const std::string& n, const std::string& desc) 
        : name(n), description(desc) {}
        
    virtual ~Item() {}
    
    virtual void use() = 0;
    virtual std::string serialize() const = 0;
    virtual void deserialize(const std::string& data) = 0;
    
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
};

// Класс оружия
class Weapon : public Item {
private:
    int attackBonus;
    
public:
    Weapon(const std::string& n, const std::string& desc, int atk)
        : Item(n, desc), attackBonus(atk) {}
        
    void use() override {
        std::cout << "Equipped " << name << " (+" << attackBonus << " attack)" << std::endl;
    }
    
    int getAttackBonus() const { return attackBonus; }
    
    std::string serialize() const override {
        return "Weapon," + name + "," + description + "," + std::to_string(attackBonus);
    }
    
    void deserialize(const std::string& data) override {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1+1);
        size_t pos3 = data.find(',', pos2+1);
        
        if (pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos) {
            throw GameException("Invalid weapon data format");
        }
        
        name = data.substr(pos1+1, pos2-pos1-1);
        description = data.substr(pos2+1, pos3-pos2-1);
        attackBonus = std::stoi(data.substr(pos3+1));
    }
};

// Класс зелья
class Potion : public Item {
private:
    int healAmount;
    
public:
    Potion(const std::string& n, const std::string& desc, int heal)
        : Item(n, desc), healAmount(heal) {}
        
    void use() override {
        std::cout << "Drank " << name << " (heals " << healAmount << " HP)" << std::endl;
    }
    
    int getHealAmount() const { return healAmount; }
    
    std::string serialize() const override {
        return "Potion," + name + "," + description + "," + std::to_string(healAmount);
    }
    
    void deserialize(const std::string& data) override {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1+1);
        size_t pos3 = data.find(',', pos2+1);
        
        if (pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos) {
            throw GameException("Invalid potion data format");
        }
        
        name = data.substr(pos1+1, pos2-pos1-1);
        description = data.substr(pos2+1, pos3-pos2-1);
        healAmount = std::stoi(data.substr(pos3+1));
    }
};

// Класс инвентаря
class Inventory {
private:
    std::vector<std::unique_ptr<Item>> items;
    
public:
    void addItem(std::unique_ptr<Item> item) {
        items.push_back(std::move(item));
    }
    
    void removeItem(const std::string& itemName) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if ((*it)->getName() == itemName) {
                items.erase(it);
                return;
            }
        }
        throw GameException("Item not found in inventory");
    }
    
    Item* getItem(const std::string& itemName) {
        for (auto& item : items) {
            if (item->getName() == itemName) {
                return item.get();
            }
        }
        return nullptr;
    }
    
    void display() const {
        if (items.empty()) {
            std::cout << "Inventory is empty" << std::endl;
            return;
        }
        
        std::cout << "Inventory:" << std::endl;
        for (const auto& item : items) {
            std::cout << "- " << item->getName() << ": " << item->getDescription() << std::endl;
        }
    }
    
    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file) {
            throw GameException("Failed to open inventory file for writing");
        }
        
        for (const auto& item : items) {
            file << item->serialize() << "\n";
        }
    }
    
    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw GameException("Failed to open inventory file for reading");
        }
        
        items.clear();
        std::string line;
        while (std::getline(file, line)) {
            size_t typePos = line.find(',');
            if (typePos == std::string::npos) continue;
            
            std::string type = line.substr(0, typePos);
            std::unique_ptr<Item> item;
            
            if (type == "Weapon") {
                item = std::make_unique<Weapon>("", "", 0);
            } else if (type == "Potion") {
                item = std::make_unique<Potion>("", "", 0);
            } else {
                continue;
            }
            
            item->deserialize(line);
            items.push_back(std::move(item));
        }
    }
};

// Базовый класс монстра
class Monster {
protected:
    std::string name;
    int health;
    int attack;
    int defense;
    
public:
    Monster(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {}
        
    virtual ~Monster() {}
    
    virtual void attackTarget(Character& target) = 0;
    
    bool isAlive() const { return health > 0; }
    
    void takeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0;
    }
    
    virtual std::string serialize() const {
        return name + "," + std::to_string(health) + "," + 
               std::to_string(attack) + "," + std::to_string(defense);
    }
    
    virtual void deserialize(const std::string& data) {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1+1);
        size_t pos3 = data.find(',', pos2+1);
        
        if (pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos) {
            throw GameException("Invalid monster data format");
        }
        
        name = data.substr(0, pos1);
        health = std::stoi(data.substr(pos1+1, pos2-pos1-1));
        attack = std::stoi(data.substr(pos2+1, pos3-pos2-1));
        defense = std::stoi(data.substr(pos3+1));
    }
    
    virtual void displayInfo() const {
        std::cout << name << " - HP: " << health 
                  << ", ATK: " << attack << ", DEF: " << defense;
    }
    
    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
};

// Классы конкретных монстров
class Goblin : public Monster {
public:
    Goblin() : Monster("Goblin", 30, 8, 3) {}
    
    void attackTarget(Character& target) override {
        std::cout << "Goblin attacks with a crude club!" << std::endl;
        int damage = attack - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << "Deals " << damage << " damage!" << std::endl;
        } else {
            std::cout << "Attack has no effect!" << std::endl;
        }
    }
    
    std::string serialize() const override {
        return "Goblin," + Monster::serialize();
    }
};

class Dragon : public Monster {
public:
    Dragon() : Monster("Dragon", 100, 20, 10) {}
    
    void attackTarget(Character& target) override {
        std::cout << "Dragon breathes fire!" << std::endl;
        int damage = attack - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << "Deals " << damage << " damage!" << std::endl;
        } else {
            std::cout << "Attack has no effect!" << std::endl;
        }
    }
    
    std::string serialize() const override {
        return "Dragon," + Monster::serialize();
    }
};

class Skeleton : public Monster {
public:
    Skeleton() : Monster("Skeleton", 40, 10, 5) {}
    
    void attackTarget(Character& target) override {
        std::cout << "Skeleton attacks with a rusty sword!" << std::endl;
        int damage = attack - target.getDefense();
        if (damage > 0) {
            target.takeDamage(damage);
            std::cout << "Deals " << damage << " damage!" << std::endl;
        } else {
            std::cout << "Attack has no effect!" << std::endl;
        }
    }
    
    std::string serialize() const override {
        return "Skeleton," + Monster::serialize();
    }
};

// Класс персонажа
class Character {
private:
    std::string name;
    int health;
    int maxHealth;
    int attack;
    int defense;
    int level;
    int experience;
    Inventory inventory;
    
public:
    Character(const std::string& n, int h, int a, int d)
        : name(n), health(h), maxHealth(h), attack(a), defense(d), level(1), experience(0) {}
        
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
        if (health < 0) {
            health = 0;
            throw GameException(name + " has been defeated!");
        }
    }
    
    void heal(int amount) {
        health += amount;
        if (health > maxHealth) health = maxHealth;
        std::cout << name << " heals for " << amount << " HP!" << std::endl;
    }
    
    void gainExperience(int exp) {
        experience += exp;
        if (experience >= 100) {
            level++;
            experience -= 100;
            maxHealth += 10;
            health = maxHealth;
            attack += 2;
            defense += 1;
            std::cout << name << " leveled up to level " << level << "!" << std::endl;
        }
    }
    
    void useItem(const std::string& itemName) {
        Item* item = inventory.getItem(itemName);
        if (!item) {
            throw GameException("Item not found in inventory");
        }
        
        if (auto potion = dynamic_cast<Potion*>(item)) {
            heal(potion->getHealAmount());
            inventory.removeItem(itemName);
        } else if (auto weapon = dynamic_cast<Weapon*>(item)) {
            attack += weapon->getAttackBonus();
            std::cout << "Equipped " << weapon->getName() << "! Attack increased by " 
                      << weapon->getAttackBonus() << std::endl;
        }
    }
    
    void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health << "/" << maxHealth
                  << ", ATK: " << attack << ", DEF: " << defense
                  << ", LVL: " << level << ", EXP: " << experience << "/100" << std::endl;
    }
    
    void saveGame(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file) {
            throw GameException("Failed to open save file");
        }
        
        file << name << "\n";
        file << health << "\n";
        file << maxHealth << "\n";
        file << attack << "\n";
        file << defense << "\n";
        file << level << "\n";
        file << experience << "\n";
        
        inventory.saveToFile(filename + "_inventory");
    }
    
    void loadGame(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw GameException("Failed to open save file");
        }
        
        std::getline(file, name);
        
        std::string line;
        std::getline(file, line); health = std::stoi(line);
        std::getline(file, line); maxHealth = std::stoi(line);
        std::getline(file, line); attack = std::stoi(line);
        std::getline(file, line); defense = std::stoi(line);
        std::getline(file, line); level = std::stoi(line);
        std::getline(file, line); experience = std::stoi(line);
        
        inventory.loadFromFile(filename + "_inventory");
    }
    
    void addToInventory(std::unique_ptr<Item> item) {
        inventory.addItem(std::move(item));
    }
    
    void showInventory() const {
        inventory.display();
    }
    
    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getLevel() const { return level; }
    int getExperience() const { return experience; }
};

// Класс игры
class Game {
private:
    Character player;
    Logger<std::string> logger;
    
public:
    Game(const std::string& playerName) 
        : player(playerName, 100, 10, 5), logger("game_log.txt") {
        logger.log("Game started with player: " + playerName);
    }
    
    void startBattle(Monster& monster) {
        logger.log("Battle started between " + player.getName() + " and " + monster.getName());
        
        std::cout << "\n=== BATTLE START ===\n";
        player.displayInfo();
        monster.displayInfo();
        std::cout << "===================\n";
        
        try {
            while (player.getHealth() > 0 && monster.isAlive()) {
                // Ход игрока
                std::cout << "\nYour turn:\n";
                std::cout << "1. Attack\n";
                std::cout << "2. Use item\n";
                std::cout << "Choose action: ";
                
                int choice;
                std::cin >> choice;
                
                if (choice == 1) {
                    player.attackEnemy(monster);
                    logger.log(player.getName() + " attacks " + monster.getName());
                } else if (choice == 2) {
                    player.showInventory();
                    std::cout << "Enter item name to use: ";
                    std::string itemName;
                    std::cin.ignore();
                    std::getline(std::cin, itemName);
                    
                    try {
                        player.useItem(itemName);
                        logger.log(player.getName() + " used item: " + itemName);
                    } catch (const GameException& e) {
                        std::cerr << "Error: " << e.what() << std::endl;
                        continue;
                    }
                } else {
                    std::cout << "Invalid choice!" << std::endl;
                    continue;
                }
                
                // Проверка, побежден ли монстр
                if (!monster.isAlive()) {
                    int expReward = monster.getAttack() * 5;
                    player.gainExperience(expReward);
                    std::cout << monster.getName() << " defeated! Gained " << expReward << " EXP.\n";
                    logger.log(player.getName() + " defeated " + monster.getName() + 
                               " and gained " + std::to_string(expReward) + " EXP");
                    return;
                }
                
                // Ход монстра
                std::cout << "\n" << monster.getName() << "'s turn:\n";
                monster.attackTarget(player);
                logger.log(monster.getName() + " attacks " + player.getName());
                
                // Вывод текущего состояния
                std::cout << "\nCurrent status:\n";
                player.displayInfo();
                monster.displayInfo();
                std::cout << "===================\n";
            }
        } catch (const GameException& e) {
            logger.log("Battle ended: " + std::string(e.what()));
            throw;
        }
    }
    
    void saveGame() {
        try {
            player.saveGame("savegame.txt");
            logger.log("Game saved successfully");
            std::cout << "Game saved!\n";
        } catch (const GameException& e) {
            logger.log("Save failed: " + std::string(e.what()));
            std::cerr << "Error saving game: " << e.what() << std::endl;
        }
    }
    
    void loadGame() {
        try {
            player.loadGame("savegame.txt");
            logger.log("Game loaded successfully");
            std::cout << "Game loaded!\n";
        } catch (const GameException& e) {
            logger.log("Load failed: " + std::string(e.what()));
            std::cerr << "Error loading game: " << e.what() << std::endl;
        }
    }
    
    void addItemToInventory(std::unique_ptr<Item> item) {
        player.addToInventory(std::move(item));
    }
    
    void showPlayerInfo() const {
        player.displayInfo();
    }
    
    void showInventory() const {
        player.showInventory();
    }
};

int main() {
    try {
        std::cout << "Enter your character name: ";
        std::string name;
        std::getline(std::cin, name);
        
        Game game(name);
        
        // Добавляем начальные предметы
        game.addItemToInventory(std::make_unique<Weapon>("Steel Sword", "A sharp steel sword", 5));
        game.addItemToInventory(std::make_unique<Potion>("Health Potion", "Restores 30 HP", 30));
        
        // Главный игровой цикл
        while (true) {
            std::cout << "\n=== MAIN MENU ===\n";
            std::cout << "1. Fight Goblin\n";
            std::cout << "2. Fight Dragon\n";
            std::cout << "3. Fight Skeleton\n";
            std::cout << "4. Show player info\n";
            std::cout << "5. Show inventory\n";
            std::cout << "6. Save game\n";
            std::cout << "7. Load game\n";
            std::cout << "8. Exit\n";
            std::cout << "Choose option: ";
            
            int choice;
            std::cin >> choice;
            
            try {
                switch (choice) {
                    case 1: {
                        Goblin goblin;
                        game.startBattle(goblin);
                        break;
                    }
                    case 2: {
                        Dragon dragon;
                        game.startBattle(dragon);
                        break;
                    }
                    case 3: {
                        Skeleton skeleton;
                        game.startBattle(skeleton);
                        break;
                    }
                    case 4:
                        game.showPlayerInfo();
                        break;
                    case 5:
                        game.showInventory();
                        break;
                    case 6:
                        game.saveGame();
                        break;
                    case 7:
                        game.loadGame();
                        break;
                    case 8:
                        return 0;
                    default:
                        std::cout << "Invalid choice!\n";
                }
            } catch (const GameException& e) {
                std::cerr << "Game over: " << e.what() << std::endl;
                return 1;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}