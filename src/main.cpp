#include <iostream>
#include <limits>
#include <cmath>
#include <algorithm>
#include "../include/MaterialDatabase.h"
#include "../include/StandardShaft.h"
#include "../include/FatigueDesign.h"
#include "../include/RigidityDesign.h"
#include "../include/DesignReport.h"

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
double getValidatedInput(const std::string& prompt, double minValue) {
    double value;
    std::cout << prompt;
    while (!(std::cin >> value) || value <= minValue) {
        std::cout << "Invalid input. Must be > " << minValue << ": ";
        clearInputBuffer();
    }
    return value;
}
int main() {
    MaterialDatabase matDB;
    StandardShaft stdShaft;
    std::cout << "Loading databases...\n";
    if (!matDB.loadDatabase("data/materials.csv") || !stdShaft.loadDiameters("data/standard_diameters.csv")) {
        std::cerr << "Failed to load required databases. Check 'data' folder.\n";
        return 1;
    }
    bool running = true;
    while (running) {
        std::cout << "\n=== Power Transmission Shaft Designer ===\n";
        double L = getValidatedInput("Enter Shaft Length (mm): ", 0.0);
        double M_min, M_max, T_min, T_max;
        std::cout << "Enter Min Bending Moment (N-m): "; std::cin >> M_min;
        std::cout << "Enter Max Bending Moment (N-m): "; std::cin >> M_max;
        std::cout << "Enter Min Torque (N-m): "; std::cin >> T_min;
        std::cout << "Enter Max Torque (N-m): "; std::cin >> T_max;
        double d_max = getValidatedInput("Enter Max Allowable Diameter (mm): ", 0.0);
        double theta_max = getValidatedInput("Enter Max Allowable Twist (degrees): ", 0.0);
        double FoS = getValidatedInput("Enter Required Factor of Safety: ", 0.99);
        matDB.displayMaterials();
        std::cout << "Select Material ID from above: ";
        int mat_id;
        Material selectedMat;
        while (!(std::cin >> mat_id) || !matDB.getMaterialById(mat_id, selectedMat)) {
            std::cout << "Invalid ID. Try again: ";
            clearInputBuffer();
        }
        double d_fatigue = 0.0, d_yield = 0.0;
        double d_strength = FatigueDesign::calculateRequiredDiameter(M_min, M_max, T_min, T_max, selectedMat, FoS, d_fatigue, d_yield);
        double abs_T_max = std::max(std::abs(T_max), std::abs(T_min));
        double d_rigidity = RigidityDesign::calculateRequiredDiameter(abs_T_max, L, selectedMat.G, theta_max);
        double d_req = std::max(d_strength, d_rigidity);
        double d_std = stdShaft.selectStandardDiameter(d_req, d_max);
        
        DesignReport::generateReport(L, M_min, M_max, T_min, T_max, d_max, theta_max, FoS, selectedMat, d_fatigue, d_yield, d_rigidity, d_req, d_std);

        std::cout << "Run another design? (y/n): ";
        char choice;
        std::cin >> choice;
        if (choice == 'n' || choice == 'N') {
            running = false;
        }
    }
    return 0;
}