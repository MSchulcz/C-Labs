#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <memory>


class Entity {
protected:
    std::string name;
    int health;
    int level;

public:
    Entity(const std::string& name, int health, int level)
        : name(name), health(health), level(level) {}

    virtual ~Entity() {}

    virtual void display() const {
        std::cout << "Name: " << name << ", Health: " << health << ", Level: " << level;
    }

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getLevel() const { return level; }

    virtual std::string serialize() const {
        return name + "," + std::to_string(health) + "," + std::to_string(level);
    }

    virtual void deserialize(const std::string& data) {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1 + 1);
        
        if (pos1 == std::string::npos || pos2 == std::string::npos) {
            throw std::runtime_error("Invalid data format");
        }
        
        name = data.substr(0, pos1);
        health = std::stoi(data.substr(pos1 + 1, pos2 - pos1 - 1));
        level = std::stoi(data.substr(pos2 + 1));
    }
};


class Player : public Entity {
private:
    int experience;

public:
    Player(const std::string& name, int health, int level, int exp = 0)
        : Entity(name, health, level), experience(exp) {}

    void display() const override {
        Entity::display();
        std::cout << ", Experience: " << experience << std::endl;
    }

    std::string serialize() const override {
        return Entity::serialize() + "," + std::to_string(experience) + ",Player";
    }

    void deserialize(const std::string& data) override {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1 + 1);
        size_t pos3 = data.find(',', pos2 + 1);
        size_t pos4 = data.find(',', pos3 + 1);
        
        if (pos1 == std::string::npos || pos2 == std::string::npos || 
            pos3 == std::string::npos || pos4 == std::string::npos) {
            throw std::runtime_error("Invalid data format for Player");
        }
        
        Entity::deserialize(data.substr(0, pos3));
        experience = std::stoi(data.substr(pos3 + 1, pos4 - pos3 - 1));
    }
};

class Enemy : public Entity {
private:
    std::string type;

public:
    Enemy(const std::string& name, int health, int level, const std::string& type)
        : Entity(name, health, level), type(type) {}

    void display() const override {
        Entity::display();
        std::cout << ", Type: " << type << std::endl;
    }

    std::string serialize() const override {
        return Entity::serialize() + "," + type + ",Enemy";
    }

    void deserialize(const std::string& data) override {
        size_t pos1 = data.find(',');
        size_t pos2 = data.find(',', pos1 + 1);
        size_t pos3 = data.find(',', pos2 + 1);
        size_t pos4 = data.find(',', pos3 + 1);
        
        if (pos1 == std::string::npos || pos2 == std::string::npos || 
            pos3 == std::string::npos || pos4 == std::string::npos) {
            throw std::runtime_error("Invalid data format for Enemy");
        }
        
        Entity::deserialize(data.substr(0, pos3));
        type = data.substr(pos3 + 1, pos4 - pos3 - 1);
    }
};

template<typename T>
class GameManager {
private:
    std::vector<T> entities;

public:
    void addEntity(T entity) {
        entities.push_back(entity);
    }

    void displayAll() const {
        for (const auto& entity : entities) {
            entity->display();
        }
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file) {
            throw std::runtime_error("Failed to open file for writing.");
        }

        for (const auto& entity : entities) {
            file << entity->serialize() << "\n";
        }
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Failed to open file for reading.");
        }

        entities.clear();

        std::string line;
        while (std::getline(file, line)) {
            size_t lastComma = line.rfind(',');
            if (lastComma == std::string::npos) {
                continue;
            }

            std::string type = line.substr(lastComma + 1);
            Entity* entity = nullptr;

            if (type == "Player") {
                entity = new Player("", 0, 0);
            } else if (type == "Enemy") {
                entity = new Enemy("", 0, 0, "");
            } else {
                continue;
            }

            entity->deserialize(line);
            entities.push_back(entity);
        }
    }

    ~GameManager() {
        for (auto& entity : entities) {
            delete entity;
        }
    }
};

int main() {
    try {
        GameManager<Entity*> manager;
        manager.addEntity(new Player("Hero", 100, 1, 0));
        manager.addEntity(new Player("Mage", 80, 2, 150));
        manager.addEntity(new Enemy("Dragon", 200, 5, "Boss"));

        std::cout << "Original characters:" << std::endl;
        manager.displayAll();

        manager.saveToFile("game_save.txt");
        std::cout << "\nCharacters saved to file.\n" << std::endl;

        GameManager<Entity*> loadedManager;
        loadedManager.loadFromFile("game_save.txt");

        std::cout << "Loaded characters:" << std::endl;
        loadedManager.displayAll();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
