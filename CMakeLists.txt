cmake_minimum_required(VERSION 3.15)
project(ascii_water)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

include_directories("${CMAKE_SOURCE_DIR}/include")

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(ascii_water ${SRC_FILES})
target_compile_features(ascii_water PRIVATE cxx_std_20)
target_link_libraries(ascii_water PRIVATE SFML::Graphics)

add_custom_command(TARGET ascii_water POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets/fonts
        ${CMAKE_BINARY_DIR}/fonts)