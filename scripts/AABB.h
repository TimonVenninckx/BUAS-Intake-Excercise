#pragma once

#include <SFML/Graphics.hpp>


struct AABB
{
	sf::Vector2f min;
	sf::Vector2f max;

	AABB(sf::Vector2f _min = { 0.f,0.f }, sf::Vector2f _max = { 0.f,0.f }) :
		min{ _min }, max{ _max }
	{}
};

