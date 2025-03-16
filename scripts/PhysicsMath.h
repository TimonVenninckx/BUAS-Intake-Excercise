#pragma once

#include <SFML/Graphics.hpp>
# define M_PI           3.14159265358979323846f  /* pi */

namespace PhysicsMath
{
	float distance(sf::Vector2f a, sf::Vector2f b);
	float distanceSquared(sf::Vector2f a, sf::Vector2f b);
	bool nearlyEqual(float a, float b);
	bool nearlyEqual(sf::Vector2f a, sf::Vector2f b);

};

