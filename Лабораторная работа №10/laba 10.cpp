#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <string>
#include <map>

// Исключения для системы контроля доступа
class AccessControlException : public std::runtime_error {
public:
    explicit AccessControlException(const std::string& message) 
        : std::runtime_error(message) {}
};

// Базовый класс User
class User {
protected:
    std::string name;
    int id;
    int accessLevel; // 1 - студент, 2 - преподаватель, 3 - администратор

public:
    User(const std::string& n, int i, int al) : name(n), id(i), accessLevel(al) {
        if (name.empty()) {
            throw AccessControlException("User name cannot be empty!");
        }
        if (id <= 0) {
            throw AccessControlException("User ID must be positive!");
        }
        if (al < 1 || al > 3) {
            throw AccessControlException("Access level must be between 1 and 3!");
        }
    }

    virtual ~User() = default;

    // Геттеры
    std::string getName() const { return name; }
    int getId() const { return id; }
    int getAccessLevel() const { return accessLevel; }

    // Виртуальный метод для вывода информации
    virtual void displayInfo() const {
        std::cout << "ID: " << id << ", Name: " << name 
                  << ", Access Level: " << accessLevel;
    }

    // Виртуальный метод для сохранения в файл
    virtual void saveToFile(std::ofstream& file) const {
        file << accessLevel << " " << id << " " << name << " ";
    }

    // Виртуальный метод для загрузки из файла
    virtual void loadFromFile(std::ifstream& file) {
        file >> id >> name;
    }
};

// Производные классы пользователей
class Student : public User {
private:
    std::string group;

public:
    Student(const std::string& n, int i, const std::string& g) 
        : User(n, i, 1), group(g) {
        if (group.empty()) {
            throw AccessControlException("Student group cannot be empty!");
        }
    }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << ", Type: Student, Group: " << group << std::endl;
    }

    void saveToFile(std::ofstream& file) const override {
        User::saveToFile(file);
        file << group << "\n";
    }

    void loadFromFile(std::ifstream& file) override {
        User::loadFromFile(file);
        file >> group;
    }

    std::string getGroup() const { return group; }
};

class Teacher : public User {
private:
    std::string department;

public:
    Teacher(const std::string& n, int i, const std::string& d) 
        : User(n, i, 2), department(d) {
        if (department.empty()) {
            throw AccessControlException("Teacher department cannot be empty!");
        }
    }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << ", Type: Teacher, Department: " << department << std::endl;
    }

    void saveToFile(std::ofstream& file) const override {
        User::saveToFile(file);
        file << department << "\n";
    }

    void loadFromFile(std::ifstream& file) override {
        User::loadFromFile(file);
        file >> department;
    }

    std::string getDepartment() const { return department; }
};

class Administrator : public User {
private:
    std::string position;

public:
    Administrator(const std::string& n, int i, const std::string& p) 
        : User(n, i, 3), position(p) {
        if (position.empty()) {
            throw AccessControlException("Administrator position cannot be empty!");
        }
    }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << ", Type: Administrator, Position: " << position << std::endl;
    }

    void saveToFile(std::ofstream& file) const override {
        User::saveToFile(file);
        file << position << "\n";
    }

    void loadFromFile(std::ifstream& file) override {
        User::loadFromFile(file);
        file >> position;
    }

    std::string getPosition() const { return position; }
};

// Класс Resource (ресурс университета)
class Resource {
private:
    std::string name;
    int requiredAccessLevel; // Необходимый уровень доступа

public:
    Resource(const std::string& n, int ral) : name(n), requiredAccessLevel(ral) {
        if (name.empty()) {
            throw AccessControlException("Resource name cannot be empty!");
        }
        if (ral < 1 || ral > 3) {
            throw AccessControlException("Required access level must be between 1 and 3!");
        }
    }

    bool checkAccess(const User& user) const {
        return user.getAccessLevel() >= requiredAccessLevel;
    }

    void displayInfo() const {
        std::cout << "Resource: " << name 
                  << ", Required Access Level: " << requiredAccessLevel << std::endl;
    }

    void saveToFile(std::ofstream& file) const {
        file << name << " " << requiredAccessLevel << "\n";
    }

    void loadFromFile(std::ifstream& file) {
        file >> name >> requiredAccessLevel;
    }

    std::string getName() const { return name; }
    int getRequiredAccessLevel() const { return requiredAccessLevel; }
};

// Шаблонный класс AccessControlSystem
template<typename T>
class AccessControlSystem {
private:
    std::vector<std::unique_ptr<User>> users;
    std::vector<T> resources;
    std::string usersFilename;
    std::string resourcesFilename;

public:
    AccessControlSystem(const std::string& uf, const std::string& rf) 
        : usersFilename(uf), resourcesFilename(rf) {}

