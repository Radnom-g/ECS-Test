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
#include "Components/CollisionComponent.h"

using namespace ECS;

namespace ECS_Game
{
    void TestGameWorld::InitialiseInternal()
    {
        SFMLWorld::InitialiseInternal();

        int blockSprite, blobbySprite, fruitSprite = -1;
        resourceManager.LoadTexture("resources/qmark_block.png", blockSprite);
        resourceManager.LoadTexture("resources/blobby.png", blobbySprite);
        resourceManager.LoadTexture("resources/fruit.png", fruitSprite);

        ECS_SFML::TransformComponent* transformComponent = componentManager.GetComponent<ECS_SFML::TransformComponent>();
        ECS_SFML::SpriteComponent* spriteComponent = componentManager.GetComponent<ECS_SFML::SpriteComponent>();
        ECS_SFML::VelocityComponent* velocityComponent = componentManager.GetComponent<ECS_SFML::VelocityComponent>();
        ECS_SFML::ScreenWrapComponent* screenWrapComponent = componentManager.GetComponent<ECS_SFML::ScreenWrapComponent>();
        ECS::CollisionComponent* collisionComponent = componentManager.GetComponent<ECS::CollisionComponent>();
        TreeComponent* treeComponent = componentManager.GetComponent<ECS::TreeComponent>();

        // Collision course Test - creates a person and a block and sets the person moving towards the block.

        Entity blockEntity = entityManager.ActivateEntity("Block");
        testTargetEntity = blockEntity;
        int blockT = transformComponent->AddComponent(blockEntity);
        transformComponent->SetDepth(blockT, 1);
        int blockS = spriteComponent->AddComponent(blockEntity);
        spriteComponent->SetTextureIndex(blockS, blockSprite);
        sf::Vector2f blockPos(64 + (std::rand() % 6) * 32, 64 + (std::rand() % 6) * 32 );
        transformComponent->SetPosition(blockT, blockPos);

        int blockC = collisionComponent->AddComponent(blockEntity);
        collisionComponent->SetCollisionResponseType(blockC, ECollision::ResponseType::Collision );
        collisionComponent->SetCategoryFlags(blockC, 1);


        Entity person = entityManager.ActivateEntity("Blobby");
        testPersonEntity = person;

        int personT = transformComponent->AddComponent(person);
        transformComponent->SetDepth(personT, 2);
        int personS = spriteComponent->AddComponent(person);
        spriteComponent->SetTextureIndex(personS, blobbySprite);
        int wrapComp = screenWrapComponent->AddComponent(person);

        sf::Vector2f personPos( (16 + (std::rand() % 12) ) * 16, ( 16 + (std::rand() % 12) ) * 16 );
        transformComponent->SetPosition(personT, personPos);

        sf::Vector2f personVel = blockPos - personPos;
        if (personVel != sf::Vector2f(0.0f, 0.0f))
            personVel = personVel.normalized();
        personVel *= 20.0f;

        int personVelComp = velocityComponent->AddComponent(person);
        velocityComponent->SetVelocity(personVelComp, personVel);
        velocityComponent->SetMaxSpeed(personVelComp, 30.0f);
        velocityComponent->SetFriction(personVelComp, 4.0f);

        int personC = collisionComponent->AddComponent(person);
        collisionComponent->SetCollisionResponseType(personC, ECollision::ResponseType::Collision | ECollision::ResponseType::Overlap);
        collisionComponent->SetCategoryFlags(personC, 2); // type 2
        collisionComponent->SetCollisionFlags(personC, 1); // hits '1'



        for (int ii = 0; ii < 10; ii++)
        {
            Entity blockEntity = entityManager.ActivateEntity("Block");

            int newT = transformComponent->AddComponent(blockEntity);
            int newS = spriteComponent->AddComponent(blockEntity);

            spriteComponent->SetTextureIndex(newS, blockSprite);
            transformComponent->SetDepth(newT, 1);

            sf::Vector2f randPos(64 + (std::rand() % 15) * 32, 64 + (std::rand() % 15) * 32 );
            //sf::Vector2f randPos(256, 96);

            transformComponent->SetPosition(newT, randPos);
            transformComponent->SetOrigin(newT, sf::Vector2f(0,0));//sf::Vector2f(16,16));

            int newC = collisionComponent->AddComponent(blockEntity);
            collisionComponent->SetCollisionResponseType(newC, ECollision::ResponseType::Collision );
            collisionComponent->SetCategoryFlags(newC, 1);
            //collisionComponent->SetCollisionFlags()

            //entitiesToRotate.push_back(blockEntity);
        }

        for (int ii = 0; ii < 5000; ii++)
        {
            Entity person = entityManager.ActivateEntity("Blobby");

            int newT = transformComponent->AddComponent(person);
            int newS = spriteComponent->AddComponent(person);
            int newWrap = screenWrapComponent->AddComponent(person);

            spriteComponent->SetTextureIndex(newS, blobbySprite);
            transformComponent->SetDepth(newT, 2);

            sf::Vector2f randPos( ((std::rand() % 1920) ) , ((std::rand() % 1080) ) );
            transformComponent->SetPosition(newT, randPos);

            sf::Vector2f vel(-50 + std::rand() % 100, -50 + std::rand() % 100);
            if (vel.x == 0 && vel.y == 0)
                vel.x = 1;
            sf::Vector2f acc = sf::Vector2f(-50 + std::rand() % 100, -50 + std::rand() % 100);
            if (acc.x == 0 && acc.y == 0)
                acc.x = 1;

            vel = vel.normalized();
            vel *= 30 * (static_cast<float>((std::rand() % 100)) / 100.0f);

            acc = acc.normalized();
            acc *= 3 * (static_cast<float>((std::rand() % 100)) / 100.0f);

            int velComp = velocityComponent->AddComponent(person);
            velocityComponent->SetVelocity(velComp, vel);
            velocityComponent->SetAcceleration(velComp, acc);

            int newC = collisionComponent->AddComponent(person);
            collisionComponent->SetCollisionResponseType(newC, ECollision::ResponseType::Collision | ECollision::ResponseType::Overlap);
            collisionComponent->SetCategoryFlags(newC, 2); // type 2
            collisionComponent->SetCollisionFlags(newC, 1); // hits '1'

            // if (ii <= 2)
            // {
            //     Entity fruitEnt = entityManager.ActivateEntity("Fruit");
            //     int fruitT = transformComponent->AddComponent(fruitEnt);
            //     int fruitS = spriteComponent->AddComponent(fruitEnt);
            //
            //     treeComponent->AddChild(person, fruitEnt);
            //
            //     spriteComponent->SetTextureIndex(fruitS, fruitSprite);
            //     transformComponent->SetDepth(fruitT, 3);
            //
            //     sf::Vector2f randFruitOffset( (std::rand() % 2), (std::rand() % 2) );
            //     transformComponent->SetPosition(fruitT, randFruitOffset);
            //     //transformComponent->SetRotation(fruitT, std::rand() % 360);
            //
            //     entitiesToRotate.push_back(fruitEnt);
            // }
        }
    }

    void TestGameWorld::UpdateInternalEarly(float _deltaSeconds)
    {
        for (int ii = 0; ii < entitiesToRotate.size(); ++ii)
        {
            if (entityManager.GetState(entitiesToRotate[ii]) == Alive)
            {
                transformSystem.RotateEntity(entitiesToRotate[ii].index, 90.0f * _deltaSeconds);
            }
        }

        if (testPersonEntity.IsValid() && testTargetEntity.IsValid())
        {
            ECS_SFML::VelocityComponent* velocityComponent = componentManager.GetComponent<ECS_SFML::VelocityComponent>();
            int personVelocityCompInd = velocityComponent->GetComponentIndex(testPersonEntity.index);
            sf::Vector2f dif = transformSystem.GetEntityWorldPosition(testTargetEntity.index) - transformSystem.GetEntityWorldPosition(testPersonEntity.index);

            if (dif != sf::Vector2f(0.0f, 0.0f))
                dif = dif.normalized();
            dif *= 3.0f;
            velocityComponent->SetAcceleration(personVelocityCompInd, dif);
        }

        SFMLWorld::UpdateInternalEarly(_deltaSeconds);
    }
} // ECS_Game