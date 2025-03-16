#include "PhysicsBody.h"
#include "PhysicsMath.h"
#include "Collision.h"
#include <iostream>
#include "TextureLoader.h"


void PhysicsBody::initialize(float density, bool isStatic, float restitution)
{


	this->density = density;
	this->restitution = restitution;
	this->isStatic = isStatic;

	sf::Vector2f center = this->shape->getGeometricCenter();
	this->shape->setOrigin(center);


	this->force = { 0.f,0.f };
	if (!this->isStatic) {
		this->mass = this->area * this->density;
		this->ineratia = this->calculateRotationalInertia();
		this->inverseMass = 1.f / this->mass;
		this->inverseIneratia = 1.f / this->ineratia;
	}
	else {
		this->mass = 0.f;
		this->ineratia = 0.f;
		this->inverseMass = 0.f;
		this->inverseIneratia = 0.f;
	}

	this->angularVelocity = 0.f;

	this->aabbUpdateRequired = true;
	std::cout << ineratia << " is the inertia" << '\n';
}


// box creation
PhysicsBody::PhysicsBody(float width, float height, float density, bool isStatic, float restitution)
{
	this->width = width;
	this->height = height;
	this->shapeType = ShapeType::Box;
	this->area = width * height;
	this->shape = new sf::RectangleShape({ width,height });

	this->initialize(density, isStatic, restitution);


	// loading amount of vertices on creation.
	this->transformedVertices.clear();
	for (unsigned int i{ 0 }; i < this->shape->getPointCount(); i++) {
		this->transformedVertices.push_back(this->shape->getTransform().transformPoint(this->shape->getPoint(i)));
		//std::cout << "POINT" << i << ':' << this->transformedVertices[i].x << ',' << this->transformedVertices[i].y << '\n';
	}
	this->transformUpdateRequired = false;

	//sf::Texture* texture = TextureLoader::LoadTexture("blocksmiley.png");
	//this->shape->setTexture(texture);
}

// circle creation
PhysicsBody::PhysicsBody(float radius,float density, bool isStatic, float restitution)
{
	this->shapeType = ShapeType::Circle;
	this->radius = radius;
	this->area = radius * radius * M_PI;
	this->shape = new sf::CircleShape(radius);

	this->initialize(density, isStatic, restitution);

	sf::Texture* texture = TextureLoader::LoadTexture("smiley.png");
	this->shape->setTexture(texture);
}

PhysicsBody::~PhysicsBody()
{
	if (this->shapeType == ShapeType::Box) {
		delete static_cast<sf::RectangleShape*>(shape);
	}
	else if (this->shapeType == ShapeType::Circle) {
		delete static_cast<sf::CircleShape*>(shape);
	}
	//delete this->shape;

	this->shape = nullptr;
	std::cout << "DESTRUCTION\n";
}


void PhysicsBody::step(float time, sf::Vector2f gravity, unsigned int iterations)
{
	// force = mass * acc;
	// acc = force / mass
	if (this->isStatic)
		return;

	time /= static_cast<float>(iterations);
	//sf::Vector2f acceleration = this->force / this->mass;
	//this->linearVelocity += acceleration * time;

	this->linearVelocity += gravity * time;
	this->shape->move(this->linearVelocity * time);
	this->rotate(this->angularVelocity * time);

	this->force = { 0.f,0.f };
	this->transformUpdateRequired = true;
	this->aabbUpdateRequired = true;
}

void PhysicsBody::addForce(sf::Vector2f amount)
{
	force = amount;
}



float PhysicsBody::calculateRotationalInertia()
{
	// https://en.wikipedia.org/wiki/List_of_moments_of_inertia
	if (this->shapeType == ShapeType::Circle) {
		return (1.f / 2.f)* mass * radius* radius;
	}
	else if (this->shapeType == ShapeType::Box) {
		return (1.f / 12.f) * this->mass * (this->width * this->width + this->height * this->height);
	}
	std::cout << "Shapetype != circle or box\n";
	return 0.f;
}

void PhysicsBody::setColor(sf::Color color)
{
	this->shape->setFillColor(color);
}

void PhysicsBody::draw(sf::RenderWindow& window)
{
	window.draw(*this->shape);
}

void PhysicsBody::move(sf::Vector2f amount)
{
	this->shape->move(amount);
	this->transformUpdateRequired = true;
	this->aabbUpdateRequired = true;
}

void PhysicsBody::moveTo(sf::Vector2f position)
{
	this->shape->setPosition(position);
	this->transformUpdateRequired = true;
	this->aabbUpdateRequired = true;
}

void PhysicsBody::rotate(float amount)
{
	this->shape->setRotation(this->shape->getRotation() + sf::radians(amount));
	this->transformUpdateRequired = true;
	this->aabbUpdateRequired = true;
}

void PhysicsBody::rotateTo(float angle)
{
	this->shape->setRotation(sf::radians(angle));
	this->transformUpdateRequired = true;
	this->aabbUpdateRequired = true;
}

sf::Vector2f PhysicsBody::getPosition() const
{
	return shape->getPosition();
	//return shape->getTransform().transformPoint(shape->getGeometricCenter());
}

float PhysicsBody::getRadius() const
{
	return radius;
}


const std::vector<sf::Vector2f>& PhysicsBody::getTransformedVertices()
{
	if (this->transformUpdateRequired)
		this->transformVertices();
	return this->transformedVertices;
}

ShapeType PhysicsBody::getShapeType() const
{
	return this->shapeType;
}

float PhysicsBody::getMass()
{
	return this->mass;
}

float PhysicsBody::getInverseMass()
{
	return this->inverseMass;
}

float PhysicsBody::getRestitution()
{
	return this->restitution;
}

bool PhysicsBody::getIsStatic()
{
	return this->isStatic;
}

float PhysicsBody::getAngularVelocity()
{
	return this->angularVelocity;
}

void PhysicsBody::setAngularVelocity(float value)
{
	angularVelocity = value;
}

float PhysicsBody::getInverseInertia()
{
	return this->inverseIneratia;
}

AABB PhysicsBody::getAABB()
{

	if (this->aabbUpdateRequired) {

		sf::Vector2f position = this->shape->getPosition();
		float minX = INFINITY;
		float minY = INFINITY;
		float maxX = -INFINITY;
		float maxY = -INFINITY;
		if (this->shapeType == ShapeType::Box) {
			if (this->transformUpdateRequired)
				this->getTransformedVertices();
			for (sf::Vector2f v : transformedVertices) {
				if (v.x < minX) { minX = v.x; }
				if (v.x > maxX) { maxX = v.x; }
				if (v.y < minY) { minY = v.y; }
				if (v.y > maxY) { maxY = v.y; }
			}
		}
		else if (this->shapeType == ShapeType::Circle) {
			minX = position.x - this->radius;
			minY = position.y - this->radius;
			maxX = position.x + this->radius;
			maxY = position.y + this->radius;
		}
		this->aabb = AABB({ minX,minY }, { maxX,maxY });
		aabbUpdateRequired = false;
	}
	return this->aabb;
}




void PhysicsBody::transformVertices()
{
	if (shapeType == ShapeType::Circle) {
		std::cout << "Why transformvertices on a circle\n";
		return;
	}
	for (unsigned int i{ 0 }; i < transformedVertices.size(); i++) {
		this->transformedVertices[i] = this->shape->getTransform().transformPoint(this->shape->getPoint(i));
	}
	//std::cout << "we transformed the vertices\n";
	this->transformUpdateRequired = false;
}

