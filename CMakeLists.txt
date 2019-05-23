cmake_minimum_required(VERSION 3.13)
project(mandelbrot C)

set(CMAKE_C_STANDARD 99)

add_executable(mandelbrot main.c vulkan.h stb_image.h)

# try to find Vulkan
find_package(Vulkan)
IF (${Vulkan_FOUND})
    MESSAGE(STATUS "Found Vulkan at ${Vulkan_INCLUDE_DIR} with ${Vulkan_LIBRARY}")
ENDIF (${Vulkan_FOUND})

# add GLFW library, link Vulkan
add_subdirectory(${PROJECT_SOURCE_DIR}/glfw)
target_link_libraries(${PROJECT_NAME} glfw ${Vulkan_LIBRARY})
target_include_directories(${PROJECT_NAME} PRIVATE glfw/include ${Vulkan_INCLUDE_DIR})

# disable building documentation, tests, and examples if build type is release
if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
endif (CMAKE_BUILD_TYPE EQUAL "RELEASE")