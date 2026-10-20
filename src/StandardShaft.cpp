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
        } catch (const std::exception& e) {
            std::cerr << "Warning: Skipping malformed line in CSV -> " << line << "\n";
        }
    }
    
    // Ensure diameters are in ascending order
    std::sort(standardDiameters.begin(), standardDiameters.end());
    return true;
}

double StandardShaft::selectStandardDiameter(double d_req, double d_max) const {
    if (d_req > d_max) {
        return -1.0; // Theoretical requirement already exceeds the maximum allowable diameter
    }

    for (double d_std : standardDiameters) {
        if (d_std >= d_req) {
            if (d_std <= d_max) {
                return d_std;
            } else {
                return -1.0; // The next available standard size violates the max diameter constraint
            }
        }
    }
    
    return -1.0; // Required diameter exceeds all available standard sizes in the database
}