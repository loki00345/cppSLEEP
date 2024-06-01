#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>

class Contact {
private:
    char* fullName;
    std::string homePhone;
    std::string workPhone;
    std::string mobilePhone;
    std::string additionalInfo;

public:
    Contact(const char* name, const std::string& home, const std::string& work, const std::string& mobile, const std::string& info) {
        fullName = new char[strlen(name) + 1];
        strcpy_s(fullName, strlen(name) + 1, name);
        homePhone = home;
        workPhone = work;
        mobilePhone = mobile;
        additionalInfo = info;
    }

    
    ~Contact() {
        delete[] fullName;
    }

    const char* getFullName() const {
        return fullName;
    }

    void printContact() const {
        std::cout << "Full Name: " << fullName << "\n"
            << "Home Phone: " << homePhone << "\n"
            << "Work Phone: " << workPhone << "\n"
            << "Mobile Phone: " << mobilePhone << "\n"
            << "Additional Info: " << additionalInfo << "\n";
    }

    void saveToFile(std::ofstream& ofs) const {
        size_t len = strlen(fullName);
        ofs.write(reinterpret_cast<const char*>(&len), sizeof(len));
        ofs.write(fullName, len);
        ofs << homePhone << "\n"
            << workPhone << "\n"
            << mobilePhone << "\n"
            << additionalInfo << "\n";
    }

    static Contact* loadFromFile(std::ifstream& ifs) {
        size_t len;
        ifs.read(reinterpret_cast<char*>(&len), sizeof(len));
        char* name = new char[len + 1];
        ifs.read(name, len);
        name[len] = '\0';

        std::string home, work, mobile, info;
        std::getline(ifs, home);
        std::getline(ifs, work);
        std::getline(ifs, mobile);
        std::getline(ifs, info);

        return new Contact(name, home, work, mobile, info);
    }
};

class PhoneBook {
private:
    std::vector<Contact*> contacts;

public:
    void addContact(Contact* contact) {
        contacts.push_back(contact);
    }

    void deleteContact(const char* name) {
        for (auto it = contacts.begin(); it != contacts.end(); ++it) {
            if (strcmp((*it)->getFullName(), name) == 0) {
                delete* it;
                contacts.erase(it);
                std::cout << "Contact deleted.\n";
                return;
            }
        }
        std::cout << "Contact not found.\n";
    }

    void searchContact(const char* name) const {
        for (const auto& contact : contacts) {
            if (strcmp(contact->getFullName(), name) == 0) {
                contact->printContact();
                return;
            }
        }
        std::cout << "Contact not found.\n";
    }

    void showAllContacts() const {
        for (const auto& contact : contacts) {
            contact->printContact();
            std::cout << "--------------------------\n";
        }
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs) {
            std::cerr << "Failed to open file for writing.\n";
            return;
        }
        size_t size = contacts.size();
        ofs.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& contact : contacts) {
            contact->saveToFile(ofs);
        }
        ofs.close();
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs) {
            std::cerr << "Failed to open file for reading.\n";
            return;
        }
        size_t size;
        ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
        for (size_t i = 0; i < size; ++i) {
            Contact* contact = Contact::loadFromFile(ifs);
            contacts.push_back(contact);
        }
        ifs.close();
    }

    ~PhoneBook() {
        for (auto contact : contacts) {
            delete contact;
        }
    }
};

int main() {
    PhoneBook phoneBook;

    phoneBook.addContact(new Contact("John Doe", "123-45-67", "765-43-21", "098-765-4321", "Work colleague"));
    phoneBook.addContact(new Contact("Jane Smith", "234-56-78", "876-54-32", "097-654-3210", "Childhood friend"));

    std::cout << "All contacts:\n";
    phoneBook.showAllContacts();

    std::cout << "Search for contact by full name (John Doe):\n";
    phoneBook.searchContact("John Doe");

    std::cout << "Delete contact (John Doe):\n";
    phoneBook.deleteContact("John Doe");

    std::cout << "All contacts after deletion:\n";
    phoneBook.showAllContacts();

    phoneBook.saveToFile("contacts.dat");

    PhoneBook newPhoneBook;
    newPhoneBook.loadFromFile("contacts.dat");

    std::cout << "All contacts from the new phone book:\n";
    newPhoneBook.showAllContacts();

    return 0;
}
