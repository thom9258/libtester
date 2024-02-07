cmake_minimum_required(VERSION 3.1)
project(test-libtester)

if (NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    message(STATUS "[${PROJECT_NAME}] has a top-level project called [${CMAKE_PROJECT_NAME}]")
else()
    message(STATUS "[${PROJECT_NAME}] This project is top-level")
endif()


set(CMAKE_CXX_FLAGS "-Wall -Wextra -ggdb")
set(CMAKE_CXX_STANDARD 17)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} test.c)
target_link_libraries(${PROJECT_NAME})
