//
// Created by Radnom on 4/11/2025.
//

#include "TestGameWorld.h"

#include <debugapi.h>

#include "Components/ComponentManager.h"
#include "../ECS-SFML/Components/TransformComponent.h"
#include "../ECS-SFML/Components/SpriteComponent.h"
#include "../ECS-SFML/Components/VelocityComponent.h"
#include "../ECS-SFML/Components/ScreenWrapComponent.h"
#include "Components/TreeComponent.h"

using namespace ECS;

namespace ECS_Game
{
    void TestGameWorld::InitialiseInternal()
    {
        SFMLWorld::InitialiseInternal();

        int blockSprite, blobbySprite, fruitSprite = -1;
        resourceManager.GetOrLoadTexture("resources/qmark_block.png", blockSprite);
        resourceManager.GetOrLoadTexture("resources/blobby.png", blobbySprite);
        resourceManager.GetOrLoadTexture("resources/fruit.png", fruitSprite);

        ECS_SFML::TransformComponent* transformComponent = componentManager.GetComponent<ECS_SFML::TransformComponent>();
        ECS_SFML::SpriteComponent* spriteComponent = componentManager.GetComponent<ECS_SFML::SpriteComponent>();
        ECS_SFML::VelocityComponent* velocityComponent = componentManager.GetComponent<ECS_SFML::VelocityComponent>();
        ECS_SFML::ScreenWrapComponent* screenWrapComponent = componentManager.GetComponent<ECS_SFML::ScreenWrapComponent>();
        TreeComponent* treeComponent = componentManager.GetComponent<ECS::TreeComponent>();


        ///////////////////////////////////////////////////////////
        // Testing this list:
        IndexList testList;
        testList.push_back(0);
        testList.push_back(3);
        testList.push_back(5);
        testList.push_back(4);
        testList.push_back(4);
        testList.push_back(3);
        testList.push_back(6);
        testList.push_back(3);

        int elementsCount = testList.size();
        testList.remove_matches(3);

        std::stringstream strDebug;
        strDebug << "-----elements: [";
        for (int ii = 0; ii < elementsCount; ++ii)
        {
            strDebug << testList[ii];
        }
        strDebug << "]";
        OutputDebugString(strDebug.str().c_str());


        std::stringstream strDebug2;
        strDebug << "-----iterating: [";
        for (auto ii : testList)
        {
            strDebug << testList[ii];
        }
        strDebug << "]";
        OutputDebugString(strDebug.str().c_str());
        ///////////////////////////////////////////////////////////


        for (int ii = 0; ii < 10; ii++)
        {
            Entity blockEntity = entityManager.ActivateEntity("Block");

            int newT = transformComponent->AddComponent(blockEntity);
            int newS = spriteComponent->AddComponent(blockEntity);

            spriteComponent->SetTextureIndex(newS, blockSprite);
            transformComponent->SetDepth(newT, 1);

            sf::Vector2f randPos((std::rand() % 6) * 32, (std::rand() % 6) * 32 );

            transformComponent->SetPosition(newT, randPos);
            transformComponent->SetOrigin(newT, sf::Vector2f(0,0));//sf::Vector2f(16,16));
                entitiesToRotate.push_back(blockEntity);
        }

        for (int ii = 0; ii < 5; ii++)
        {
            Entity person = entityManager.ActivateEntity("Blobby");

            int newT = transformComponent->AddComponent(person);
            int newS = spriteComponent->AddComponent(person);
            int newWrap = screenWrapComponent->AddComponent(person);

            spriteComponent->SetTextureIndex(newS, blobbySprite);
            transformComponent->SetDepth(newT, 2);

            sf::Vector2f randPos( ((std::rand() % 4) ) * 16, ((std::rand() % 4) ) * 16 );
            transformComponent->SetPosition(newT, randPos);

            sf::Vector2f vel(std::rand() % 3, std::rand() % 3);
            sf::Vector2f acc(0,0);
            if (std::rand() % 2 == 0 )
            {
                acc = sf::Vector2f(std::rand() % 3, std::rand() % 3);
            }

            int velComp = velocityComponent->AddComponent(person.index);
            velocityComponent->SetVelocity(velComp, vel);
            velocityComponent->SetAcceleration(velComp, acc);

            if (ii <= 2)
            {
                Entity fruitEnt = entityManager.ActivateEntity("Fruit");
                int fruitT = transformComponent->AddComponent(fruitEnt);
                int fruitS = spriteComponent->AddComponent(fruitEnt);

                treeComponent->AddChild(person, fruitEnt);

                spriteComponent->SetTextureIndex(fruitS, fruitSprite);
                transformComponent->SetDepth(fruitT, 3);

                sf::Vector2f randFruitOffset( (std::rand() % 2), (std::rand() % 2) );
                transformComponent->SetPosition(fruitT, randFruitOffset);
                //transformComponent->SetRotation(fruitT, std::rand() % 360);

                entitiesToRotate.push_back(fruitEnt);
            }
        }
    }

    void TestGameWorld::UpdateInternal(float _deltaSeconds)
    {
        for (int ii = 0; ii < entitiesToRotate.size(); ++ii)
        {
            if (entityManager.GetState(entitiesToRotate[ii]) == Alive)
            {
                transformSystem.RotateEntity(entitiesToRotate[ii].index, 90.0f * _deltaSeconds);
            }
        }

        SFMLWorld::UpdateInternal(_deltaSeconds);
    }
} // ECS_Game