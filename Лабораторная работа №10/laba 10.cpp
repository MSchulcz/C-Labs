#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>
#include <exception>


class User {
private:
    std::string name;
    int id;
    int accessLevel;

public:
    User(const std::string& name, int id, int accessLevel) {
        setName(name);
        setId(id);
        setAccessLevel(accessLevel);
    }

    virtual ~User() = default;


    std::string getName() const { return name; }
    int getId() const { return id; }
    int getAccessLevel() const { return accessLevel; }

    void setName(const std::string& newName) {
        if (newName.empty()) {
            throw std::invalid_argument("Имя не может быть пустым");
        }
        name = newName;
    }

    void setId(int newId) {
        if (newId < 0) {
            throw std::invalid_argument("ID не может быть отрицательным");
        }
        id = newId;
    }

    void setAccessLevel(int newAccessLevel) {
        if (newAccessLevel < 0) {
            throw std::invalid_argument("Уровень доступа не может быть отрицательным");
        }
        accessLevel = newAccessLevel;
    }

    // Virtual method for polymorphism
    virtual void displayInfo() const {
            std::cout << "Пользователь: " << name << ", ID: " << id << ", Уровень доступа: " << accessLevel << std::endl;
    }
};

// Derived Student class
class Student : public User {
private:
    std::string group;

public:
    Student(const std::string& name, int id, int accessLevel, const std::string& group)
        : User(name, id, accessLevel), group(group) {}

    void setGroup(const std::string& newGroup) {
        if (newGroup.empty()) {
            throw std::invalid_argument("Группа не может быть пустой");
        }
        group = newGroup;
    }

    std::string getGroup() const { return group; }

    void displayInfo() const override {
        std::cout << "Студент: " << getName() << ", ID: " << getId()
                  << ", Уровень доступа: " << getAccessLevel() << ", Группа: " << group << std::endl;
    }
};

// Derived Teacher class
class Teacher : public User {
private:
    std::string department;

public:
    Teacher(const std::string& name, int id, int accessLevel, const std::string& department)
        : User(name, id, accessLevel), department(department) {}

    void setDepartment(const std::string& newDepartment) {
        if (newDepartment.empty()) {
            throw std::invalid_argument("Кафедра не может быть пустой");
        }
        department = newDepartment;
    }

    std::string getDepartment() const { return department; }

    void displayInfo() const override {
        std::cout << "Преподаватель: " << getName() << ", ID: " << getId()
                  << ", Уровень доступа: " << getAccessLevel() << ", Кафедра: " << department << std::endl;
    }
};

// Derived Administrator class
class Administrator : public User {
private:
    int adminLevel;

public:
    Administrator(const std::string& name, int id, int accessLevel, int adminLevel)
        : User(name, id, accessLevel), adminLevel(adminLevel) {
        if (adminLevel < 0) {
            throw std::invalid_argument("Уровень администратора не может быть отрицательным");
        }
    }

    void setAdminLevel(int newAdminLevel) {
        if (newAdminLevel < 0) {
            throw std::invalid_argument("Admin level cannot be negative");
        }
        adminLevel = newAdminLevel;
    }

    int getAdminLevel() const { return adminLevel; }

    void displayInfo() const override {
        std::cout << "Администратор: " << getName() << ", ID: " << getId()
                  << ", Уровень доступа: " << getAccessLevel() << ", Уровень администратора: " << adminLevel << std::endl;
    }
};

// Resource class representing university resources
class Resource {
private:
    std::string resourceName;
    int requiredAccessLevel;

public:
    Resource(const std::string& name, int accessLevel)
        : resourceName(name), requiredAccessLevel(accessLevel) {
        if (name.empty()) {
            throw std::invalid_argument("Имя ресурса не может быть пустым");
        }
        if (accessLevel < 0) {
            throw std::invalid_argument("Уровень доступа не может быть отрицательным");
        }
    }

    std::string getName() const { return resourceName; }
    int getRequiredAccessLevel() const { return requiredAccessLevel; }

    bool checkAccess(const User& user) const {
        return user.getAccessLevel() >= requiredAccessLevel;
    }

    void displayInfo() const {
        std::cout << "Ресурс: " << resourceName << ", Требуемый уровень доступа: " << requiredAccessLevel << std::endl;
    }
};

