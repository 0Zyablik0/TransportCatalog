
#include "manager.h"
#include <iostream>
#include "json.h"


const size_t precision = 6;

int main() {

    Json::Document input_document = Json::Load(std::cin);
    Manager bus_manager;
    std::cout.precision(precision);
    std::cout << bus_manager.JSONRequest(input_document);

}