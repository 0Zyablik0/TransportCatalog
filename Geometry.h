//
// Created by Artem Kryukov on 24.12.2021.
//


#pragma once



static const double pi = 3.1415926535;
static const double R_Earth = 6371000;

namespace Geometry {
    double Distance(double latitude_1, double latitude_2, double longitude_1, double longitude_2);
}