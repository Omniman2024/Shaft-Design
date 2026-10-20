#pragma once
#include <vector>
#include <string>

class StandardShaft {
private:
    std::vector<double> standardDiameters;

public:
    // Loads the standard diameters from a CSV file
    bool loadDiameters(const std::string& filepath);
    
    // Selects the smallest standard diameter >= d_req.
    // Returns -1.0 if no valid diameter exists or if the smallest valid diameter exceeds d_max.
    double selectStandardDiameter(double d_req, double d_max) const;
};