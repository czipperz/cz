project(cz)
cmake_minimum_required(VERSION 2.6)

file(GLOB_RECURSE SRCS src/*.cpp)
add_library(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC include)

file(GLOB_RECURSE TEST_SRCS tests/*.cpp)
set(TEST_EXECUTABLE test)
add_executable(${TEST_EXECUTABLE} ${TEST_SRCS})
target_link_libraries(${TEST_EXECUTABLE} ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC include)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /std:c++11")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
endif()
