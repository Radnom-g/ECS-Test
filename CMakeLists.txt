cmake_minimum_required(VERSION 4.0)
project(ECSTest LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

set(CMAKE_CXX_STANDARD 20)

add_executable(ECSTest src/main.cpp
        src/ECS/Accessors/ComponentAccessor.h
        src/ECS/Accessors/DepthAccessor.cpp
        src/ECS/Accessors/DepthAccessor.h
        src/ECS/Accessors/IComponentAccessor.h
        src/ECS/Components/DepthComponent.h
        src/ECS/Components/IComponent.h
        src/ECS/Components/TreeComponent.h
        src/ECS/Entities/Entity.cpp
        src/ECS/Entities/Entity.h
        src/ECS/Managers/AccessorManager.h
        src/ECS/Managers/AccessorManager.cpp
        src/ECS/Managers/EntityManager.cpp
        src/ECS/Managers/EntityManager.h
        src/ECS/Managers/RenderManager.cpp
        src/ECS/Managers/RenderManager.h
        src/ECS/Systems/ISystem.cpp
        src/ECS/Systems/ISystem.h
        src/ECS/Worlds/World.cpp
        src/ECS/Worlds/World.h
        src/ECS-SFML/Accessors/AccessorTypes.h
        src/ECS-SFML/Components/RenderSpriteComponent.h
        src/ECS-SFML/Components/TransformComponent.h
        src/ECS-SFML/Managers/ResourceManager.cpp
        src/ECS-SFML/Managers/ResourceManager.h
        src/ECS-SFML/Systems/RenderSystem.cpp
        src/ECS-SFML/Systems/RenderSystem.h
        src/TestGame/Worlds/TestGameWorld.cpp
        src/TestGame/Worlds/TestGameWorld.h
        src/ECS-SFML/Systems/TransformSystem.cpp
        src/ECS-SFML/Systems/TransformSystem.h
        src/ECS-SFML/Systems/TransformSystem.h
)
target_include_directories(ECSTest PRIVATE src/ECS)
target_compile_features(ECSTest PRIVATE cxx_std_17)
target_link_libraries(ECSTest PRIVATE SFML::Graphics)
