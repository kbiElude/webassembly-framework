cmake_minimum_required(VERSION 3.0)

project(imgui)

include_directories(../glfw/include)
include_directories(repo)
include_directories(repo/examples)
include_directories(repo/examples/example_null)

file (GLOB imgui_sourceFiles repo/backends/imgui_impl_glfw.cpp
                             repo/backends/imgui_impl_opengl3.cpp
                             repo/imgui.cpp
                             repo/imgui_draw.cpp
                             repo/imgui_tables.cpp
                             repo/imgui_widgets.cpp)

add_library(imgui ${imgui_sourceFiles})


