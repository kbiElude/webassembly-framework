cmake_minimum_required(VERSION 3.0)

project(glad)

file               (GLOB glad_sourceFiles "src/glad.c")
include_directories(include)

add_library(glad ${glad_sourceFiles})


