#pragma once

#include <SFML/Graphics.hpp>
#include "../objects/PhysicsBody.h"
#include "AABB.h"



namespace Collision
{ 
	struct HitResult {
		bool collided{ false };
		sf::Vector2f normal{ 0.f,0.f };
		float depth = 0.f;
	};

	struct ContactResult {
		sf::Vector2f contact1{ 0.f,0.f };
		sf::Vector2f contact2{ 0.f,0.f };
		int contactCount{ 0 };
	};

	struct Manifold {
		PhysicsBody* bodyA;
		PhysicsBody* bodyB;
		sf::Vector2f normal;
		float depth;
		sf::Vector2f contact1{ 0.f,0.f };
		sf::Vector2f contact2{ 0.f,0.f };
		unsigned int contactCount{ 0 };

		Manifold(
			PhysicsBody* _bodyA, PhysicsBody* _bodyB,
			sf::Vector2f _normal, float _depth,
			sf::Vector2f _contact1, sf::Vector2f _contact2, unsigned int _contactCount)
			: bodyA{ _bodyA }, bodyB{ _bodyB }, normal{ _normal }, depth{ _depth },
			 contact1{ _contact1 }, contact2{ _contact2 }, contactCount{ _contactCount }
		{}
	};

	struct MinMax {
		float min = INFINITY;
		float max = -INFINITY;
	};

	bool IntersectAABB(AABB& a, AABB& b);

	ContactResult findContactPoints(PhysicsBody* bodyA, PhysicsBody* bodyB);


	HitResult collide(PhysicsBody* bodyA, PhysicsBody* bodyB);


	// returns MinMax
	MinMax projectVertices(const std::vector<sf::Vector2f>& vertices, sf::Vector2f axis);
	
	MinMax projectCircle(sf::Vector2f center,float radius, sf::Vector2f axis);

	// normal returns polygon - circle
	HitResult intersectCirclePolygon(sf::Vector2f circleCenter, float circleRadius, 
									 sf::Vector2f polygonCenter,const std::vector<sf::Vector2f>& vertices);

	// normal returns b-a
	HitResult intersectPolygons(sf::Vector2f polygonCenterA, const std::vector<sf::Vector2f>& verticesA,
								sf::Vector2f polygonCenterB,const std::vector<sf::Vector2f>& verticesB);

	int findClosestPointOnPolygon(sf::Vector2f circleCenter, const std::vector<sf::Vector2f>& vertices);

	// normal returns b-a
	HitResult intersectCircles(sf::Vector2f centerA, float radiusA,
		sf::Vector2f centerB, float radiusB);
};

