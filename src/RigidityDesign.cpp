#include "../include/RigidityDesign.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double RigidityDesign::calculateRequiredDiameter(
    double T_max, double L, 
    double G, double theta_max_deg) 
{
    // Convert inputs to consistent units (N, mm, rad)
    double T_max_nmm = T_max * 1000.0;     // N-m to N-mm
    double G_mpa = G * 1000.0;             // GPa to MPa (N/mm^2)
    double theta_rad = theta_max_deg * (M_PI / 180.0); // Degrees to Radians

    // Twist equation: theta = (T * L) / (J * G)
    // For a solid circular shaft: J = (pi * d^4) / 32
    // Substituting J: theta = (32 * T * L) / (pi * d^4 * G)
    // Solving for d: d^4 = (32 * T * L) / (pi * G * theta)
    
    double d_fourth = (32.0 * T_max_nmm * L) / (M_PI * G_mpa * theta_rad);
    return std::pow(d_fourth, 0.25);
}