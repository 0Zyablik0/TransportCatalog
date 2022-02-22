//
// Created by Artem Kryukov on 24.12.2021.
//


#include "Geometry.h"
#include <cmath>

double Geometry::Distance(double latitude_1, double latitude_2, double longitude_1, double longitude_2) {
    double phi_1 = latitude_1 * pi / 180.0;
    double phi_2 = latitude_2 * pi / 180.0;
    double delta_phi = phi_2 - phi_1;
    double delta_lambda = (longitude_2 - longitude_1) * pi / 180.0;
    double d = std::acos(sin(phi_1) * std::sin(phi_2) +
                    std::cos(phi_1) * std::cos(phi_2) *
                    std::cos(std::abs(delta_lambda))) * R_Earth;
    return d;
}