#pragma once

#include <SFML/graphics.hpp>

class PhysicsBody
{
public:

	enum ShapeType{
		Circle,
		Box,
	};

	PhysicsBody(float radius, sf::Vector2f position, float density, bool isStatic, float restitution,
		ShapeType shapeType);

	PhysicsBody(float width, float height, sf::Vector2f position, float density, bool isStatic, float restitution,
		ShapeType shapeType);
	~PhysicsBody(); 

	void SetColor(sf::Color color);
	void Draw(sf::RenderWindow& window);
	
	void Move(sf::Vector2f amount);
	void MoveTo(sf::Vector2f position);
	void Rotate(float radii);
	sf::Vector2f GetPosition() const;
	float GetRadius()const;

	const std::vector<sf::Vector2f>& GetTransformedVertices();

	ShapeType GetShapeType() const;

private:
	bool transformUpdateRequired{ true };
	std::vector<sf::Vector2f> transformedVertices;

	void TransformVertices();

	sf::Shape* shape;
	sf::Vector2f linearVelocity{};
	float rotationalVelocity{};

	float density;
	float mass;
	float restitution;
	float area;
	float radius;
		
	bool isStatic;

	ShapeType shapeType;
};

