//
// Created by Artem Kryukov on 24.12.2021.
//
#pragma once

#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include "stop.h"
#include "bus.h"
#include "json.h"

namespace Request {
    enum class Type {
        EMPTY,
        BUS_IN,
        STOP_IN,
        BUS_OUT,
        STOP_OUT,
        ROUTE,
        MAP
    };
}



