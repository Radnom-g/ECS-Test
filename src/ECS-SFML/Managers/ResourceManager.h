//
// Created by Radnom on 4/11/2025.
//

#pragma once

#include <map>
#include <SFML/Graphics/Texture.hpp>

#include "SFML/Graphics/Sprite.hpp"

namespace ECS_SFML
{
    class ResourceManager
    {
    public:
        virtual ~ResourceManager();

    public:
        // Only call this initially, then GetTexture using the Texture ID instead.
        sf::Texture* GetOrLoadTexture(const std::string& _fileName, int& _outTextureId);
        sf::Texture* GetTexture(int _textureId);
        sf::Sprite* GetSprite(int _textureId);

        // returns lookup id for the texture
        int LoadTexture(const std::string& _fileName);

        void UnloadTexture(int _textureId);

    protected:
        std::map<std::string, sf::Texture*> StringTextureLookup;
        std::map<int, sf::Texture*> IdTextureLookup;
        std::map<int, sf::Sprite*> IdSpriteLookup;

        int loadedTextures = 0;

    };
} // ECS_SFML