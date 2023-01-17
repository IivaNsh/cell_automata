cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(cell_automata VERSION 1.0)

#glfw section
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

#glad source 
file(GLOB_RECURSE GLAD_SRC lib/glad/src/*.c)

#stb_image source
file(GLOB_RECURSE STB_SRC lib/stb_image/*.cpp)

#imgui source
file(GLOB_RECURSE IMGUI_SRC lib/imgui/*.cpp)

#my source files
file(GLOB_RECURSE MY_SRC src/*.cpp)

#add exe
add_executable(cell_automata ${MY_SRC} ${GLAD_SRC} ${STB_SRC} ${IMGUI_SRC})
target_include_directories(cell_automata PUBLIC lib/glfw/glfw-3.3.8/include) #glfw inlude directory
target_include_directories(cell_automata PUBLIC lib/glad/include) #glad inlcude directory
target_include_directories(cell_automata PUBLIC lib/stb_image) #stb_image include directory
target_include_directories(cell_automata PUBLIC lib/imgui) #imgui include directory
target_include_directories(cell_automata PUBLIC src)
target_link_libraries(cell_automata ${GLFW_STATIC_LIBRARIES}) #glfw static linking
