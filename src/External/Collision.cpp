/*
 * File:   collision.cpp
 * Author: Nick (original version), ahnonay (SFML2 compatibility), Paweł Syska (C++17 refactor + naming convention)
 */
#include "Collision.h"

#include <map>
#include <vector>
#include <array>


namespace Collision
{

	static std::uint8_t getPixel(const TextureMask& mask, const sf::Texture& tex, uint32_t x, uint32_t y)
	{
		if (x > tex.getSize().x || y > tex.getSize().y)
			return 0;

		return mask[x + y * tex.getSize().x];
	}

	class BitmaskRegistry
	{
	public:
		TextureMask& get(const sf::Texture& tex)
		{
			auto pair = bitmasks.find(&tex);
			if (pair == bitmasks.end())
			{
				return create(tex, tex.copyToImage());
			}

			return pair->second;
		}

		TextureMask& create(const sf::Texture& tex, const sf::Image& img)
		{
			auto mask = TextureMask(tex.getSize().y * tex.getSize().x);

			for (uint32_t y = 0; y < tex.getSize().y; ++y)
			{
				for (uint32_t x = 0; x < tex.getSize().x; ++x)
					mask[x + y * tex.getSize().x] = img.getPixel({ x, y }).a;
			}

			// store and return ref to the mask
			return (bitmasks[&tex] = std::move(mask));
		}
	private:
		std::map<const sf::Texture*, TextureMask> bitmasks;
	};

	// Gets global instance of BitmaskRegistry.
	// "static" to make sure this function doesn't leak to other source file
	static BitmaskRegistry& bitmasks()
	{
		static BitmaskRegistry instance; // Initialized on first use
		return instance;
	}

	bool singlePixelTest(const sf::Sprite& sprite, sf::Vector2f& mousePosition, std::uint8_t alphaLimit)
	{
		if (!sprite.getGlobalBounds().contains({ mousePosition.x, mousePosition.y }))
			return false;

		sf::IntRect subRect = sprite.getTextureRect();
		TextureMask& mask = bitmasks().get(sprite.getTexture());

		sf::Vector2f sv = sprite.getInverseTransform().transformPoint({ mousePosition.x, mousePosition.y });
		// Make sure pixels fall within the sprite's subrect
		if (sv.x > 0 && sv.y > 0 && sv.x < subRect.size.x && sv.y < subRect.size.y)
		{
			return getPixel(mask, sprite.getTexture(), static_cast<int>(sv.x) + subRect.position.x, static_cast<int>(sv.y) + subRect.position.y) > alphaLimit;
		}
		return false;
	}

	bool pixelPerfectTest(const sf::Sprite& sprite1, const sf::Sprite& sprite2, std::uint8_t alphaLimit)
	{
		std::optional<sf::FloatRect> intersection = sprite1.getGlobalBounds().findIntersection(sprite2.getGlobalBounds());
		if (!intersection)
			return false;

		sf::IntRect s1SubRect = sprite1.getTextureRect();
		sf::IntRect s2SubRect = sprite2.getTextureRect();

		TextureMask& mask1 = bitmasks().get(sprite1.getTexture());
		TextureMask& mask2 = bitmasks().get(sprite2.getTexture());

		// Loop through our pixels
		for (float i = intersection->position.x; i < intersection->position.x + intersection->size.x; ++i)
		{
			for (float j = intersection->position.y; j < intersection->position.y + intersection->size.y; ++j)
			{
				sf::Vector2f s1v = sprite1.getInverseTransform().transformPoint({ i, j });
				sf::Vector2f s2v = sprite2.getInverseTransform().transformPoint({ i, j });

				// Make sure pixels fall within the sprite's subrect
				if (s1v.x > 0 && s1v.y > 0 && s2v.x > 0 && s2v.y > 0 &&
					s1v.x < s1SubRect.size.x && s1v.y < s1SubRect.size.y &&
					s2v.x < s2SubRect.size.x && s2v.y < s2SubRect.size.y)
				{

					if (getPixel(mask1, sprite1.getTexture(), static_cast<int>(s1v.x) + s1SubRect.position.x, static_cast<int>(s1v.y) + s1SubRect.position.y) > alphaLimit &&
						getPixel(mask2, sprite2.getTexture(), static_cast<int>(s2v.x) + s2SubRect.position.x, static_cast<int>(s2v.y) + s2SubRect.position.y) > alphaLimit)
						return true;
				}
			}
		}

		return false;
	}

	bool pixelPerfectTest(const CollisionSprite &sprite1, const CollisionSprite &sprite2, std::uint8_t alphaLimit)
	{

		std::optional<sf::FloatRect> intersection = sprite1.getGlobalBounds().findIntersection(sprite2.getGlobalBounds());
		if (!intersection)
			return false;

		sf::IntRect s1SubRect = sprite1.getTextureRect();
		sf::IntRect s2SubRect = sprite2.getTextureRect();

		const TextureMask& mask1 = *sprite1.textureMask;
		const TextureMask& mask2 = *sprite2.textureMask;

		// Loop through our pixels
		for (float i = intersection->position.x; i < intersection->position.x + intersection->size.x; ++i)
		{
			for (float j = intersection->position.y; j < intersection->position.y + intersection->size.y; ++j)
			{
				sf::Vector2f s1v = sprite1.inverseTransform->transformPoint({ i, j });
				sf::Vector2f s2v = sprite2.inverseTransform->transformPoint({ i, j });

				// Make sure pixels fall within the sprite's subrect
				if (s1v.x > 0 && s1v.y > 0 && s2v.x > 0 && s2v.y > 0 &&
					s1v.x < s1SubRect.size.x && s1v.y < s1SubRect.size.y &&
					s2v.x < s2SubRect.size.x && s2v.y < s2SubRect.size.y)
				{

					if (getPixel(mask1, *sprite1.texture, static_cast<int>(s1v.x) + s1SubRect.position.x, static_cast<int>(s1v.y) + s1SubRect.position.y) > alphaLimit &&
						getPixel(mask2, *sprite2.texture, static_cast<int>(s2v.x) + s2SubRect.position.x, static_cast<int>(s2v.y) + s2SubRect.position.y) > alphaLimit)
						return true;
				}
			}
		}

		return false;
	}

