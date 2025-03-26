#pragma once

#include "PhysicsBody.h"


class Catapult
{
public:
	Catapult(sf::Vector2f position);
	~Catapult();


	void load(PhysicsBody* body);
	bool isLoaded();
	void update(sf::Vector2f mousePosition);

	bool contains(sf::Vector2f mousePosition);

	PhysicsBody* release();
	void draw(sf::RenderWindow& window);

private:
	sf::RectangleShape shape;
	float maxPullLength;
	PhysicsBody* launchingObject{};
	sf::Vector2f loadPosition;
};

