#pragma once
#include "AABB.h"
#include <SFML/graphics.hpp>

enum class ShapeType {
	Circle,
	Box,
};

// initiate as pointer
class PhysicsBody
{

	// general initialize function used by both constructors
	void initialize(float density, bool isStatic, float restitution);

public:

	PhysicsBody(PhysicsBody&) = delete;

	// CIRCLE CONSTRUCTOR
	PhysicsBody(float radius,float density, bool isStatic, float restitution);

	// CUBE CONSTRUCTOR
	PhysicsBody(float width, float height, float density, bool isStatic, float restitution);
	~PhysicsBody(); 

	float calculateRotationalInertia();

	void setColor(sf::Color color);
	void draw(sf::RenderWindow& window);
	
	void step(float time,sf::Vector2f gravity, unsigned int iterations);

	void addForce(sf::Vector2f amount);

	void move(sf::Vector2f amount);
	void moveTo(sf::Vector2f position);

	void rotate(float amount);
	void rotateTo(float angle);

	sf::Vector2f getPosition() const;
	float getRadius()const;

	const std::vector<sf::Vector2f>& getTransformedVertices();
	ShapeType getShapeType() const;

	sf::Vector2f linearVelocity{};
	float getMass();
	float getInverseMass();
	float getRestitution();
	bool  getIsStatic();
	float getAngularVelocity();
	void setAngularVelocity(float value);
	float getInverseInertia();
	AABB getAABB();

	float angularVelocity{};

private:

	bool aabbUpdateRequired{ true };
	AABB aabb;
	bool transformUpdateRequired{ true };
	std::vector<sf::Vector2f> transformedVertices;

	void transformVertices();

	sf::Vector2f force{};


	sf::Shape* shape;
	// physics data
	ShapeType shapeType{};
	float density{};
	float mass{};
	float inverseMass{};
	float restitution{};
	float area{};
	float ineratia;
	float inverseIneratia;
	bool isStatic{ false };
	float radius{};
	float width{};
	float height{};
	float staticFriction{};
	float dynamicFriction{};
		


};

