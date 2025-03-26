#include "Catapult.h"
#include "../loaders/TextureLoader.h"

Catapult::Catapult(sf::Vector2f position)
{
    if (sf::Texture* t = TextureLoader::LoadTexture("catapult.png")) {
        shape.setTexture(t);
        shape.setSize(sf::Vector2f(t->getSize()) * 0.02f);
    }
    else {
        std::cout << " Failed to load catapult\n";
    }
    shape.setScale({ 1.f,-1.f });

    // setting origin bottom middle
    shape.setOrigin({ shape.getGlobalBounds().size.x * 0.5f ,shape.getGlobalBounds().size.y });
    shape.setPosition(position);

    // setting loadposition middle top 
    loadPosition = { position.x, position.y + shape.getGlobalBounds().size.y * 0.8f };


    std::cout << "CatapultSize:" << shape.getGlobalBounds().size.x << ',' << shape.getGlobalBounds().size.y << '\n';

    this->maxPullLength = 5.f;
}

Catapult::~Catapult()
{
    if (launchingObject) {
        delete  launchingObject;
        launchingObject = nullptr;
    }
}


void Catapult::load(PhysicsBody* body)
{
    this->launchingObject = body;
    this->launchingObject->moveTo(loadPosition);
}

bool Catapult::isLoaded()
{
    return launchingObject != nullptr;
}

void Catapult::update(sf::Vector2f mousePosition)
{
    if (launchingObject) {
        this->launchingObject->moveTo(mousePosition);
        sf::Vector2f ab = mousePosition - this->loadPosition;
        if ((ab).length() < this->maxPullLength) {
            launchingObject->moveTo(mousePosition);
            return;
        }
        if (ab == sf::Vector2f(0.f, 0.f)) {
            return;
        }
        sf::Vector2f pullDirection = ab.normalized();

        pullDirection *= std::clamp(ab.length(), 0.f, this->maxPullLength);

        launchingObject->moveTo(this->loadPosition + pullDirection);
    }
}

bool Catapult::contains(sf::Vector2f mousePosition)
{
    if (launchingObject == nullptr)
        return false;

    AABB aabb = launchingObject->getAABB();

    if (mousePosition.x < aabb.min.x || mousePosition.y < aabb.min.y ||
        mousePosition.x > aabb.max.x || mousePosition.y > aabb.max.y) {
        return false;
    }
    return true;
}

PhysicsBody* Catapult::release()
{
    if (launchingObject == nullptr)
        return nullptr;
    launchingObject->addLinearVelocity((loadPosition - launchingObject->getPosition()) * 15.f);
    
    
    PhysicsBody* temp = launchingObject;
    launchingObject = nullptr;
    return temp;
}

void Catapult::draw(sf::RenderWindow& window)
{
    window.draw(shape);
    if (launchingObject)
        launchingObject->draw(window);
}
