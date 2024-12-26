#include <fstream>
#include <iostream>
#include <string>
#include "CSVParser.h"

int main() {
    std::ofstream testFile("test.csv");
    //testFile << "ID,Name,Description\n";
    testFile << "1,john,\"developer, likes C++\"\n";
    testFile << "2,4,\"data scientist\"\n";
    testFile << "3,bob,\"teacher, loves \"\"teaching,\"\"\"\n";
    testFile.close();

    std::ifstream file("test.csv");

    try {
        CSVParser<int, std::string, std::string> parser(file, 0, '\n', ',', '"');
        for (const auto &row : parser) {
            auto [id, name, description] = row;
            std::cout << "id: " << id << ", name: " << name << ", decription: " << description << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
