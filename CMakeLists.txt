cmake_minimum_required(VERSION 3.17)
project(PBR CXX)


set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})


set(pbr_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(pbr_SRC_CORE_DIR "${pbr_SRC_DIR}/core")
set(pbr_lib_CORE_SOURCES ${pbr_SRC_CORE_DIR}/core.hpp
                         ${pbr_SRC_CORE_DIR}/pbr_math.hpp
                         ${pbr_SRC_CORE_DIR}/pbr_concepts.hpp
                         ${pbr_SRC_CORE_DIR}/geometry.hpp
                         ${pbr_SRC_CORE_DIR}/geometry.cpp
                         ${pbr_SRC_CORE_DIR}/transform.hpp
                         ${pbr_SRC_CORE_DIR}/interaction.hpp
                         ${pbr_SRC_CORE_DIR}/shape.h
                         ${pbr_SRC_CORE_DIR}/shape.cpp
                         ${pbr_SRC_CORE_DIR}/stats.h
                         ${pbr_SRC_CORE_DIR}/stats.cpp
                         ${pbr_SRC_CORE_DIR}/primitive.h
                         ${pbr_SRC_CORE_DIR}/primitive.cpp)

set(pbr_SRC_SHAPES_DIR "${pbr_SRC_DIR}/shapes")
set(pbr_lib_SHAPES_SOURCES ${pbr_SRC_SHAPES_DIR}/sphere.h
                           ${pbr_SRC_SHAPES_DIR}/sphere.cpp
                           ${pbr_SRC_SHAPES_DIR}/cylinder.h
                           ${pbr_SRC_SHAPES_DIR}/cylinder.cpp
                           ${pbr_SRC_SHAPES_DIR}/disk.h
                           ${pbr_SRC_SHAPES_DIR}/disk.cpp
                           ${pbr_SRC_SHAPES_DIR}/cone.h
                           ${pbr_SRC_SHAPES_DIR}/cone.cpp
                           ${pbr_SRC_SHAPES_DIR}/paraboloid.h
                           ${pbr_SRC_SHAPES_DIR}/paraboloid.cpp
                           ${pbr_SRC_SHAPES_DIR}/hyperboloid.h
                           ${pbr_SRC_SHAPES_DIR}/hyperboloid.cpp
                           ${pbr_SRC_SHAPES_DIR}/triangle.h
                           ${pbr_SRC_SHAPES_DIR}/triangle.cpp)


add_library(pbr_lib STATIC ${pbr_lib_CORE_SOURCES} ${pbr_lib_SHAPES_SOURCES})
set_target_properties(pbr_lib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(pbr_lib INTERFACE ${pbr_SRC_CORE_DIR} ${pbr_SRC_SHAPES_DIR})

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(pbr_lib PUBLIC "-std=c++20")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(pbr_lib PUBLIC "/std:c++latest")
endif()
#add_compile_options("/std:c++latest")

#target_compile_features(pbr_exe PRIVATE cxx_std_20)


if(${CMAKE_PROJECT_NAME} STREQUAL PBR)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

#add_subdirectory(docs)
