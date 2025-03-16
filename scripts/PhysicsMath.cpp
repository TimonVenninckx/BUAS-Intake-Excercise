#include "PhysicsMath.h"

constexpr float VerySmallAmount = 0.0005f;

namespace PhysicsMath {

    // equal to 1/2 of milimeter

    float distance(sf::Vector2f a, sf::Vector2f b)
    {
        sf::Vector2f distance = a - b;
        return distance.length();
    }

    float distanceSquared(sf::Vector2f a, sf::Vector2f b)
    {
        sf::Vector2f d = a - b;
        return d.x * d.x + d.y * d.y;
    }

    bool nearlyEqual(float a, float b) {
        return std::abs(a - b) < VerySmallAmount;
    }

    bool nearlyEqual(sf::Vector2f a, sf::Vector2f b)
    {
        return distanceSquared(a,b) < VerySmallAmount * VerySmallAmount;
    }


}