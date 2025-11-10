//
// Created by Radnom on 4/11/2025.
//

#pragma once

#include <map>
#include <vector>
#include <SFML/Graphics/Texture.hpp>

#include "SFML/Graphics/Sprite.hpp"

namespace ECS_SFML
{
	using TextureMask = std::vector<std::uint8_t>;

    class ResourceManager
    {
    public:
        virtual ~ResourceManager();

    public:
        // Only call this initially, then GetTexture using the Texture ID instead.
        bool LoadTexture(const std::string& _fileName, int& _outTextureId);

        sf::Texture* GetTexture(int _textureId);
        sf::Sprite* GetSprite(int _textureId);
        TextureMask* GetTextureMask(int _textureId);

        // returns lookup id for the texture
        int LoadTexture(const std::string& _fileName);

        void UnloadTexture(int _textureId);

    protected:
        TextureMask CreateBitmask(const sf::Texture& _tex, const sf::Image& _img);

    protected:
        std::map<std::string, int> StringIndexLookup;
        std::vector<sf::Texture*> IdTextureLookup;
        std::vector<sf::Sprite*> IdSpriteLookup;
        std::vector<TextureMask> IdBitmaskLookup;

        int loadedTextures = 0;

    };
} // ECS_SFML