	bool createTextureAndBitmask(sf::Texture& loadInto, const std::string& filename)
	{
		sf::Image img{};
		if (!img.loadFromFile(filename))
			return false;
		if (!loadInto.loadFromImage(img))
			return false;

		bitmasks().create(loadInto, img);
		return true;
	}

	sf::Vector2f getSpriteCenter(const sf::Sprite& sprite)
	{
		sf::FloatRect AABB = sprite.getGlobalBounds();
		return sf::Vector2f(AABB.position.x + AABB.size.x / 2.f, AABB.position.y + AABB.size.y / 2.f);
	}

	sf::Vector2f getSpriteSize(const sf::Sprite& sprite)
	{
		sf::IntRect originalSize = sprite.getTextureRect();
		sf::Vector2f scale = sprite.getScale();
		return sf::Vector2f(originalSize.size.x * scale.x, originalSize.size.y * scale.y);
	}

	bool circleTest(const sf::Sprite& sprite1, const sf::Sprite& sprite2)
	{
		sf::Vector2f spr1Size = getSpriteSize(sprite1);
		sf::Vector2f spr2Size = getSpriteSize(sprite2);
		float radius1 = (spr1Size.x + spr1Size.y) / 4.f;
		float radius2 = (spr2Size.x + spr2Size.y) / 4.f;

		sf::Vector2f diff = getSpriteCenter(sprite1) - getSpriteCenter(sprite2);

		return (diff.x * diff.x + diff.y * diff.y <= (radius1 + radius2) * (radius1 + radius2));
	}

	bool boundingBoxTest(const sf::Sprite& sprite1, const sf::Sprite& sprite2)
	{
		OrientedBoundingBox OBB1{ sprite1 };
		OrientedBoundingBox OBB2{ sprite2 };

		// Create the four distinct axes that are perpendicular to the edges of the two rectangles
		std::array<sf::Vector2f, 4> axes{ {
			{ OBB1.points[1].x - OBB1.points[0].x, OBB1.points[1].y - OBB1.points[0].y },
			{ OBB1.points[1].x - OBB1.points[2].x, OBB1.points[1].y - OBB1.points[2].y },
			{ OBB2.points[0].x - OBB2.points[3].x, OBB2.points[0].y - OBB2.points[3].y },
			{ OBB2.points[0].x - OBB2.points[1].x, OBB2.points[0].y - OBB2.points[1].y }
			} };

		for (auto& axis : axes)
		{
			float minOBB1, maxOBB1, minOBB2, maxOBB2;

			// Project the points of both OBBs onto the axis ...
			OBB1.projectOntoAxis(axis, minOBB1, maxOBB1);
			OBB2.projectOntoAxis(axis, minOBB2, maxOBB2);

			// ... and check whether the outermost projected points of both OBBs overlap.
			// If this is not the case, the Separating Axis Theorem states that there can be no collision between the rectangles
			if (!((minOBB2 <= maxOBB1) && (maxOBB2 >= minOBB1)))
				return false;
		}
		return true;
	}

	bool boundingBoxTest(const CollisionSprite &sprite1, const CollisionSprite &sprite2)
	{
		OrientedBoundingBox OBB1{ sprite1 };
		OrientedBoundingBox OBB2{ sprite2 };

		// Create the four distinct axes that are perpendicular to the edges of the two rectangles
		std::array<sf::Vector2f, 4> axes{ {
			{ OBB1.points[1].x - OBB1.points[0].x, OBB1.points[1].y - OBB1.points[0].y },
			{ OBB1.points[1].x - OBB1.points[2].x, OBB1.points[1].y - OBB1.points[2].y },
			{ OBB2.points[0].x - OBB2.points[3].x, OBB2.points[0].y - OBB2.points[3].y },
			{ OBB2.points[0].x - OBB2.points[1].x, OBB2.points[0].y - OBB2.points[1].y }
		} };

		for (auto& axis : axes)
		{
			float minOBB1, maxOBB1, minOBB2, maxOBB2;

			// Project the points of both OBBs onto the axis ...
			OBB1.projectOntoAxis(axis, minOBB1, maxOBB1);
			OBB2.projectOntoAxis(axis, minOBB2, maxOBB2);

			// ... and check whether the outermost projected points of both OBBs overlap.
			// If this is not the case, the Separating Axis Theorem states that there can be no collision between the rectangles
			if (!((minOBB2 <= maxOBB1) && (maxOBB2 >= minOBB1)))
				return false;
		}
		return true;
	}
}
