cmake_minimum_required(VERSION 3.17)

project(PBR_UTests CXX)


#set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
set(PBR_UTESTS_SOURCES test_geometry.cpp)

#enable_testing()

#function(pbr_prepare_test test_target spdlog_lib)
    add_executable(PBR_UTests main.cpp ${PBR_UTESTS_SOURCES})
    #spdlog_enable_warnings(${test_target})
    #target_link_libraries(PBR_UTests PRIVATE ${spdlog_lib})

    add_test(NAME PBR_UTests COMMAND PBR_UTests)
    #set_tests_properties(PBR_UTests PROPERTIES RUN_SERIAL ON)
#endfunction()

#if(PBR_EXECUTE_TESTS)
#    pbr_prepare_test(spdlog-utests spdlog::spdlog)
#endif()
