#pragma once
#include <string>
#include <vector>

struct Material {
    int id;
    std::string name;
    double Sut; // Ultimate tensile strength (MPa)
    double Sy;  // Yield strength (MPa)
    double Se;  // Endurance limit (MPa)
    double G;   // Shear modulus (GPa)
};

class MaterialDatabase {
private:
    std::vector<Material> materials;

public:
    // Loads materials from the specified CSV file path
    bool loadDatabase(const std::string& filepath);
    
    // Displays all loaded materials to the terminal
    void displayMaterials() const;
    
    // Retrieves a material by its ID. Returns true if found.
    bool getMaterialById(int id, Material& outMaterial) const;
};