#pragma once
#include "MaterialDatabase.h"

class DesignReport {
    public:
        static void generateReport(double L, double M_min, double M_max, double T_min, double T_max, double P_min, double P_max, double d_max, double theta_max, double FoS, const Material& mat,double d_fatigue, double d_yield, double d_rigidity, double d_req,double d_std);
};