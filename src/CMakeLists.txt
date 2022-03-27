CMAKE_MINIMUM_REQUIRED(VERSION 3.16.3)

project(self_made_vec)

include_directories(${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME} test.cpp)

target_compile_options(${PROJECT_NAME} PRIVATE -std=c++17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
