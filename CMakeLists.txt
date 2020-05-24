cmake_minimum_required(VERSION 3.17)
project(PBR CXX)


set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})


set(pbr_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(pbr_lib_SOURCES ${pbr_SRC_DIR}/core/core.hpp
                    ${pbr_SRC_DIR}/core/pbr_math.hpp
                    ${pbr_SRC_DIR}/core/pbr_concepts.hpp
                    ${pbr_SRC_DIR}/core/geometry.hpp
                    ${pbr_SRC_DIR}/core/geometry.cpp)


add_library(pbr_lib STATIC ${pbr_lib_SOURCES})
set_target_properties(pbr_lib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(pbr_lib INTERFACE ${pbr_SRC_DIR})
#add_compile_options("/std:c++latest")
target_compile_options(pbr_lib PUBLIC "/std:c++latest")
#target_compile_features(pbr_exe PRIVATE cxx_std_20)


if(${CMAKE_PROJECT_NAME} STREQUAL PBR)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

add_subdirectory(docs)
