#pragma once
#include "MaterialDatabase.h"

class ASMEDesign {
public:
    static bool calculateRequiredDimensions(double M, double T, double P, double Kb, double Kt, bool hasKeyway, const Material& mat, int hollowMode, double hollowVal, double& out_do, double& out_di);
};