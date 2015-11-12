project(learnopengl)
cmake_minimum_required(VERSION 2.8)
find_package(PkgConfig REQUIRED)
pkg_check_modules(glfw REQUIRED glfw3)
pkg_check_modules(glew REQUIRED glew)
SET(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or
        CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
set(CMAKE_EXE_LINKER_FLAGS "-g -Wall")
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${CMAKE_EXE_LINKER_FLAGS})
include_directories(${glfw_INCLUDE_DIRS} ${glew_INCLUDE_DIRS})
aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${glfw_LIBRARIES} ${glew_LIBRARIES} m)

