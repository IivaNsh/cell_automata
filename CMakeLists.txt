cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(cell_automata VERSION 1.0)

#glfw section
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/glfw/glfw-3.3.8)
#glad source 
file(GLOB_RECURSE GLAD_SRC lib/glad/src/*.c)

#stb_image source
file(GLOB_RECURSE STB_SRC lib/stb_image/*.cpp)

#imgui source
file(GLOB_RECURSE IMGUI_SRC lib/imgui/*.cpp)

#my source files
file(GLOB_RECURSE MY_SRC src/*.cpp)



if($(CMAKE_SYSTEM_NAME) MATCHES "Windows")

    set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CXX_STANDARD_LIBRARIES}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")

endif()


#add exe
add_executable(cell_automata ${MY_SRC} ${GLAD_SRC} ${STB_SRC} ${IMGUI_SRC})
target_include_directories(cell_automata PUBLIC lib/glfw/glfw-3.3.8/include) #glfw inlude directory
target_include_directories(cell_automata PUBLIC lib/glad/include) #glad inlcude directory
target_include_directories(cell_automata PUBLIC lib/stb_image) #stb_image include directory
target_include_directories(cell_automata PUBLIC lib/imgui) #imgui include directory
target_include_directories(cell_automata PUBLIC src)
target_link_libraries(cell_automata glfw) #glfw static linking
