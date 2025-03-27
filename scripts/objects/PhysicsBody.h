#pragma once
#include "../collision/AABB.h"
#include <SFML/graphics.hpp>
#include "Material.h"

enum class ShapeType {
	Circle,
	Box,
	Triangle
};




// initiate as pointer
class PhysicsBody
{

public:
	// General constructor
	PhysicsBody(ShapeType shape, float radius, float width, float height, float density, bool isStatic, float restitution, float dynamicFriction, float staticFriction, float health = 0.f);

	// fast constructor
	PhysicsBody(ShapeType type, float radius, float width, float height, bool isStatic,MaterialType mType, float health = 0.f);

	PhysicsBody(PhysicsBody&) = delete;
	~PhysicsBody(); 


	void damage(float amount);
	void setOpacity(int opacity);

	void draw(sf::RenderWindow& window);
	void step(float time,sf::Vector2f gravity);

	void move(sf::Vector2f amount);
	void moveTo(sf::Vector2f position);
	 
	void rotate(float amount);
	void rotateTo(float angle);

	void addAngularVelocity(float amount);
	void addLinearVelocity(sf::Vector2f amount);


	void setTexture(sf::Texture* texture, int animations = 1);

	// getters
	const std::vector<sf::Vector2f>& getTransformedVertices();
	sf::Vector2f getPosition() const;
	sf::Vector2f getLinearVelocity()const;
	float		getAngularVelocity()const;
	float		getRadius()const;
	float		getMass()const;
	float		getInverseMass()const;
	float		getRestitution()const;
	float		getInverseInertia()const;
	float		getStaticFriction()const;
	float		getDynamicFriction()const;
	float		getHealth()const;
	bool		getIsDestroyed()const;
	bool		getIsStatic()const;
	AABB		getAABB();
	ShapeType	getShapeType() const;
	MaterialType getMaterialType()const;


private:
	// CIRCLE CONSTRUCTOR
	void circleConstructor(float radius);
	// CUBE CONSTRUCTOR
	void boxConstructor(float width, float height);
	// TRIANGLE CONSTRUCTOR
	void triangleConstructor(float width, float height);

	void transformVertices();
	float calculateRotationalInertia();

	float maxHealth;
	float health;

	bool aabbUpdateRequired{ true };
	AABB aabb;
	bool transformUpdateRequired{ true };
	std::vector<sf::Vector2f> transformedVertices;


	sf::Vector2f linearVelocity{};
	float angularVelocity{};

	// texture stuff
	sf::Vector2i textureSize;
	int animationAmount{1};

	sf::Shape* shape;
	// physics data
	ShapeType shapeType{};
	float density{};
	float mass{};
	float inverseMass{};
	float restitution{};
	float area{};
	float ineratia{};
	float inverseIneratia{};
	float radius{};
	float width{};
	float height{};
	float staticFriction{};
	float dynamicFriction{};

	bool isStatic{ false };
	bool isDestroyed{ false };


	MaterialType materialType{ MaterialType::None };
};

