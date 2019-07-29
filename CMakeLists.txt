project(cz)
cmake_minimum_required(VERSION 2.6)

file(GLOB SRCS src/*.cpp)
add_library(${PROJECT_NAME} ${SRCS})

file(GLOB TEST_SRCS tests/*.cpp)
set(TEST_EXECUTABLE test)
add_executable(${TEST_EXECUTABLE} ${TEST_SRCS})
target_link_libraries(${TEST_EXECUTABLE} ${PROJECT_NAME})
