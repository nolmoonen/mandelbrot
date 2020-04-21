cmake_minimum_required(VERSION 3.13)
project(mandelbrot C)

set(CMAKE_C_STANDARD 99)

# recursively find source and header files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)

# resource files
add_executable(embedfile embedfile.c)
add_custom_command(OUTPUT default_frag.c COMMAND embedfile default_frag ../res/shader/default.frag DEPENDS res/shader/default.frag)
add_custom_command(OUTPUT default_vert.c COMMAND embedfile default_vert ../res/shader/default.vert DEPENDS res/shader/default.vert)
add_custom_command(OUTPUT select_png.c COMMAND embedfile select_png ../res/tex/select.png DEPENDS res/tex/select.png)
set(RESOURCES default_frag.c default_vert.c select_png.c)

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

# link pthreads
# https://stackoverflow.com/questions/1620918/cmake-and-libpthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(mandelbrot Threads::Threads)