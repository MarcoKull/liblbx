cmake_minimum_required(VERSION 2.6)
project(liblbx)

# compiler options
if(NOT MSVC)
    add_definitions(-std=c++11 -pipe -pedantic -Wall -Wconversion)
endif()

# library with header file
add_library(lbx src/lbx.cpp)
install(
    TARGETS lbx
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(
    FILES src/lbx.h DESTINATION include
)

# test
enable_testing()
add_executable(liblbx-test src/test.cpp)
add_test(liblbx liblbx-test)
target_link_libraries(liblbx-test lbx)
