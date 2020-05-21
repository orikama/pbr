cmake_minimum_required(VERSION 3.13)
project(PBR)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(pbr_exe
               ${SRC_DIR}/main.cpp
               ${SRC_DIR}/core/core.hpp
               ${SRC_DIR}/core/pbr_math.hpp
               ${SRC_DIR}/core/geometry.hpp)

target_include_directories(pbr_exe PRIVATE "${SRC_DIR}")
target_compile_features(pbr_exe PRIVATE cxx_std_20)
