#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>
#include <exception>

// Исключения
class InvalidDataException : public std::exception {
    std::string msg;
public:
    InvalidDataException(const std::string& m) : msg(m) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

// 1. Базовый класс User
class User {
private:
    std::string name;
    int id;
    int accessLevel;

public:
    User(const std::string& n, int i, int level) {
        setName(n);
        setId(i);
        setAccessLevel(level);
    }
    virtual ~User() = default;

    // Геттеры и сеттеры с проверками
    std::string getName() const { return name; }
    void setName(const std::string& n) {
        if (n.empty()) throw InvalidDataException("Name cannot be empty");
        name = n;
    }

    int getId() const { return id; }
    void setId(int i) {
        if (i < 0) throw InvalidDataException("ID must be non-negative");
        id = i;
    }

    int getAccessLevel() const { return accessLevel; }
    void setAccessLevel(int level) {
        if (level < 0) throw InvalidDataException("Access level must be non-negative");
        accessLevel = level;
    }

    virtual void displayInfo() const {
        std::cout << "User: " << name << ", ID: " << id << ", Access Level: " << accessLevel << std::endl;
    }
    virtual std::string serialize() const {
        return name + "," + std::to_string(id) + "," + std::to_string(accessLevel);
    }
    // Для поиска и сортировки можно добавить дополнительные сравнения
};

// 2. Производные классы
class Student : public User {
private:
    std::string group;

public:
    Student(const std::string& n, int i, int level, const std::string& g) 
        : User(n, i, level), group(g) {}

    std::string getGroup() const { return group; }
    void setGroup(const std::string& g) { group = g; }

    void displayInfo() const override {
        std::cout << "Student: " << getName() << ", ID: " << getId()
                  << ", Access Level: " << getAccessLevel() << ", Group: " << group << std::endl;
    }
    std::string serialize() const override {
        return "Student," + User::serialize() + "," + group;
    }
};

class Teacher : public User {
private:
    std::string department;

public:
    Teacher(const std::string& n, int i, int level, const std::string& dep)
        : User(n, i, level), department(dep) {}

    std::string getDepartment() const { return department; }
    void setDepartment(const std::string& dep) { department = dep; }

    void displayInfo() const override {
        std::cout << "Teacher: " << getName() << ", ID: " << getId()
                  << ", Access Level: " << getAccessLevel() << ", Department: " << department << std::endl;
    }
    std::string serialize() const override {
        return "Teacher," + User::serialize() + "," + department;
    }
};

class Administrator : public User {
private:
    std::string role;

public:
    Administrator(const std::string& n, int i, int level, const std::string& r)
        : User(n, i, level), role(r) {}

    std::string getRole() const { return role; }
    void setRole(const std::string& r) { role = r; }

    void displayInfo() const override {
        std::cout << "Administrator: " << getName() << ", ID: " << getId()
                  << ", Access Level: " << getAccessLevel() << ", Role: " << role << std::endl;
    }
    std::string serialize() const override {
        return "Administrator," + User::serialize() + "," + role;
    }
};

// 4. Ресурс университета
class Resource {
private:
    std::string name;
    int requiredAccessLevel;

public:
    Resource(const std::string& n, int level) : name(n), requiredAccessLevel(level) {
        if (level < 0) throw InvalidDataException("Resource access level must be non-negative");
    }

    std::string getName() const { return name; }

int getRequiredAccessLevel() const { return requiredAccessLevel; }

    bool checkAccess(const User& user) const {
        return user.getAccessLevel() >= requiredAccessLevel;
    }
    std::string serialize() const {
        return name + "," + std::to_string(requiredAccessLevel);
    }
};

// 6. Шаблонный класс AccessControlSystem
template<typename UserType, typename ResourceType>
class AccessControlSystem {
private:
    std::vector<std::shared_ptr<UserType>> users;
    std::vector<std::shared_ptr<ResourceType>> resources;

public:
    void addUser(std::shared_ptr<UserType> user) {
        users.push_back(user);
    }

    void addResource(std::shared_ptr<ResourceType> resource) {
        resources.push_back(resource);
    }

    void checkAccessAll() {
        for (auto& user : users) {
            std::cout << "Access info for " << user->getName() << ":" << std::endl;
            for (auto& res : resources) {
                bool canAccess = res->checkAccess(*user);
                std::cout << "  Resource " << res->getName() << ": " << (canAccess ? "Access granted" : "Access denied") << std::endl;
            }
        }
    }

    // Поиск пользователей
    std::shared_ptr<UserType> findUserByName(const std::string& name) {
        for (auto& u : users) {
            if (u->getName() == name) return u;
        }
        return nullptr;
    }

    std::shared_ptr<UserType> findUserById(int id) {
        for (auto& u : users) {
            if (u->getId() == id) return u;
        }
        return nullptr;
    }

    // Сортировка по уровню доступа
    void sortUsersByAccessLevel() {
        std::sort(users.begin(), users.end(),
                  [](const std::shared_ptr<UserType>& a, const std::shared_ptr<UserType>& b) {
                      return a->getAccessLevel() < b->getAccessLevel();
                  });
    }

    // Сохранение и загрузка из файла
    void saveUsersToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file) throw std::runtime_error("Cannot open file for writing");
        for (auto& u : users) {
            file << u->serialize() << std::endl;
        }
        file.close();
    }

    void loadUsersFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) throw std::runtime_error("Cannot open file for reading");
        users.clear();
        std::string line;
        while (std::getline(file, line)) {
            try {
                auto user = parseUser(line);
                if (user) users.push_back(user);
            } catch (const std::exception& e) {
                std::cerr << "Failed to parse user: " << e.what() << std::endl;
            }
        }
        file.close();
    }

    // Аналогично можно сделать для ресурсов...

private:
    // Распарсить строку и создать новый user
    std::shared_ptr<UserType> parseUser(const std::string& line) {
        // Формат: Type,name,id,accessLevel,[group/department/role]
        std::istringstream iss(line);
        std::string type, name, groupDepRole;
        int id, accessLevel;

        size_t pos1 = line.find(',');
        if (pos1 == std::string::npos) throw InvalidDataException("Invalid data format");

        type = line.substr(0, pos1);
        std::string rest = line.substr(pos1 + 1);
        // Используем std::getline для парсинга
        std::istringstream ss(rest);
        std::string idStr, accessLevelStr;

        std::getline(ss, name, ',');
        std::getline(ss, idStr, ',');
        std::getline(ss, accessLevelStr, ',');
        if (!std::getline(ss, groupDepRole, ',')) groupDepRole = "";

        id = std::stoi(idStr);
        accessLevel = std::stoi(accessLevelStr);

        if (type == "Student") {
            return std::make_shared<Student>(name, id, accessLevel, groupDepRole);
        } else if (type == "Teacher") {
            return std::make_shared<Teacher>(name, id, accessLevel, groupDepRole);
        } else if (type == "Administrator") {
            return std::make_shared<Administrator>(name, id, accessLevel, groupDepRole);
} else {
            throw InvalidDataException("Unknown user type");
        }
    }
};