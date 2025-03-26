#pragma once
#include <map>
#include "collision/Collision.h"
#include "objects/PhysicsBody.h"
#include "objects/Catapult.h"
#include "ui/Button.h"
#include "loaders/AudioLoader.h"

class PhysicsWorld
{
public:

	static const unsigned int minIterations;
	static const unsigned int maxIterations;


	PhysicsWorld(sf::Vector2f worldsize, sf::Vector2f screenSize);
	~PhysicsWorld();

	void addObject(PhysicsBody* body);
	bool removeBody(PhysicsBody* body);
	void addEnemy(PhysicsBody* body);

	// event handling, mouseclick button click also used to quit the level
	bool exitLevel(const std::optional<sf::Event> event, sf::Vector2f mouseLocation, sf::Vector2f mouseLocationInUI);
	// update
	bool update(float delta, sf::Vector2f mouseLocation);
	// physics step
	void step(float delta);
	void draw(sf::RenderWindow& window);
	void drawUI(sf::RenderWindow& window);
	

	void setLevelName(const std::string& name);

	void setPlayerLives(int lives);;

private:

	void reload();
	void resetLevel();

	void separateBodies(PhysicsBody* bodyA, PhysicsBody* bodyB, sf::Vector2f mtv);
	void broadPhase();
	void broadPhaseCheckSelf(const std::vector<PhysicsBody*>& list);
	void broadPhaseCheckVersus(const std::vector<PhysicsBody*>& listA, const std::vector<PhysicsBody*>& listB);

	void stepBodies(float time);

	void narrowPhase(float delta);


	sf::Vector2f resolveCollisionWithRotationAndFriction(const Collision::Manifold& contact, float delta);

	void checkOutOfBounds();

	void dampenVelocity(PhysicsBody* body);

	bool wonGame{ false };

	Button mainMenuButton;
	
	int playerLives{3};
	sf::RectangleShape playerLivesImage;
	sf::Text* playerLivesText{};
	sf::Text* scoreText{};
	sf::Text* levelNameText{};
	sf::Text* youWonText{};
	int score{};

	sf::Sound catapultPullSound;
	sf::Sound woodBreakingSound;
	sf::Sound stoneBreakingSound;
	sf::Sound launchingSound;
	sf::Sound collideSound;
	sf::Sound enemySound;
	sf::Sound victorySound;

	// damage needed to apply
	const float damageThreshold = 5.f;

	bool holdingCatapult{ false };

	Catapult catapult;
	sf::Vector2f worldSize;

	sf::RectangleShape background;

	// trail variables 
	float timeBetweenTrails;
	float timeTillNextTrail;
	PhysicsBody* playerNotHitAnytingYet;
	std::vector<sf::CircleShape> trailList;

	sf::Vector2f gravity;
	std::vector<PhysicsBody*> objectList;
	std::vector<PhysicsBody*> playerList;
	std::vector<PhysicsBody*> enemyList;

	// time till destruction
	// destroyed bodies which are slowly fading out
	std::map<PhysicsBody*,float> destroyedBodyList;

	std::vector<std::tuple<PhysicsBody*, PhysicsBody*>> contactPairs;


	// debugging
	sf::VertexArray contactPointsList;
	sf::VertexArray contactDirectionList;

};

