#include "ObjectLoader.h"


namespace ObjectLoader {

    void loadObject(ObjectInfo info, PhysicsWorld& world)
    {
        PhysicsBody* body{nullptr};

        
        switch (info.type) {
        case ObjectType::woodBox:       body = createWoodBox(info.isStatic); break;
        case ObjectType::woodStick:     body = createWoodStick(info.isStatic); break;
        case ObjectType::woodThickStick:body = createWoodThickStick(info.isStatic); break;

        case ObjectType::stoneBox:      body = createWoodBox(info.isStatic); break;
        case ObjectType::stoneStick:    body = createStoneStick(info.isStatic); break;
        case ObjectType::stoneThickStick:body =createStoneThickStick(info.isStatic); break;
        case ObjectType::stonePillar:   body = createStonePillar(info.isStatic); break;

        case ObjectType::iceBox:        body = createIceBox(info.isStatic);  break;
        case ObjectType::iceStick:      body = createWoodStick(info.isStatic); break;
        case ObjectType::enemy:         body = createEnemy();      break;
        }
        if (body == nullptr) {
            std::cout << "BODY NOT INITIALISED IN OBJECTLOADER\n";
            return;
        }
        body->moveTo(info.position);
        body->rotateTo(info.rotationRadii);

        if (info.type == ObjectType::enemy)
            world.addEnemy(body);
        else
            world.addObject(body);

    }


    PhysicsBody* createEnemy()
    {
        std::cout << "Create enemy\n";
        PhysicsBody* body = new PhysicsBody(ShapeType::Circle, 1.5f, 0.f, 0.f, false, MaterialType::Enemy, 15.f);
        body->setTexture(TextureLoader::LoadTexture("enemy.png"), 3);
        return body;
    }


    PhysicsBody* createWoodBox(bool isStatic) {
        std::cout << "Create Box\n";
        PhysicsBody* body = new PhysicsBody(ShapeType::Box, 0.f, 4.f, 4.f, isStatic, MaterialType::Wood);
        body->setTexture(TextureLoader::LoadTexture("4x4woodblock.png"), 4);
        return body;
    }
    PhysicsBody* createWoodStick(bool isStatic) {
        std::cout << "Create 10x1 woodbar\n";
        PhysicsBody* body = new PhysicsBody(ShapeType::Box, 0.f, 10.0f, 1.f, isStatic, MaterialType::Wood);
        body->setTexture(TextureLoader::LoadTexture("10x1woodbar.png"), 4);
        return body;
    }

    PhysicsBody* createWoodThickStick(bool isStatic) {
        std::cout << "Create 10x1 woodbar\n";
        PhysicsBody* body = new PhysicsBody(ShapeType::Box, 0.f, 8.0f, 2.f, isStatic, MaterialType::Wood);
        body->setTexture(TextureLoader::LoadTexture("10x1woodbar.png"), 4);
        return body;
    }


    PhysicsBody* createStoneStick(bool isStatic) {
        std::cout << "Create 10x1 stoneBar\n";
        PhysicsBody* body = new PhysicsBody(ShapeType::Box, 0.f, 10.0f, 1.f, isStatic, MaterialType::Stone);
        body->setTexture(TextureLoader::LoadTexture("10x1stonebar.png"), 4);
        return body;
    }
    PhysicsBody* createStoneThickStick(bool isStatic) {
        std::cout << "Create 10x3 stoneBar\n";
        PhysicsBody* body = new PhysicsBody(ShapeType::Box, 0.f, 8.0f, 2.f, isStatic, MaterialType::Stone);
        body->setTexture(TextureLoader::LoadTexture("10x1stonebar.png"), 4);
        return body;
    }
    PhysicsBody* createStonePillar(bool isStatic)
    {
        std::cout << "Create stonepillar\n";
        PhysicsBody* body = new PhysicsBody(ShapeType::Box, 0.f, 16.0f, 4.f, isStatic, MaterialType::Stone);
        body->setTexture(TextureLoader::LoadTexture("10x1stonebar.png"), 4);
        return body;
    }

    // give ice certain health values
    PhysicsBody* createIceBox(bool isStatic) {
        std::cout << "Create Box\n";
        PhysicsBody* body = new PhysicsBody(ShapeType::Box, 0.f, 4.f, 4.f, isStatic, MaterialType::Ice, 10.f);
        body->setTexture(TextureLoader::LoadTexture("4x4iceblock.png"), 4);
        return body;
    }
    PhysicsBody* createTriangle(bool isStatic) {
        std::cout << "Create Triangle\n";
        PhysicsBody* body = new PhysicsBody(ShapeType::Triangle, 0.f, 4.f, 4.f, isStatic, MaterialType::Wood);
        body->setTexture(TextureLoader::LoadTexture("4x4triangle.png"), 1);
        return body;
    }

}