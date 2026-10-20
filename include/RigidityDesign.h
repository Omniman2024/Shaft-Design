#pragma once

class RigidityDesign {
public:
    // Calculates the required diameter based on the angle of twist constraint.
    // T_max in N-m, L in mm, G in GPa, theta_max in degrees. Returned diameter is in mm.
    static double calculateRequiredDiameter(
        double T_max, double L, 
        double G, double theta_max_deg
    );
};