cmake_minimum_required(VERSION 3.13)
project(mandelbrot C)

set(CMAKE_C_STANDARD 99)

add_executable(mandelbrot main.c vulkan.h stb_image.h)

# add glfw library
add_subdirectory(${PROJECT_SOURCE_DIR}/glfw)
target_link_libraries(${PROJECT_NAME} glfw ${VULKAN_LIBRARY})
target_include_directories(${PROJECT_NAME} PRIVATE glfw/include ${VULKAN_INCLUDE_DIR})

# disable building documentation, tests, and examples if build type is release
if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
endif (CMAKE_BUILD_TYPE EQUAL "RELEASE")