// Template class AccessControlSystem to manage users and resources
template <typename UserType, typename ResourceType>
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

    void displayUsers() const {
        for (const auto& user : users) {
            user->displayInfo();
        }
    }

    void displayResources() const {
        for (const auto& resource : resources) {
            resource->displayInfo();
        }
    }

    bool checkUserAccessToResource(int userId, const std::string& resourceName) const {
        auto userIt = std::find_if(users.begin(), users.end(),
            [userId](const std::shared_ptr<UserType>& user) { return user->getId() == userId; });
        if (userIt == users.end()) {
            throw std::runtime_error("Пользователь не найден");
        }

        auto resourceIt = std::find_if(resources.begin(), resources.end(),
            [&resourceName](const std::shared_ptr<ResourceType>& resource) { return resource->getName() == resourceName; });
        if (resourceIt == resources.end()) {
            throw std::runtime_error("Ресурс не найден");
        }

        return (*resourceIt)->checkAccess(**userIt);
    }

    // Search users by name
    std::vector<std::shared_ptr<UserType>> searchUsersByName(const std::string& name) const {
        std::vector<std::shared_ptr<UserType>> result;
        for (const auto& user : users) {
            if (user->getName() == name) {
                result.push_back(user);
            }
        }
        return result;
    }

    // Search users by ID
    std::shared_ptr<UserType> searchUserById(int id) const {
        auto it = std::find_if(users.begin(), users.end(),
            [id](const std::shared_ptr<UserType>& user) { return user->getId() == id; });
        if (it != users.end()) {
            return *it;
        }
        return nullptr;
    }

    // Sort users by access level ascending
    void sortUsersByAccessLevel() {
        std::sort(users.begin(), users.end(),
            [](const std::shared_ptr<UserType>& a, const std::shared_ptr<UserType>& b) {
                return a->getAccessLevel() < b->getAccessLevel();
            });
    }

    // Save users and resources to files
    void saveToFile(const std::string& usersFile, const std::string& resourcesFile) const {
        std::ofstream uFile(usersFile);
        if (!uFile) {
            throw std::runtime_error("Failed to open users file for writing");
        }
        for (const auto& user : users) {
            // Save user type info for loading
            if (dynamic_cast<Student*>(user.get())) {
                uFile << "Student ";
                uFile << user->getName() << " " << user->getId() << " " << user->getAccessLevel() << " ";
                uFile << static_cast<Student*>(user.get())->getGroup() << std::endl;
            } else if (dynamic_cast<Teacher*>(user.get())) {
                uFile << "Teacher ";
                uFile << user->getName() << " " << user->getId() << " " << user->getAccessLevel() << " ";
                uFile << static_cast<Teacher*>(user.get())->getDepartment() << std::endl;
            } else if (dynamic_cast<Administrator*>(user.get())) {
                uFile << "Administrator ";
                uFile << user->getName() << " " << user->getId() << " " << user->getAccessLevel() << " ";
                uFile << static_cast<Administrator*>(user.get())->getAdminLevel() << std::endl;
            } else {
                uFile << "User ";
                uFile << user->getName() << " " << user->getId() << " " << user->getAccessLevel() << std::endl;
            }
        }
        uFile.close();

        std::ofstream rFile(resourcesFile);
        if (!rFile) {
            throw std::runtime_error("Failed to open resources file for writing");
        }
        for (const auto& resource : resources) {
            rFile << resource->getName() << " " << resource->getRequiredAccessLevel() << std::endl;
        }
        rFile.close();
    }

    // Load users and resources from files
    void loadFromFile(const std::string& usersFile, const std::string& resourcesFile) {
        users.clear();
        resources.clear();

        std::ifstream uFile(usersFile);
        if (!uFile) {
            throw std::runtime_error("Failed to open users file for reading");
        }
        std::string userType;
        while (uFile >> userType) {
            if (userType == "Student") {
                std::string name, group;
                int id, accessLevel;
                uFile >> name >> id >> accessLevel >> group;
                addUser(std::make_shared<Student>(name, id, accessLevel, group));
            } else if (userType == "Teacher") {
                std::string name, department;
                int id, accessLevel;
                uFile >> name >> id >> accessLevel >> department;
                addUser(std::make_shared<Teacher>(name, id, accessLevel, department));
            } else if (userType == "Administrator") {
                std::string name;
                int id, accessLevel, adminLevel;
                uFile >> name >> id >> accessLevel >> adminLevel;
                addUser(std::make_shared<Administrator>(name, id, accessLevel, adminLevel));
            } else if (userType == "User") {
                std::string name;
                int id, accessLevel;
                uFile >> name >> id >> accessLevel;
                addUser(std::make_shared<User>(name, id, accessLevel));
            } else {
                // Unknown user type, skip line
                std::string skipLine;
                std::getline(uFile, skipLine);
            }
        }
        uFile.close();

        std::ifstream rFile(resourcesFile);
        if (!rFile) {
            throw std::runtime_error("Failed to open resources file for reading");
        }
        std::string resourceName;
        int accessLevel;
        while (rFile >> resourceName >> accessLevel) {
            addResource(std::make_shared<Resource>(resourceName, accessLevel));
        }
        rFile.close();
    }
};

