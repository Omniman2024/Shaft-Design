#pragma once
#include "MaterialDatabase.h"

class DesignReport {
public:
    static void generateReport(int mode, int shaftConf, int hollowMode, double L, double M_min, double M_max, double T_min, double T_max, double P_min, double P_max,double peak_M, double peak_T, double peak_P, double Kb, double Kt, bool hasKeyway,double d_max, double theta_max, double FoS, const Material& mat,double d_strength_do, double d_rigidity_do, double d_req_do, double d_std_do,double d_req_di, double d_std_di);
};