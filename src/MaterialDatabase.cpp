#include "../include/MaterialDatabase.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool MaterialDatabase::loadDatabase(const std::string& filepath) { // INPUT : file path , OUTPUT : bool
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open material database at " << filepath << "\n";
        return false;
    }
    std::string line;
    bool isHeader = true;
    while (std::getline(file, line)) {
        if (isHeader) {
            isHeader = false;
            continue;
        }
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;
        Material mat;
        try {
            std::getline(ss, token, ','); mat.id = std::stoi(token);
            std::getline(ss, token, ','); mat.name = token;
            std::getline(ss, token, ','); mat.Sut = std::stod(token);
            std::getline(ss, token, ','); mat.Sy = std::stod(token);
            std::getline(ss, token, ','); mat.Se = std::stod(token);
            std::getline(ss, token, ','); mat.G = std::stod(token);
            materials.push_back(mat);
        }
        catch (const std::exception& e) {
            std::cerr << "Warning: Skipping malformed line in CSV -> " << line << "\n";
        }
    }
    return true;
}

void MaterialDatabase::displayMaterials() const { // const guarantees no modification of class data will happen
    std::cout << "\n--- Available Materials ---\n";
    std::cout << "ID\tName\t\tSut(MPa)\tSy(MPa)\t\tSe(MPa)\t\tG(GPa)\n";
    std::cout << "----------------------------------------------------------------------\n";
    for (const auto& mat : materials) {
        std::cout << mat.id << "\t" << mat.name << "\t\t" 
                  << mat.Sut << "\t\t" << mat.Sy << "\t\t" 
                  << mat.Se << "\t\t" << mat.G << "\n";
    }
    std::cout << "----------------------------------------------------------------------\n";
}

bool MaterialDatabase::getMaterialById(int id, Material& outMaterial) const {
    for (const auto& mat : materials) {
        if (mat.id == id) {
            outMaterial = mat;
            return true;
        }
    }
    return false; // requested ID doesn't exist in database
}