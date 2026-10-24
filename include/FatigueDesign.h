#pragma once
#include "MaterialDatabase.h"

class FatigueDesign {
    public:
        static double calculateRequiredDiameter(
            double M_min, double M_max, 
            double T_min, double T_max, 
            const Material& mat, double factorOfSafety,
            double& out_dfatigue, double& out_dyield
        );
};