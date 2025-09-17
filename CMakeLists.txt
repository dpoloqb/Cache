cmake_minimum_required(VERSION 3.11)
project("2Q_Cache")

enable_testing()

# SET(SRCS
#     source/cache.cpp
# )

SET(TESTS ${CMAKE_SOURCE_DIR}/tests)

# add_library(cachelib STATIC ${})
# target_include_directories(cachelib PUBLIC "include")

add_executable(main.x main.cpp)
# target_link_libraries(main cachelib)

file(GLOB testfiles "tests/*.dat")
foreach(file ${testfiles})
    add_test(
        NAME ${file}
        COMMAND bash -c "${CMAKE_SOURCE_DIR}/runtest.sh ${file} ./main"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(${file} PROPERTIES DEPENDS main)
endforeach()
