//
// Created by Radnom on 4/11/2025.
//

#include "ResourceManager.h"

#include <vector>

#include "SFML/Graphics/Image.hpp"

namespace ECS_SFML
{
    ResourceManager::~ResourceManager()
    {
        for (int i = 0; i < IdTextureLookup.size(); i++)
        {
            UnloadTexture(i);
        }
    }

    bool ResourceManager::LoadTexture(const std::string& _fileName, int &_outTextureId)
    {
        // Try and find it.
        auto iter = StringIndexLookup.find(_fileName);
        if (iter != StringIndexLookup.end())
        {
            // Also get the Texture ID so that we don't have to look it up by filename any more.
            _outTextureId = iter->second;
            return true;
        }

        // Try and load it.
        _outTextureId = LoadTexture(_fileName);
        if (_outTextureId != -1)
        {
            return true;
        }

        return false;
    }

    sf::Texture * ResourceManager::GetTexture(int _textureId)
    {
        if (_textureId < 0 || _textureId >= IdTextureLookup.size())
            return nullptr;
        return (IdTextureLookup[_textureId]);
    }

    sf::Sprite * ResourceManager::GetSprite(int _textureId)
    {
        if (_textureId < 0 || _textureId >= IdSpriteLookup.size())
            return nullptr;
        return (IdSpriteLookup[_textureId]);
    }

    TextureMask * ResourceManager::GetTextureMask(int _textureId)
    {
        if (_textureId < 0 || _textureId >= IdBitmaskLookup.size())
            return nullptr;
        return (&IdBitmaskLookup[_textureId]);
    }

    int ResourceManager::LoadTexture(const std::string& _fileName)
    {
        if (StringIndexLookup.contains(_fileName))
        {
            assert(false && "LoadTexture already loaded!");
        }

        sf::Texture* texture = new sf::Texture();
        bool bLoaded = texture->loadFromFile(_fileName);
        if (bLoaded)
        {
            int newId = loadedTextures;
            loadedTextures++;

            std::pair<std::string, int> idPair(_fileName, newId);

            StringIndexLookup.emplace(idPair);
            IdTextureLookup.push_back(texture);
            IdSpriteLookup.push_back(new sf::Sprite(*texture));
            IdBitmaskLookup.push_back(CreateBitmask(*texture, texture->copyToImage()));

            return newId;
        }

        delete texture;
        return -1;
    }

    void ResourceManager::UnloadTexture(int _textureId)
    {
        if (_textureId < 0 || _textureId > IdTextureLookup.size())
            return;

        sf::Texture* texture = IdTextureLookup[_textureId];
        delete texture;
        IdTextureLookup[_textureId] = nullptr;

        sf::Sprite * sprite = IdSpriteLookup[_textureId];
        delete sprite;
        IdSpriteLookup[_textureId] = nullptr;
    }

    TextureMask ResourceManager::CreateBitmask(const sf::Texture &_tex, const sf::Image &_img)
    {
        TextureMask _outMask = TextureMask(_tex.getSize().y * _tex.getSize().x);

        for (uint32_t y = 0; y < _tex.getSize().y; ++y)
        {
            for (uint32_t x = 0; x < _tex.getSize().x; ++x)
                _outMask[x + y * _tex.getSize().x] = _img.getPixel({ x, y }).a;
        }
        return _outMask;
    }
} // ECS_SFML