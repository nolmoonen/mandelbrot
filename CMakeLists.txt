cmake_minimum_required(VERSION 3.13)
project(mandelbrot C)

set(CMAKE_C_STANDARD 99)

# recursively find source and header files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)

# resource files
add_executable(embedfile embedfile.c)
add_custom_command(OUTPUT ortho_tex_frag.c
        COMMAND embedfile ortho_tex_frag ../res/shader/ortho_tex.frag DEPENDS res/shader/ortho_tex.frag)
add_custom_command(OUTPUT ortho_tex_vert.c
        COMMAND embedfile ortho_tex_vert ../res/shader/ortho_tex.vert DEPENDS res/shader/ortho_tex.vert)
set(RESOURCES ortho_tex_frag.c ortho_tex_vert.c)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/third-party)

# disable glfw documentation, tests, and examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# build and link GLFW
add_subdirectory(${PROJECT_SOURCE_DIR}/external/glfw-3.3.2)
target_link_libraries(${PROJECT_NAME} glfw)

# build and link GLAD
add_subdirectory(${PROJECT_SOURCE_DIR}/external/glad-0.1.33)
target_link_libraries(${CMAKE_PROJECT_NAME} glad)

# include stb
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/external/stb)

# include linmath
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/external/linmath)

# statically link pthreads (https://stackoverflow.com/questions/1620918/cmake-and-libpthread)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} Threads::Threads -static)