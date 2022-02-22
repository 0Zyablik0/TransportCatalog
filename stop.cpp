//
// Created by Artem Kryukov on 24.12.2021.
//

#include "stop.h"
#include "Geometry.h"

double Stop::GeometricalDistance(const Stop &stop1, const Stop &stop2) {
    return Geometry::Distance(stop1.GetLatitude(), stop2.GetLatitude(), stop1.GetLongitude(), stop2.GetLongitude());
}

std::ostream& operator<<(std::ostream& stream, const Stop::Stop& stop){
    stream << stop.GetName() <<  " " << stop.GetLatitude() <<  " " << stop.GetLongitude();
    return stream;
}