#cmake_minimum_required(VERSION 3.17)
#project(pbr_utests CXX)


#set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
set(pbr_utests_SOURCES test_geometry.cpp)

#enable_testing()


add_executable(pbr_utests main.cpp doctest.h ${pbr_utests_SOURCES})
target_link_libraries(pbr_utests PRIVATE pbr_lib)
#target_include_directories(pbr_utests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
#spdlog_enable_warnings(${test_target})

add_test(NAME pbr_utests COMMAND pbr_utests)
#set_tests_properties(PBR_UTests PROPERTIES RUN_SERIAL ON)
