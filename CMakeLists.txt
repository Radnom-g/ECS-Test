cmake_minimum_required(VERSION 4.0)
project(ECSTest LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

set(CMAKE_CXX_STANDARD 20)

add_executable(ECSTest src/main.cpp)
target_compile_features(ECSTest PRIVATE cxx_std_17)
target_link_libraries(ECSTest PRIVATE SFML::Graphics)
