#pragma once
#include "Collision.h"
#include "PhysicsBody.h"

class PhysicsWorld
{
public:
	static const float minBodySize;
	static const float maxBodySize;

	static const float minDensity; // g/cm^3
	static const float maxDensity;
	
	static const unsigned int minIterations;
	static const unsigned int maxIterations;


	PhysicsWorld();

	void addBody(PhysicsBody* body);
	bool removeBody(PhysicsBody* body);

	// returns nullptr if out of bounds
	PhysicsBody* getBody(unsigned int index);

	void step(float time, unsigned int iterations);

	void resolveCollisionBasic(const Collision::Manifold& contact);
	void resolveCollisionWithRotation(const Collision::Manifold& contact);

	unsigned int getBodyCount()const;

	sf::VertexArray contactPointsList;
	sf::VertexArray contactDirectionList;




private:
	void seperateBodies(PhysicsBody* bodyA, PhysicsBody* bodyB, sf::Vector2f mtv);

	void narrowPhase();
	void broadPhase();
	void stepBodies(float time, int total);

	sf::Vector2f gravity;
	std::vector<PhysicsBody*> bodylist;

	std::vector<std::tuple<unsigned int, unsigned int>> contactPairs;

};

