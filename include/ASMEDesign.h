#pragma once
#include "MaterialDatabase.h"

class ASMEDesign {
public:
    static double calculateRequiredDiameter(double M, double T, double P, double Kb, double Kt, bool hasKeyway, const Material& mat);
};
