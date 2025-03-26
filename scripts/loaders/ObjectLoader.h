#pragma once
#include "TextureLoader.h"
#include "../PhysicsWorld.h"
#include "../PhysicsMath.h"
#include "../objects/PhysicsBody.h"


enum class ObjectType {
    woodBox,
    woodStick,
    woodThickStick,
    stoneBox,
    stoneStick,
    stoneThickStick,
    stonePillar,
    iceBox,
    iceStick,
    enemy,
};

struct ObjectInfo {
    sf::Vector2f position;
    float rotationRadii;
    ObjectType type;
    bool isStatic{ false };
};



namespace ObjectLoader
{

    void loadObject(ObjectInfo info, PhysicsWorld& world);


    PhysicsBody* createEnemy();

    PhysicsBody* createWoodBox(bool isStatic);
    PhysicsBody* createIceBox(bool isStatic);

    PhysicsBody* createTriangle(bool isStatic);
    PhysicsBody* createWoodStick(bool isStatic);
    PhysicsBody* createWoodThickStick(bool isStatic);

    PhysicsBody* createStoneStick(bool isStatic);
    PhysicsBody* createStoneThickStick(bool isStatic);

    PhysicsBody* createStonePillar(bool isStatic);


};

