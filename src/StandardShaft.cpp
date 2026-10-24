#include "../include/StandardShaft.h"
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

bool StandardShaft::loadDiameters(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open standard diameters database at " << filepath << "\n";
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
        try {
            double dia = std::stod(line);
            standardDiameters.push_back(dia);
        } 
        catch (const std::exception& e) {
            std::cerr << "Warning: Skipping malformed line in CSV -> " << line << "\n";
        }
    }
    std::sort(standardDiameters.begin(), standardDiameters.end());
    return true;
}

double StandardShaft::selectStandardDiameter(double d_req, double d_max) const { // defines a read-only method that accepts the theoretical required diameter and the user's maximum constraint
    if (d_req > d_max) {
        return -1.0; 
    }
    for (double d_std : standardDiameters) {
        if (d_std >= d_req) {
            if (d_std <= d_max) {
                return d_std;
            } 
            else {
                return -1.0; 
            }
        }
    }
    return -1.0;
}