    // Добавление пользователя
    void addUser(std::unique_ptr<User> user) {
        // Проверка на уникальность ID
        for (const auto& u : users) {
            if (u->getId() == user->getId()) {
                throw AccessControlException("User with this ID already exists!");
            }
        }
        users.push_back(std::move(user));
    }

    // Добавление ресурса
    void addResource(const T& resource) {
        // Проверка на уникальность имени ресурса
        for (const auto& r : resources) {
            if (r.getName() == resource.getName()) {
                throw AccessControlException("Resource with this name already exists!");
            }
        }
        resources.push_back(resource);
    }

    // Проверка доступа
    bool checkAccess(int userId, const std::string& resourceName) const {
        User* userPtr = nullptr;
        T resource;
        bool resourceFound = false;

        // Поиск пользователя
        for (const auto& u : users) {
            if (u->getId() == userId) {
                userPtr = u.get();
                break;
            }
        }

        if (!userPtr) {
            throw AccessControlException("User not found!");
        }

        // Поиск ресурса
        for (const auto& r : resources) {
            if (r.getName() == resourceName) {
                resource = r;
                resourceFound = true;
                break;
            }
        }

        if (!resourceFound) {
            throw AccessControlException("Resource not found!");
        }

        return resource.checkAccess(*userPtr);
    }

    // Поиск пользователя по имени
    std::vector<User*> findUsersByName(const std::string& name) const {
        std::vector<User*> result;
        for (const auto& u : users) {
            if (u->getName().find(name) != std::string::npos) {
                result.push_back(u.get());
            }
        }
        return result;
    }

    // Поиск пользователя по ID
    User* findUserById(int id) const {
        for (const auto& u : users) {
            if (u->getId() == id) {
                return u.get();
            }
        }
        return nullptr;
    }

    // Сортировка пользователей по уровню доступа
    void sortUsersByAccessLevel() {
        std::sort(users.begin(), users.end(), 
            [](const std::unique_ptr<User>& a, const std::unique_ptr<User>& b) {
                return a->getAccessLevel() < b->getAccessLevel();
            });
    }

    // Сортировка пользователей по имени
    void sortUsersByName() {
        std::sort(users.begin(), users.end(), 
            [](const std::unique_ptr<User>& a, const std::unique_ptr<User>& b) {
                return a->getName() < b->getName();
            });
    }

    // Сохранение данных в файлы
    void saveData() const {
        // Сохранение пользователей
        std::ofstream ufile(usersFilename);
        if (!ufile) {
            throw AccessControlException("Failed to open users file for writing!");
        }

        for (const auto& u : users) {
            u->saveToFile(ufile);
        }

        // Сохранение ресурсов
        std::ofstream rfile(resourcesFilename);
        if (!rfile) {
            throw AccessControlException("Failed to open resources file for writing!");
        }

        for (const auto& r : resources) {
            r.saveToFile(rfile);
        }
    }

    // Загрузка данных из файлов
    void loadData() {
        users.clear();
        resources.clear();

        // Загрузка пользователей
        std::ifstream ufile(usersFilename);
        if (!ufile) {
            throw AccessControlException("Failed to open users file for reading!");
        }

        int accessLevel, id;
        std::string name, additionalInfo;

        while (ufile >> accessLevel >> id >> name >> additionalInfo) {
            switch (accessLevel) {
                case 1:
                    users.push_back(std::make_unique<Student>(name, id, additionalInfo));
                    break;
                case 2:
                    users.push_back(std::make_unique<Teacher>(name, id, additionalInfo));
                    break;
                case 3:
                    users.push_back(std::make_unique<Administrator>(name, id, additionalInfo));
                    break;
                default:
                    throw AccessControlException("Invalid access level in users file!");
            }
        }

        // Загрузка ресурсов
        std::ifstream rfile(resourcesFilename);
        if (!rfile) {
            throw AccessControlException("Failed to open resources file for reading!");
        }

        std::string resourceName;
        int requiredLevel;

        while (rfile >> resourceName >> requiredLevel) {
            resources.emplace_back(resourceName, requiredLevel);
        }
    }

    // Вывод всех пользователей
    void displayAllUsers() const {
        if (users.empty()) {
            std::cout << "No users in the system." << std::endl;
            return;
        }

        std::cout << "=== Users ===" << std::endl;
        for (const auto& u : users) {
            u->displayInfo();
        }
    }

    // Вывод всех ресурсов
    void displayAllResources() const {
        if (resources.empty()) {
            std::cout << "No resources in the system." << std::endl;
            return;
        }

        std::cout << "=== Resources ===" << std::endl;
        for (const auto& r : resources) {
            r.displayInfo();
        }
    }
};

