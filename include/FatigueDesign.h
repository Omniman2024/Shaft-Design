#pragma once
#include "MaterialDatabase.h"

class FatigueDesign {
public:
    static bool calculateRequiredDimensions(double M_min, double M_max, double T_min, double T_max, double P_min, double P_max, const Material& mat, double factorOfSafety, int hollowMode, double hollowVal, double& out_do, double& out_di);
};