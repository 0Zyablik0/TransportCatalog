//
// Created by Artem Kryukov on 27.12.2021.
//

#pragma once

#include "graph.h"
#include <memory>
#include <vector>

using BusId = short;
using StopId = short;
using GraphPtr = std::shared_ptr<Graph::DirectedWeightedGraph<double>>;