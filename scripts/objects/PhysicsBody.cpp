#include "PhysicsBody.h"
#include "../PhysicsMath.h"
#include <iostream>

PhysicsBody::PhysicsBody(ShapeType shape, float radius, float width, float height, bool isStatic, MaterialType mType, float health)
	: PhysicsBody(shape, radius, width, height, 
		Materials::getMaterial(mType).density, 
		isStatic, 
		Materials::getMaterial(mType).restitution, 
		Materials::getMaterial(mType).dynamicFriction, 
		Materials::getMaterial(mType).staticFriction, health)
{
	this->materialType = mType;
}

PhysicsBody::PhysicsBody(ShapeType type, float radius, float width, float height, float density, bool isStatic, float restitution, float dynamicFriction, float staticFriction, float health)
{
	this->health = health;
	this->maxHealth = health;

	this->radius = radius;
	this->width = width;
	this->height = height;
	this->shapeType = type;

	switch (this->shapeType) {
	case ShapeType::Box: boxConstructor(width, height); break;
	case ShapeType::Circle: circleConstructor(radius); break;
	case ShapeType::Triangle: triangleConstructor(width, height); break;
	}


	this->dynamicFriction = dynamicFriction;
	this->staticFriction = staticFriction;
	if (dynamicFriction > staticFriction) {
		std::cout << "ERROR DYNAMIC FRICTION IS MORE THEN STATIC FRICTION\n";
		dynamicFriction = staticFriction;
	}

	this->density = density;
	this->restitution = std::clamp(restitution, 0.f, 1.f);
	this->isStatic = isStatic;

	sf::Vector2f center = this->shape->getGeometricCenter();
	this->shape->setOrigin(center);


	this->mass = this->area * this->density;
	this->ineratia = this->calculateRotationalInertia();
	this->inverseMass = 1.f / this->mass;
	this->inverseIneratia = 1.f / this->ineratia;

	if(this->isStatic) {
		this->shape->setFillColor(sf::Color(71, 110, 45, 255));
	}

	std::cout << " Ineratia:" << this->ineratia << '\n';
	std::cout << " Mass:" << this->mass << '\n';

	this->aabbUpdateRequired = true;

	// if health was not set
	if (health == 0.f){
		this->health = 20 + mass * 50.f;
		this->maxHealth = health;
	}
}


// box creation
void PhysicsBody::boxConstructor(float width, float height)
{
	this->area = width * height;
	this->shape = new sf::RectangleShape({ width,height });

	// loading amount of vertices on creation.
	this->transformedVertices.clear();
	for (unsigned int i{ 0 }; i < this->shape->getPointCount(); i++) {
		this->transformedVertices.push_back(this->shape->getTransform().transformPoint(this->shape->getPoint(i)));
	}
	this->transformUpdateRequired = false;
}

// circle creation
void PhysicsBody::circleConstructor(float radius)
{
	this->area = radius * radius * M_PI;
	this->shape = new sf::CircleShape(radius);
}

// triangle creation
void PhysicsBody::triangleConstructor(float width, float height)
{
	this->area = width * height *  0.5f;

	this->shape = new sf::ConvexShape(3);
	static_cast<sf::ConvexShape*>(this->shape)->setPoint(0, { -width * 0.5f,-height * 0.5f });
	static_cast<sf::ConvexShape*>(this->shape)->setPoint(1, {  width * 0.5f,-height * 0.5f });
	static_cast<sf::ConvexShape*>(this->shape)->setPoint(2, { 0.f,			 height * 0.5f });


	// loading amount of vertices on creation.
	this->transformedVertices.clear();
	for (unsigned int i{ 0 }; i < this->shape->getPointCount(); i++) {
		this->transformedVertices.push_back(this->shape->getTransform().transformPoint(this->shape->getPoint(i)));
	}
	this->transformUpdateRequired = false;
}

PhysicsBody::~PhysicsBody()
{
	if (this->shapeType == ShapeType::Box) {
		delete static_cast<sf::RectangleShape*>(this->shape);
	}
	else if (this->shapeType == ShapeType::Circle) {
		delete static_cast<sf::CircleShape*>(this->shape);
	}
	else if (this->shapeType == ShapeType::Triangle) {
		delete static_cast<sf::ConvexShape*>(this->shape);
	}
	//delete this->shape;

	this->shape = nullptr;
	std::cout << "DESTRUCTION\n";
}




