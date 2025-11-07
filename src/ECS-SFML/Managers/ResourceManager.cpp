//
// Created by Radnom on 4/11/2025.
//

#include "ResourceManager.h"

#include <vector>

namespace ECS_SFML
{
    ResourceManager::~ResourceManager()
    {
        std::vector<int> TextureIds;
        for (const auto& kv : IdTextureLookup)
        {
            TextureIds.push_back(kv.first);
        }
        for (int& i : TextureIds)
        {
            UnloadTexture(i);
        }
    }

    sf::Texture * ResourceManager::GetOrLoadTexture(const std::string& _fileName, int &_outTextureId)
    {
        // Try and find it.
        auto iter = StringTextureLookup.find(_fileName);
        if (iter != StringTextureLookup.end())
        {
            sf::Texture* texture = iter->second;

            // Also get the Texture ID so that we don't have to look it up by filename any more.
            for (auto it = IdTextureLookup.begin(); it != IdTextureLookup.end(); ++it)
            {
                if (it->second == texture)
                {
                    _outTextureId = it->first;
                }
            }

            return texture;
        }

        // Try and load it.
        _outTextureId = LoadTexture(_fileName);
        if (_outTextureId != -1)
        {
            return IdTextureLookup[_outTextureId];
        }

        return nullptr;
    }

    sf::Texture * ResourceManager::GetTexture(int _textureId)
    {
        auto iter = IdTextureLookup.find(_textureId);
        if (iter != IdTextureLookup.end())
        {
            return iter->second;
        }
        return nullptr;
    }

    sf::Sprite * ResourceManager::GetSprite(int _textureId)
    {
        auto iter = IdSpriteLookup.find(_textureId);
        if (iter != IdSpriteLookup.end())
        {
            return iter->second;
        }
        return nullptr;
    }

    int ResourceManager::LoadTexture(const std::string& _fileName)
    {
        if (StringTextureLookup.contains(_fileName))
        {
            assert(false && "LoadTexture already loaded!");
        }

        sf::Texture* texture = new sf::Texture();

        bool bLoaded = texture->loadFromFile(_fileName);
        if (bLoaded)
        {
            int newId = loadedTextures + 1;
            loadedTextures++;

            std::pair<int, sf::Texture*> idPair(newId, texture);
            IdTextureLookup.emplace(idPair);

            std::pair<std::string, sf::Texture*> strPair(_fileName, texture);
            StringTextureLookup.emplace(strPair);

            sf::Sprite* sprite = new sf::Sprite(*texture);
            IdSpriteLookup.emplace(std::pair<int, sf::Sprite*>(newId, sprite));

            return newId;
        }

        delete texture;
        return -1;
    }

    void ResourceManager::UnloadTexture(int _textureId)
    {
        auto idIter = IdTextureLookup.find(_textureId);
        if (idIter != IdTextureLookup.end())
        {
            for (auto strIter = StringTextureLookup.begin(); strIter != StringTextureLookup.end(); ++strIter)
            {
                if (strIter->second == idIter->second)
                {
                    StringTextureLookup.erase(strIter);
                    break;
                }
            }

            delete idIter->second;
            IdTextureLookup.erase(idIter);
        }

        auto idSpriteIter = IdSpriteLookup.find(_textureId);
        if (idSpriteIter != IdSpriteLookup.end())
        {
            delete idSpriteIter->second;
            IdSpriteLookup.erase(idSpriteIter);
        }

    }
} // ECS_SFML