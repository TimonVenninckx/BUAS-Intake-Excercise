#include "PhysicsMath.h"

float PhysicsMath::Distance(sf::Vector2f a, sf::Vector2f b)
{
    sf::Vector2f distance = a - b;
    return distance.length();
}