void PhysicsBody::step(float time, sf::Vector2f gravity)
{
	// force = mass * acc;
	// acc = force / mass
	//sf::Vector2f acceleration = this->force / this->mass;
	//this->linearVelocity += acceleration * time;

	if (this->isStatic)
		return;

	this->linearVelocity += gravity * mass * time;
	this->shape->move(this->linearVelocity * time);
	this->rotate(this->angularVelocity *  time);

	//this->force = { 0.f,0.f };
	this->transformUpdateRequired = true;
	this->aabbUpdateRequired = true;
}

void PhysicsBody::draw(sf::RenderWindow& window)
{
	if (this->shapeType == ShapeType::Triangle) {
		window.draw(static_cast<sf::ConvexShape&>(*this->shape));
	}
	else {
		window.draw(*this->shape);
	}
}

void PhysicsBody::damage(float amount)
{
	this->health -= amount;
	float percentHealth = health / maxHealth;

	float animationPerc = 1.f / animationAmount;
	for (int size{ 1}; size < animationAmount; size++) {
		if (percentHealth < size * animationPerc) {
			this->shape->setTextureRect(sf::Rect(sf::Vector2i(textureSize.x * (animationAmount - size), textureSize.y), sf::Vector2i(textureSize.x, -textureSize.y)));
			break;
		}
	}
	if (health < 0.f) {
		isDestroyed = true;
	}
}

void PhysicsBody::setOpacity(int opacity)
{

	this->shape->setFillColor(sf::Color(255, 255, 255, opacity));
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
	else if (this->shapeType == ShapeType::Triangle) {
		return (1.f / 18.f) * this->mass * (this->width * this->width + this->height * this->height);

	}
	std::cout << "Shapetype != circle or box\n";
	return 0.f;
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
	this->shape->rotate(sf::radians(amount));
	this->transformUpdateRequired = true;
	this->aabbUpdateRequired = true;
}

void PhysicsBody::rotateTo(float angle)
{
	this->shape->setRotation(sf::radians(angle));
	this->transformUpdateRequired = true;
	this->aabbUpdateRequired = true;
}

void PhysicsBody::addLinearVelocity(sf::Vector2f amount)
{
	this->linearVelocity += amount;
}

void PhysicsBody::addAngularVelocity(float value)
{
	this->angularVelocity += value;
}


void PhysicsBody::setTexture(sf::Texture* texture, int animations)
{
	this->shape->setFillColor(sf::Color(255, 255, 255, 255));
	if (texture == nullptr)
		return;
	this->shape->setTexture(texture);
	this->animationAmount = animations;
	this->textureSize = { static_cast<int>(texture->getSize().x / animations),static_cast<int>(texture->getSize().y)};
	 
	this->shape->setTextureRect(sf::Rect(sf::Vector2i( 0,textureSize.y ), sf::Vector2i(textureSize.x, -textureSize.y)));
	
}

const std::vector<sf::Vector2f>& PhysicsBody::getTransformedVertices()
{
	if (this->transformUpdateRequired)
		this->transformVertices();
	return this->transformedVertices;
}
bool PhysicsBody::getIsDestroyed() const
{
	return this->isDestroyed;
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
ShapeType PhysicsBody::getShapeType() const
{
	return this->shapeType;
}
MaterialType PhysicsBody::getMaterialType() const
{
	return this->materialType;
}
float PhysicsBody::getMass() const
{
	return this->mass;
}
float PhysicsBody::getInverseMass() const
{
	return this->inverseMass;
}
float PhysicsBody::getRestitution() const
{
	return this->restitution;
}
bool PhysicsBody::getIsStatic() const
{
	return this->isStatic;
}
float PhysicsBody::getAngularVelocity() const
{
	return this->angularVelocity;
}

sf::Vector2f PhysicsBody::getLinearVelocity() const
{
	return this->linearVelocity;
}

float PhysicsBody::getInverseInertia() const
{
	return this->inverseIneratia;
}
float PhysicsBody::getStaticFriction() const
{
	return this->staticFriction;
}
float PhysicsBody::getDynamicFriction() const
{
	return this->dynamicFriction;
}
float PhysicsBody::getHealth() const
{
	return this->health;
}
AABB PhysicsBody::getAABB()
{

	if (this->aabbUpdateRequired) {

		sf::Vector2f position = this->shape->getPosition();
		float minX = INFINITY;
		float minY = INFINITY;
		float maxX = -INFINITY;
		float maxY = -INFINITY;
		if (this->shapeType == ShapeType::Box || this->shapeType == ShapeType::Triangle) {
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

