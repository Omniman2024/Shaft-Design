#pragma once

class RigidityDesign {
public:
    static bool calculateRequiredDimensions(double T_max, double L, double G, double theta_max_deg, int hollowMode, double hollowVal, double& out_do, double& out_di);
};