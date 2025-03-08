#include "PhysicsBody.h"
#include "PhysicsMath.h"
#include <iostream>

PhysicsBody::PhysicsBody(float width, float height, sf::Vector2f position, float density, bool isStatic, float restitution,
	ShapeType shapeType)
{

	if (shapeType != ShapeType::Box)
		std::cout << "USING BOX INITIALISER WITHOUT WANTING BOX???\n";

	this->shapeType = shapeType;
	this->density = density;
	this->restitution = restitution;


	this->area = width * height;
	this->mass = area * density;

	this->isStatic = isStatic;


	this->shape = new sf::RectangleShape({ width,height });
	this->shape->setPosition(position);
	sf::Vector2f center = this->shape->getGeometricCenter();
	this->shape->setOrigin(center);

	this->transformedVertices.clear();
	for (int i{ 0 }; i < shape->getPointCount(); i++) {
		transformedVertices.emplace_back(this->shape->getTransform().transformPoint(this->shape->getPoint(i)));
	}

	transformUpdateRequired = false;
}

PhysicsBody::PhysicsBody(float radius, sf::Vector2f position, float density, bool isStatic, float restitution,
	ShapeType shapeType)
{
	if (shapeType != ShapeType::Circle)
		std::cout << "USING CIRCLE INITIALISER WITHOUT WANTING CIRCLE???\n";
	this->radius = radius;
	this->shapeType = shapeType;

	this->density = density;
	this->restitution = restitution;


	this->shape = new sf::CircleShape(radius);
	this->area = radius * radius * M_PI;

	this->mass = area * density;

	this->isStatic = isStatic;
	this->shape->setPosition(position);
	sf::Vector2f center = this->shape->getGeometricCenter();
	this->shape->setOrigin(center);

}



PhysicsBody::~PhysicsBody()
{
	delete this->shape;
	this->shape = nullptr;
	std::cout << "DESTRUCTION\n";
}

void PhysicsBody::SetColor(sf::Color color)
{
	this->shape->setFillColor(color);
}

void PhysicsBody::Draw(sf::RenderWindow& window)
{
	window.draw(*this->shape);
}

void PhysicsBody::Move(sf::Vector2f amount)
{
	//this->shape->setPosition(this->shape->getPosition() + amount);
	this->shape->move(amount);
	this->transformUpdateRequired = true;
}

void PhysicsBody::MoveTo(sf::Vector2f position)
{
	this->shape->setPosition(position);
	this->transformUpdateRequired = true;
}

void PhysicsBody::Rotate(float radii)
{
	this->shape->rotate(sf::radians(radii));
	this->transformUpdateRequired = true;
}

sf::Vector2f PhysicsBody::GetPosition() const
{
	return shape->getTransform().transformPoint(shape->getGeometricCenter());
}

float PhysicsBody::GetRadius() const
{
	return radius;
}

const std::vector<sf::Vector2f>& PhysicsBody::GetTransformedVertices()
{
	if (transformUpdateRequired)
		TransformVertices();
	return transformedVertices;
}

PhysicsBody::ShapeType PhysicsBody::GetShapeType() const
{
	return shapeType;
}

void PhysicsBody::TransformVertices()
{
	if (shapeType == Circle) {
		std::cout << "Why transformvertices on a circle\n";
		return;
	}
	for (unsigned int i{ 0 }; i < transformedVertices.size(); i++) {
		transformedVertices[i] = this->shape->getTransform().transformPoint(this->shape->getPoint(i));
	}
	//std::cout << "we transformed the vertices\n";
	transformUpdateRequired = false;
}

