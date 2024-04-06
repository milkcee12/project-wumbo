# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.12)

project ("Wumbo")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include SDL library
include_directories(Libraries/SDL2/include)
include_directories(Libraries/SDL2_image/include)
include_directories(Libraries/SDL2_mixer/include)
include_directories(Libraries/SDL2_ttf/include)

link_directories(Libraries/SDL2/lib/x64)
link_directories(Libraries/SDL2_image/lib/x64)
link_directories(Libraries/SDL2_mixer/lib/x64)
link_directories(Libraries/SDL2_ttf/lib/x64)

set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W3 /GR /EHsc /wd26812 /w34456 /w34457 /w34458 /w34459 /w35038 /we4715 /we4834 /we4172")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099 /ignore:4075")

add_subdirectory (Wumbo)
