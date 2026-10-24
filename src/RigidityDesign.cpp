#include "../include/RigidityDesign.h"
#include <cmath>

#define PI 3.14159265358979323846

double RigidityDesign::calculateRequiredDiameter(double T_max, double L, double G, double theta_max_deg) {
    double T_max_nmm = T_max * 1000.0;     // N-m to N-mm
    double G_mpa = G * 1000.0;             // GPa to MPa = N/mm^2 
    double theta_rad = theta_max_deg * (PI / 180.0); // degrees to radians
    double d_fourth = (32.0 * T_max_nmm * L) / (PI * G_mpa * theta_rad);
    return std::pow(d_fourth, 0.25);
}