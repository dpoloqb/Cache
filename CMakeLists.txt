cmake_minimum_required(VERSION 3.11)
project(cache)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

function(add_debug_flags TGT) 
    set (DEBUG_FLAGS 
        -Wall 
        -Wextra 
        -pedantic
    )

	target_compile_options(${TGT} PUBLIC
		$<$<CONFIG:Debug>:${DEBUG_FLAGS}>)
endfunction(add_debug_flags)

# 2Q_Cache -------------------------------------------------
add_executable(2q_cache src/main_2q_cache.cpp)
target_include_directories(2q_cache PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ideal_cache ----------------------------------------------
add_executable(ideal_cache src/main_ideal_cache.cpp)
target_include_directories(ideal_cache PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# tests ----------------------------------------------------
include(CTest)
enable_testing()

file(GLOB testfiles "tests/*.dat")
foreach(file ${testfiles})
    get_filename_component(test_name ${file} NAME)
    add_test(
        NAME "2q_${test_name}"
        COMMAND bash -c "${CMAKE_SOURCE_DIR}/runtest.sh ${file} $<TARGET_FILE:2q_cache> 2q"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties("2q_${test_name}" PROPERTIES DEPENDS 2q_cache)
    
    add_test(
        NAME "ideal_${test_name}"
        COMMAND bash -c "${CMAKE_SOURCE_DIR}/runtest.sh ${file} $<TARGET_FILE:ideal_cache> ideal"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties("ideal_${test_name}" PROPERTIES DEPENDS ideal_cache)
endforeach()