cmake_minimum_required(VERSION 3.13)
project(mandelbrot C)

set(CMAKE_C_STANDARD 99)

# recursively find source and header files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.c ${PROJECT_SOURCE_DIR}/third-party/*.c)
file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/src/*.h ${PROJECT_SOURCE_DIR}/third-party/*.h)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/third-party)

# try to find Vulkan
find_package(Vulkan)
IF (${Vulkan_FOUND})
    MESSAGE(STATUS "Found Vulkan at ${Vulkan_INCLUDE_DIR} with ${Vulkan_LIBRARY}")
ENDIF (${Vulkan_FOUND})

# add GLFW library, link Vulkan
add_subdirectory(${PROJECT_SOURCE_DIR}/external/glfw-3.3.2)
target_link_libraries(${PROJECT_NAME} glfw ${Vulkan_LIBRARY})
target_include_directories(${PROJECT_NAME} PRIVATE glfw/include ${Vulkan_INCLUDE_DIR})

# link pthreads
# https://stackoverflow.com/questions/1620918/cmake-and-libpthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(mandelbrot Threads::Threads)

# disable building documentation, tests, and examples if build type is release
if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
endif (CMAKE_BUILD_TYPE EQUAL "RELEASE")