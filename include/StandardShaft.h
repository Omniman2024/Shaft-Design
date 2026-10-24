#pragma once
#include <vector>
#include <string>

class StandardShaft {
    private:
        std::vector<double> standardDiameters; // encapsulation

    public:
        bool loadDiameters(const std::string& filepath); // abstraction
        double selectStandardDiameter(double d_req, double d_max) const; // read-only abstraction
};