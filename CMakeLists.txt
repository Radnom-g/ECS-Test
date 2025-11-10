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
        src/ECS/Components/IComponent.h
        src/ECS/Components/TreeComponent.h
        src/ECS/Entities/EntityManager.cpp
        src/ECS/Entities/EntityManager.h
        src/ECS/Systems/ISystem.cpp
        src/ECS/Systems/ISystem.h
        src/ECS/Worlds/World.cpp
        src/ECS/Worlds/World.h
        src/ECS-SFML/Components/SpriteComponent.h
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
        src/ECS/Components/ComponentManager.cpp
        src/ECS/Components/ComponentManager.h
        src/ECS/Components/IComponent.cpp
        src/ECS/Components/IComponent.cpp
        src/ECS/Worlds/WorldContext.h
        src/ECS/Worlds/WorldContext.h
        src/ECS-SFML/Components/TransformComponent.cpp
        src/ECS/Components/TreeComponent.cpp
        src/ECS-SFML/Components/SpriteComponent.cpp
        src/ECS-SFML/Components/Transform.h
        src/ECS-SFML/Worlds/SFMLWorldContext.h
        src/ECS/Worlds/WorldSettings.h
        src/ECS/Components/ComponentSettings.h
        src/ECS-SFML/Components/VelocityComponent.cpp
        src/ECS-SFML/Components/VelocityComponent.h
        src/ECS-SFML/Components/TimerComponent.cpp
        src/ECS-SFML/Components/TimerComponent.h
        src/ECS-SFML/Components/ScreenWrapComponent.cpp
        src/ECS-SFML/Components/ScreenWrapComponent.h
        src/ECS-SFML/Systems/TimerSystem.cpp
        src/ECS-SFML/Systems/TimerSystem.h
        src/ECS-SFML/Systems/MovementSystem.cpp
        src/ECS-SFML/Systems/MovementSystem.h
        src/ECS-SFML/Worlds/SFMLWorld.cpp
        src/ECS-SFML/Worlds/SFMLWorld.h
        src/ECS-SFML/Components/BoundsComponent.cpp
        src/ECS-SFML/Components/BoundsComponent.h
        src/ECS/Components/CollisionComponent.cpp
        src/ECS/Components/CollisionComponent.h
        src/ECS-SFML/Systems/CollisionSystem.cpp
        src/ECS-SFML/Systems/CollisionSystem.h
        src/External/SmallList.h
        src/External/UGrid.cpp
        src/External/UGrid.h
        src/External/IndexList.h
        src/ECS/Systems/SystemManager.cpp
        src/ECS/Systems/SystemManager.h
        src/External/Collision.cpp
        src/External/Collision.h
        src/External/Collision.h
)
target_include_directories(ECSTest PRIVATE src/ECS)
target_compile_features(ECSTest PRIVATE cxx_std_17)
target_link_libraries(ECSTest PRIVATE SFML::Graphics)