int main() {
    try {
        AccessControlSystem<User, Resource> system;

        // Add users
        system.addUser(std::make_shared<Student>("Александров Михаил Максимович", 1, 2, "Т.РИ21"));
        system.addUser(std::make_shared<Student>("Прогуляев Студент Неходилович", 2, 1, "Т.РИ22"));
        system.addUser(std::make_shared<Teacher>("Подколзина Любовь Александровна", 3, 3, "ИСТ"));
        system.addUser(std::make_shared<Teacher>("Месхи Бессарион Чохоевич", 4, 4, "ДГТУ"));
        system.addUser(std::make_shared<Administrator>("superadmin", 5, 5, 1));

        // Add resources
        system.addResource(std::make_shared<Resource>("Аудитория 1-228", 1));
        system.addResource(std::make_shared<Resource>("Архивы ДГТУ", 3));
        system.addResource(std::make_shared<Resource>("Админ-панель", 5));

        bool running = true;
        while (running) {
            std::cout << "\n----------------\n";
            std::cout << "\nВыберите действие:\n";
            std::cout << "1. Показать всех пользователей\n";
            std::cout << "2. Показать ресурсы\n";
            std::cout << "3. Проверить доступ пользователя к ресурсу\n";
            std::cout << "4. Найти пользователя по имени\n";
            std::cout << "5. Сортировать пользователей по уровню доступа\n";
            std::cout << "6. Сохранить данные в файл\n";
            std::cout << "7. Загрузить данные из файла\n";
            std::cout << "8. Выход\n";
            std::cout << "Введите выбор: ";
            std::cout << "\n----------------\n";

            int choice;
            std::cin >> choice;

            switch (choice) {
                case 1: {
                    system.displayUsers();
                    break;
                }
                case 2: {
                    system.displayResources();
                    break;
                }
                case 3: {
                    std::cout << "Введите ID пользователя: ";
                    int userId;
                    std::cin >> userId;
                    std::cout << "Введите имя ресурса: ";
                    std::string resourceName;
                    std::cin.ignore();
                    std::getline(std::cin, resourceName);
                    try {
                        bool hasAccess = system.checkUserAccessToResource(userId, resourceName);
                        std::cout << "Пользователь с ID " << userId << (hasAccess ? " имеет " : " не имеет ") << "доступ к " << resourceName << std::endl;
                    } catch (const std::exception& e) {
                        std::cout << "Ошибка: " << e.what() << std::endl;
                    }
                    break;
                }
                case 4: {
                    std::cout << "Введите имя для поиска: ";
                    std::string searchName;
                    std::cin.ignore();
                    std::getline(std::cin, searchName);
                    auto foundUsers = system.searchUsersByName(searchName);
                    std::cout << "Результаты поиска пользователей с именем \"" << searchName << "\":" << std::endl;
                    for (const auto& user : foundUsers) {
                        user->displayInfo();
                    }
                    break;
                }
                case 5: {
                    system.sortUsersByAccessLevel();
                    std::cout << "Пользователи отсортированы по уровню доступа:" << std::endl;
                    system.displayUsers();
                    break;
                }
                case 6: {
                    std::cout << "Введите имя файла для данных пользователей: ";
                    std::string usersFile;
                    std::cin >> usersFile;
                    std::cout << "Введите имя файла для данных ресурсов: ";
                    std::string resourcesFile;
                    std::cin >> resourcesFile;
                    try {
                        system.saveToFile(usersFile, resourcesFile);
                        std::cout << "Данные сохранены в файл." << std::endl;
                    } catch (const std::exception& e) {
                        std::cout << "Ошибка при сохранении данных: " << e.what() << std::endl;
                    }
                    break;
                }
                case 7: {
                    std::cout << "Введите имя файла для данных пользователей: ";
                    std::string usersFile;
                    std::cin >> usersFile;
                    std::cout << "Введите имя файла для данных ресурсов: ";
                    std::string resourcesFile;
                    std::cin >> resourcesFile;
                    try {
                        system.loadFromFile(usersFile, resourcesFile);
                        std::cout << "Данные загружены из файла:" << std::endl;
                        system.displayUsers();
                        system.displayResources();
                    } catch (const std::exception& e) {
                        std::cout << "Ошибка при загрузке данных: " << e.what() << std::endl;
                    }
                    break;
                }
                case 8: {
                    running = false;
                    std::cout << "Завершение работы..." << std::endl;
                    break;
                }
                default: {
                    std::cout << "Неверный вариант. Попробуйте снова." << std::endl;
                    break;
                }
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
