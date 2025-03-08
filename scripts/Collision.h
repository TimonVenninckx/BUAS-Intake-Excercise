#pragma once

#include <SFML/Graphics.hpp>




namespace Collision
{ 
	struct HitResult {
		bool collided{ false };
		sf::Vector2f normal{ 0.f,0.f };
		float depth = 0.f;
	};

	struct MinMax {
		float min = INFINITY;
		float max = -INFINITY;
	};


	// returns MinMax
	MinMax ProjectVertices(const std::vector<sf::Vector2f>& vertices, sf::Vector2f axis);

	MinMax ProjectCircle(sf::Vector2f center,float radius, sf::Vector2f axis);

	// normal returns polygon - circle
	HitResult IntersectCirclePolygon(sf::Vector2f circleCenter, float circleRadius, 
		const std::vector<sf::Vector2f>& vertices);
	// normal returns b-a
	HitResult IntersectPolygons(const std::vector<sf::Vector2f>& verticesA,
		const std::vector<sf::Vector2f>& verticesB);

	int FindClosestPointOnPolygon(sf::Vector2f circleCenter, const std::vector<sf::Vector2f>& vertices);
	sf::Vector2f FindArithmeticMean(const std::vector<sf::Vector2f>& vertices);

	// normal returns b-a
	HitResult IntersectCircles(sf::Vector2f centerA, float radiusA,
		sf::Vector2f centerB, float radiusB);
};

