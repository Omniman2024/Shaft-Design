#pragma once
#include <string>
#include <vector>

struct Material { // members of struct are public by default
    int id;
    std::string name;
    double Sut; 
    double Sy;  
    double Se; 
    double G;  
};

class MaterialDatabase {
    private:
        std::vector<Material> materials; // encapsulation

    public:
        bool loadDatabase(const std::string& filepath); // abstraction
        void displayMaterials() const; // read-only
        bool getMaterialById(int id, Material& outMaterial) const; // read-only abstraction
};