// Функция для отображения меню
void displayMenu() {
    std::cout << "\n=== University Access Control System ===" << std::endl;
    std::cout << "1. Add User" << std::endl;
    std::cout << "2. Add Resource" << std::endl;
    std::cout << "3. Check Access" << std::endl;
    std::cout << "4. Find User by Name" << std::endl;
    std::cout << "5. Find User by ID" << std::endl;
    std::cout << "6. Sort Users by Access Level" << std::endl;
    std::cout << "7. Sort Users by Name" << std::endl;
    std::cout << "8. Display All Users" << std::endl;
    std::cout << "9. Display All Resources" << std::endl;
    std::cout << "10. Save Data" << std::endl;
    std::cout << "11. Load Data" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Choose an option: ";
}

int main() {
    AccessControlSystem<Resource> system("users.txt", "resources.txt");
    
    try {
        // Попытка загрузить данные при старте
        system.loadData();
        std::cout << "Data loaded successfully." << std::endl;
    } catch (const AccessControlException& e) {
        std::cout << "Warning: " << e.what() << " Starting with empty system." << std::endl;
    }

    int choice;
    do {
        displayMenu();
        std::cin >> choice;

        try {
            switch (choice) {
                case 1: { // Добавление пользователя
                    std::string name, additionalInfo;
                    int id, userType;

                    std::cout << "Enter user type (1 - Student, 2 - Teacher, 3 - Administrator): ";
                    std::cin >> userType;
                    std::cout << "Enter user ID: ";
                    std::cin >> id;
                    std::cout << "Enter user name: ";
                    std::cin.ignore();
                    std::getline(std::cin, name);

                    switch (userType) {
                        case 1:
                            std::cout << "Enter student group: ";
                            std::getline(std::cin, additionalInfo);
                            system.addUser(std::make_unique<Student>(name, id, additionalInfo));
                            break;
                        case 2:
                            std::cout << "Enter teacher department: ";
                            std::getline(std::cin, additionalInfo);
                            system.addUser(std::make_unique<Teacher>(name, id, additionalInfo));
                            break;
                        case 3:
                            std::cout << "Enter administrator position: ";
                            std::getline(std::cin, additionalInfo);
                            system.addUser(std::make_unique<Administrator>(name, id, additionalInfo));
                            break;
                        default:
                            std::cout << "Invalid user type!" << std::endl;
                    }
                    break;
                }
                case 2: { // Добавление ресурса
                    std::string name;
                    int requiredLevel;

                    std::cout << "Enter resource name: ";
                    std::cin.ignore();
                    std::getline(std::cin, name);
                    std::cout << "Enter required access level (1-3): ";
                    std::cin >> requiredLevel;

                    system.addResource(Resource(name, requiredLevel));
                    break;
                }
                case 3: { // Проверка доступа
                    int userId;
                    std::string resourceName;

                    std::cout << "Enter user ID: ";
                    std::cin >> userId;
                    std::cout << "Enter resource name: ";
                    std::cin.ignore();
                    std::getline(std::cin, resourceName);

                    if (system.checkAccess(userId, resourceName)) {
                        std::cout << "Access GRANTED." << std::endl;
                    } else {
                        std::cout << "Access DENIED." << std::endl;
                    }
                    break;
                }
                case 4: { // Поиск пользователя по имени
                    std::string name;
                    std::cout << "Enter name to search: ";
                    std::cin.ignore();
                    std::getline(std::cin, name);

                    auto users = system.findUsersByName(name);
                    if (users.empty()) {
                        std::cout << "No users found with name containing '" << name << "'." << std::endl;
                    } else {
                        std::cout << "Found users:" << std::endl;
                        for (const auto& u : users) {
                            u->displayInfo();
                        }
                    }
                    break;
                }
                case 5: { // Поиск пользователя по ID
                    int id;
                    std::cout << "Enter ID to search: ";
                    std::cin >> id;

                    auto user = system.findUserById(id);
                    if (user) {
                        user->displayInfo();
                    } else {
                        std::cout << "User with ID " << id << " not found." << std::endl;
                    }
                    break;
                }
                case 6: // Сортировка пользователей по уровню доступа
                    system.sortUsersByAccessLevel();
                    std::cout << "Users sorted by access level." << std::endl;
                    break;
                case 7: // Сортировка пользователей по имени
                    system.sortUsersByName();
                    std::cout << "Users sorted by name." << std::endl;
                    break;
                case 8: // Вывод всех пользователей
                    system.displayAllUsers();
                    break;
                case 9: // Вывод всех ресурсов
                    system.displayAllResources();
                    break;
                case 10: // Сохранение данных
                    system.saveData();
                    std::cout << "Data saved successfully." << std::endl;
                    break;
                case 11: // Загрузка данных
                    system.loadData();
                    std::cout << "Data loaded successfully." << std::endl;
                    break;
                case 0: // Выход
                    std::cout << "Exiting..." << std::endl;
                    break;
                default:
                    std::cout << "Invalid option!" << std::endl;
            }
        } catch (const AccessControlException& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown error occurred!" << std::endl;
        }

    } while (choice != 0);

    return 0;
}
