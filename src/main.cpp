#include <iostream>

#include <SFML/Graphics.hpp>

#include <../TestGame/Worlds/TestGameWorld.h>

#include "Components/TreeComponent.h"
#include "ECS-SFML/Components/RenderSpriteComponent.h"
#include "ECS-SFML/Components/TransformComponent.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({200, 200}), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    sf::Clock clock;

    ECS_Game::TestGameWorld gameWorld;
    ECS::WorldSettings* worldSettings = new ECS::WorldSettings();

    // Eventually, components will be defined in the world or in the settings
    ECS::ComponentSettings transformSettings = ECS_SFML::TransformComponent::CreateSettings<ECS_SFML::TransformComponent>(1000, 100000);
    ECS::ComponentSettings treeSettings = ECS::TreeComponent::CreateSettings<ECS::TreeComponent>(1000, 100000);
    ECS::ComponentSettings spriteSettings = ECS_SFML::RenderSpriteComponent::CreateSettings<ECS_SFML::RenderSpriteComponent>(1000, 100000);

    worldSettings->ComponentSettings.push_back(transformSettings);
    worldSettings->ComponentSettings.push_back(treeSettings);
    worldSettings->ComponentSettings.push_back(spriteSettings);

    gameWorld.Initialise(worldSettings, &window);

    float accumulator = 0.0f;
    const sf::Time update_ms = sf::seconds(1.f / 60.f);

    sf::Time elapsed = clock.restart();
    while (window.isOpen())
    {
        accumulator += elapsed.asSeconds();

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        elapsed += clock.restart();

        // PROCESS WORLDS
        while (elapsed >= update_ms)
        {
            elapsed -= update_ms;
            gameWorld.Update(update_ms.asSeconds());
        }

        // RENDER WORLDS
        float tween = (update_ms - elapsed) / update_ms;
        window.clear(sf::Color::Black);
        gameWorld.Render(tween);
        window.display();
    }

    return 0;
}