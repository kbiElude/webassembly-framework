cmake_minimum_required(VERSION 3.0)

project(webassembly-framework)

if (EMSCRIPTEN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sUSE_GLFW=3")

    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        set(linkFlags "")
    else() # Either MinSizeRel, RelWithDebInfo or Release, all which run with optimizations enabled.
        set(linkFlags "-O2")
    endif()

    if (NOT CMAKE_AR OR NOT EXISTS "${CMAKE_AR}")
        message(FATAL_ERROR "CMAKE_AR='${CMAKE_AR}' does not exist for Emscripten toolchain!")
    endif()

    if (NOT CMAKE_RANLIB OR NOT EXISTS "${CMAKE_RANLIB}")
        message(FATAL_ERROR "CMAKE_RANLIB='${CMAKE_RANLIB}' does not exist for Emscripten toolchain!")
    endif()

    if (NOT CMAKE_C_COMPILER OR NOT EXISTS "${CMAKE_C_COMPILER}")
        message(FATAL_ERROR "CMAKE_C_COMPILER='${CMAKE_C_COMPILER}' does not exist for Emscripten toolchain!")
    endif()

    if (NOT CMAKE_CXX_COMPILER OR NOT EXISTS "${CMAKE_CXX_COMPILER}")
        message(FATAL_ERROR "CMAKE_CXX_COMPILER='${CMAKE_CXX_COMPILER}' does not exist for Emscripten toolchain!")
    endif()

    if (NOT CMAKE_C_SIZEOF_DATA_PTR)
        message(FATAL_ERROR "CMAKE_C_SIZEOF_DATA_PTR was not defined!")
    endif()

    include(CheckIncludeFile)

    check_include_file(math.h      HAVE_MATH_H)
    check_include_file(stdlib.h    HAVE_STDLIB_H)
    check_include_file(sys/types.h HAVE_SYS_TYPES_H)

    find_path(EMSCRIPTEN_INCLUDE_DIR emscripten/emscripten.h
             PATHS /include
             NO_DEFAULT_PATH
             ONLY_CMAKE_FIND_ROOT_PATH
    )

    if(NOT EMSCRIPTEN_INCLUDE_DIR)
        message(FATAL_ERROR "emscripten.h could not be found! Is CMAKE_FIND_ROOT_PATH='${CMAKE_FIND_ROOT_PATH}' correct?")
    endif()

    if (NOT HAVE_STDLIB_H)
        message(FATAL_ERROR "CMake script check_include_file failed! Could not find stdlib.h via it!")
    endif()

    if (NOT HAVE_SYS_TYPES_H)
        message(FATAL_ERROR "CMake script check_include_file failed! Could not find sys/types.h via it!")
    endif()

    if (NOT CMAKE_EXECUTABLE_SUFFIX STREQUAL ".js")
        message(FATAL_ERROR "The default suffix for building executables should be .js!")
    endif()

    if (NOT HAVE_MATH_H)
        message(FATAL_ERROR "CMake script check_include_file failed! Could not find math.h via it!")
    endif()

    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()


include_directories(deps/imgui/repo)
include_directories(deps/imgui/repo/backends)
include_directories(deps/imgui/repo/examples/libs/emscripten)
include_directories(include)

if (NOT EMSCRIPTEN)
    add_subdirectory(deps/glad)
    add_subdirectory(deps/glfw)

    include_directories(deps)
    include_directories(deps/glad/include)
    include_directories(deps/glfw/include)
    include_directories(deps/khronos)
endif()


file(GLOB sourceFiles include/framework.h
                      include/program.h
                      include/sampler.h
                      include/shader.h
                      include/texture.h
                      src/framework.cpp
                      src/program.cpp
                      src/sampler.cpp
                      src/shader.cpp
                      src/texture.cpp)

add_subdirectory(deps/imgui)
add_library     (webassembly-framework STATIC ${sourceFiles})

if (NOT EMSCRIPTEN)
    target_link_libraries(webassembly-framework glad)
    target_link_libraries(webassembly-framework glfw)
endif()

target_link_libraries(webassembly-framework imgui)

set_target_properties(webassembly-framework PROPERTIES LINK_FLAGS "${linkFlags}")
