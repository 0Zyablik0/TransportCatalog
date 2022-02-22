cmake_minimum_required(VERSION 3.19)
project(YandexCourses)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g -O0")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
file(GLOB SOURCES
        bus.cpp
        bus.h
        coordinate_table.h
        coordinate_table.cpp
        database.cpp
        database.h
        Geometry.cpp
        Geometry.h
        graph.h
        interpolator.h
        interpolator.cpp
        json.h
        json.cpp
        manager.h
        manager.cpp
        main.cpp
        mapper.h
        profile.h
        requests.h
        router.h
        stop.h
        stop.cpp
        svg.h
        svg.cpp
        render_manager.h
        render_manager.cpp
        route_manager.cpp
        route_manager.h
        test_runner.h
        types.h
        svg.h
        )

add_executable(Project ${SOURCES})



