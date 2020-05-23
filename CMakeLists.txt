cmake_minimum_required(VERSION 3.17)
project(PBR CXX)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(pbr_exe
               ${SRC_DIR}/main.cpp
               ${SRC_DIR}/core/core.hpp
               ${SRC_DIR}/core/pbr_math.hpp
               ${SRC_DIR}/core/pbr_concepts.hpp
               ${SRC_DIR}/core/geometry.hpp)

target_include_directories(pbr_exe PRIVATE "${SRC_DIR}")
#add_compile_options("/std:c++latest")
target_compile_options(pbr_exe PUBLIC "/std:c++latest")
#target_compile_features(pbr_exe PRIVATE cxx_std_20)


if(${CMAKE_PROJECT_NAME} STREQUAL PBR)
    #include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()
