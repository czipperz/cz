cmake_minimum_required(VERSION 3.3)

project(cz)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRCS src/*.cpp)
add_library(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC include)
export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}LibraryConfig.cmake)

file(GLOB_RECURSE TEST_BASE_SRCS test_base/*.cpp)
set(TEST_BASE_NAME czt)
add_library(${TEST_BASE_NAME} ${TEST_BASE_SRCS})
target_link_libraries(${TEST_BASE_NAME} ${PROJECT_NAME})
target_include_directories(${TEST_BASE_NAME} PUBLIC include)
export(TARGETS ${TEST_BASE_NAME} FILE ${TEST_BASE_NAME}LibraryConfig.cmake)

if (CZ_BUILD_TESTS)
  file(GLOB_RECURSE TEST_SRCS tests/*.cpp)
  set(TEST_EXECUTABLE test)
  add_executable(${TEST_EXECUTABLE} ${TEST_SRCS})
  target_link_libraries(${TEST_EXECUTABLE} ${PROJECT_NAME})
  target_link_libraries(${TEST_EXECUTABLE} ${TEST_BASE_NAME})
  target_include_directories(${TEST_EXECUTABLE} PUBLIC include)
  target_include_directories(${TEST_EXECUTABLE} PUBLIC test_base)
endif()

if (CZ_BENCHMARK)
  file(GLOB_RECURSE BENCH_SRCS bench/*.cpp)
  set(BENCH_EXECUTABLE bench)
  add_executable(${BENCH_EXECUTABLE} ${BENCH_SRCS})

  add_subdirectory(benchmark)
  target_link_libraries(${BENCH_EXECUTABLE} benchmark::benchmark)
  target_include_directories(${BENCH_EXECUTABLE} PUBLIC include)
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror=switch")
endif()
