#pragma once
#include "MaterialDatabase.h"

class FatigueDesign {
public:
    // Calculates the required diameter based on the Modified Goodman criterion and Yield check.
    // Loads should be provided in N-m. Returned diameter is in mm.
    static double calculateRequiredDiameter(
        double M_min, double M_max, 
        double T_min, double T_max, 
        const Material& mat, double factorOfSafety,
        double& out_dfatigue, double& out_dyield
    );
};