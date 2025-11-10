/*
 * File:   collision.h
 * Authors: Nick Koirala (original version), ahnonay (SFML2 compatibility), switchboy (single pixel test)
 *          Paweł Syska (C++17 refactor + naming convention)
 *          Sean Flannigan (added support for Texture + Transform instead of Sprite)
 *
 * Collision Detection and handling class
 * For SFML2.

Notice from the original version:

(c) 2009 - LittleMonkey Ltd

This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented;
   you must not claim that you wrote the original software.
   If you use this software in a product, an acknowledgment
   in the product documentation would be appreciated but
   is not required.

2. Altered source versions must be plainly marked as such,
   and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any
   source distribution.

 *
 * Created on 30 January 2009, 11:02
 */


// SEAN: I use a single Sprite and render it all over the place, so the original code doesn't quite work for me.
// I need to just use the texture and the transform separately.
// I also want to store off the bitmask separately and pass it in.

#pragma once

#include <SFML/Graphics.hpp>

namespace Collision
{
	using TextureMask = std::vector<std::uint8_t>;

	class CollisionSprite
	{
	public:
		// Pass in nullptr if you don't need inverse transform
		CollisionSprite(const sf::Texture* _texture, sf::Vector2f _originRelative, const sf::Transform* _transform, const sf::Transform* _inverseTransform,
			const TextureMask* _textureMask)
			: texture(_texture), transform(_transform), inverseTransform(_inverseTransform), textureMask(_textureMask),
				m_offset(_originRelative.x * texture->getSize().x, _originRelative.y * texture->getSize().y),
				m_textureRect ({0,0}, sf::Vector2i(texture->getSize()))
		{
			m_textureRect.position = -sf::Vector2i(static_cast<int>(m_offset.x), static_cast<int>(m_offset.y));
		}

		const sf::Texture* texture = nullptr;
		const sf::Transform* transform = nullptr;
		const sf::Transform* inverseTransform = nullptr;
		const TextureMask* textureMask = nullptr;

		sf::FloatRect getLocalBounds() const
		{
			return sf::FloatRect(-m_offset, sf::Vector2f(static_cast<float>(texture->getSize().x), static_cast<float>(texture->getSize().y)));
		}
		sf::FloatRect getGlobalBounds() const
		{
			return (transform->transformRect(getLocalBounds()));
		}

		[[nodiscard]] sf::IntRect getTextureRect() const
		{
			return m_textureRect;
		}

	private:
		sf::IntRect m_textureRect;
		sf::Vector2f m_offset = sf::Vector2f(0.f, 0.f);

	};


	struct OrientedBoundingBox // Used in the BoundingBoxTest
	{
		std::array<sf::Vector2f, 4> points;

		OrientedBoundingBox(const sf::Sprite& sprite) // Calculate the four points of the OBB from a transformed (scaled, rotated...) sprite
		{
			sf::Transform transform = sprite.getTransform();
			sf::IntRect local = sprite.getTextureRect();
			points[0] = transform.transformPoint({ 0.f, 0.f });
			points[1] = transform.transformPoint({ static_cast<float>(local.size.x), 0.f });
			points[2] = transform.transformPoint({ static_cast<float>(local.size.x), static_cast<float>(local.size.y)});
			points[3] = transform.transformPoint({ 0.f, static_cast<float>(local.size.y) });
		}

		OrientedBoundingBox(const CollisionSprite& sprite) // Calculate the four points of the OBB from a transformed (scaled, rotated...) sprite
		{
			sf::IntRect local = sprite.getTextureRect();
			sf::Vector2f topLeft( static_cast<float>(local.position.x), static_cast<float>(local.position.y) );
			sf::Vector2f bottomRight( static_cast<float>( local.position.x + local.size.x ), static_cast<float>( local.position.y + local.size.y ) );
			points[0] = sprite.transform->transformPoint(topLeft);
			points[1] = sprite.transform->transformPoint({ bottomRight.x, topLeft.y });
			points[2] = sprite.transform->transformPoint( bottomRight );
			points[3] = sprite.transform->transformPoint({ topLeft.x, bottomRight.y });
		}

		OrientedBoundingBox(float x, float y, float w, float h)
		{
			points[0] = sf::Vector2f(x, y);
			points[1] = sf::Vector2f(x+w, y);
			points[2] = sf::Vector2f(x+w, y+h);
			points[3] = sf::Vector2f(x, y+h);
		}

		// Project all four points of the OBB onto the given axis and return the dot products of the two outermost points
		void projectOntoAxis(const sf::Vector2f& axis, float& min, float& max)
		{
			min = (points[0].x * axis.x + points[0].y * axis.y);
			max = min;
			for (int j = 1; j < points.size(); ++j)
			{
				float projection = points[j].x * axis.x + points[j].y * axis.y;

				if (projection < min)
					min = projection;
				if (projection > max)
					max = projection;
			}
		}
	};


	//////
	/// Test for a collision between two sprites by comparing the alpha values of overlapping pixels
	/// Supports scaling and rotation
	/// alphaLimit: The threshold at which a pixel becomes "solid". If alphaLimit is 127, a pixel with
	/// alpha value 128 will cause a collision and a pixel with alpha value 126 will not.
	///
	/// This functions creates bitmasks of the textures of the two sprites by
	/// downloading the textures from the graphics card to memory -> SLOW!
	/// You can avoid this by using the "createTextureAndBitmask" function
	//////
	bool pixelPerfectTest(const sf::Sprite& sprite1, const sf::Sprite& sprite2, std::uint8_t alphaLimit = 0);

	bool pixelPerfectTest(const CollisionSprite& sprite1, const CollisionSprite& sprite2, std::uint8_t alphaLimit = 0);

	///////
	/// Test if a single pixel collides By testing the alpha value at the given location.
	/// Supports scaling and rotation
	/// alphaLimit: The threshold at which a pixel becomes "solid". If alphaLimit is 127, a pixel with
	/// alpha value 128 will cause a collision and a pixel with alpha value 126 will not.
	///
	/// This functions creates bitmasks of the textures of the sprite by
	/// downloading the textures from the graphics card to memory -> SLOW!
	/// You can avoid this by using the "createTextureAndBitmask" function
	//////
	bool singlePixelTest(const sf::Sprite& sprite, sf::Vector2f& mousePosition, std::uint8_t alphaLimit);

	bool singlePixelTest(const sf::Texture& texture1, const sf::Transform& transform1,
		sf::Vector2f& position, std::uint8_t alphaLimit);

	//////
	/// Replaces Texture::loadFromFile
	/// Load an image file into the given texture and create a bitmask for it
	/// This is much faster than creating the bitmask for a texture on the first run of "pixelPerfectTest"
	///
	/// The function returns false if the file could not be opened for some reason
	//////
	bool createTextureAndBitmask(sf::Texture& loadInto, const std::string& filename);

	//////
	/// Test for collision using circle collision detection
	/// Radius is averaged from the dimensions of the sprite so
	/// roughly circular objects will be much more accurate
	//////
	bool circleTest(const sf::Sprite& sprite1, const sf::Sprite& sprite2);

	//////
	/// Test for bounding box collision using the Separating Axis Theorem
	/// Supports scaling and rotation
	//////
	bool boundingBoxTest(const sf::Sprite& sprite1, const sf::Sprite& sprite2);

	bool boundingBoxTest(const CollisionSprite& sprite1, const CollisionSprite& sprite2);

}
