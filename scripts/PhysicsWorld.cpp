#include "PhysicsWorld.h"
#include <iostream>
#include "PhysicsMath.h"
#include "loaders/TextureLoader.h"
#include "loaders/FontLoader.h"
#include "loaders/LevelLoader.h"


PhysicsWorld::PhysicsWorld(sf::Vector2f worldSize, sf::Vector2f screenSize)
    : catapult{ sf::Vector2f(worldSize.x * -0.4f,5.f) }
    , mainMenuButton{ "MainMenu",{160.f,50.f}, {screenSize.x - 105.f, 50.f} }
    , catapultPullSound{ *AudioLoader::getAudio("pull string.mp3") }
    , woodBreakingSound{ *AudioLoader::getAudio("test.mp3") }
    , stoneBreakingSound{ *AudioLoader::getAudio("test.mp3") }
    , iceBreakingSound{ *AudioLoader::getAudio("icebreak.mp3") }
    , launchingSound{ *AudioLoader::getAudio("launchsound.mp3") }
    , collideSound{ *AudioLoader::getAudio("collide.mp3") }
    , enemySound{ *AudioLoader::getAudio("hurt.mp3") }
    , victorySound{ *AudioLoader::getAudio("victory.mp3") }
    , gameOverMenu{ screenSize }
{
    
    this->worldSize = worldSize;
    this->gravity = sf::Vector2f(0.f, -9.81f);


    // first trail point should be delayed
    this->timeBetweenTrails = .07f;
    this->timeTillNextTrail = timeBetweenTrails * 2.f;
    this->playerNotHitAnytingYet = nullptr;

    // SETUP CATAPULT
    reload();

    // SETUP BACKGROUND
    sf::Texture* backgroundTexture = TextureLoader::LoadTexture("pixels-evening-grass-sun.jpg");
    if (backgroundTexture) {
        this->background = sf::RectangleShape(worldSize);
        this->background.setPosition({ -worldSize.x / 2.f, 0.f });
        sf::Vector2i textureSize = { 0,static_cast<int>(backgroundTexture->getSize().y) };
        this->background.setTexture(backgroundTexture);
        this->background.setTextureRect(sf::IntRect{ {0,this->background.getTextureRect().size.y},{this->background.getTextureRect().size.x, -this->background.getTextureRect().size.y } });
    }

    this->scoreText = new sf::Text(*FontLoader::getFont("angrybirds.ttf"), "Score: ", 50);

    this->playerLivesImage = sf::RectangleShape({ 30.f,30.f });
    this->playerLivesImage.setPosition({ 5.f,55.f });
    this->playerLivesImage.setTexture(TextureLoader::LoadTexture("ball1.png"));
    this->playerLivesText = new sf::Text(*FontLoader::getFont("angrybirds.ttf"), "3 ", 30);
    this->playerLivesText->setPosition({ 40.f, 50.f });
    
    this->levelNameText = new sf::Text(*FontLoader::getFont("angrybirds.ttf"), "LevelName", 50);
    this->levelNameText->setPosition({ screenSize.x / 2.f, 0.f });


    this->woodBreakingSound.setVolume(AudioLoader::Volume);
    this->stoneBreakingSound.setVolume(AudioLoader::Volume);
    this->iceBreakingSound.setVolume(AudioLoader::Volume);
    this->catapultPullSound.setVolume(AudioLoader::Volume);
    this->launchingSound.setVolume(AudioLoader::Volume);
    this->collideSound.setVolume(AudioLoader::Volume * 0.1f);
    this->enemySound.setVolume(AudioLoader::Volume);
    this->victorySound.setVolume(AudioLoader::Volume);
}

void PhysicsWorld::reload()
{
    if (catapult.isLoaded() || playerLives < 1)
        return;


    PhysicsBody* launchedBody = new PhysicsBody(ShapeType::Circle, 1.5f, 0.f, 0.f, false, MaterialType::Player, 300.f);
    launchedBody->setTexture(TextureLoader::LoadTexture("ball1.png"));
    this->catapult.load(launchedBody);
    this->timeTillNextTrail = timeBetweenTrails * 2.f;
}

void PhysicsWorld::resetLevel()
{
    for (unsigned int i{ 0 }; i < objectList.size(); i++) {
        delete objectList[i];
    }
    objectList.clear();
    for (unsigned int i{ 0 }; i < enemyList.size(); i++) {
        delete enemyList[i];
    }
    enemyList.clear();
    for (unsigned int i{ 0 }; i < playerList.size(); i++) {
        delete playerList[i];
    }
    playerList.clear();
    score = 0;

    trailList.clear();
    timeTillNextTrail = timeBetweenTrails * 2.f;
    this->playerNotHitAnytingYet = nullptr;
    gameOver = false;
}

PhysicsWorld::~PhysicsWorld()
{
    resetLevel();
    delete levelNameText;
    delete scoreText;

    // destroyed bodylist contains the same bodies so we don't need to clear that aswell
}


void PhysicsWorld::addObject(PhysicsBody* body)
{
    if (body)
	    this->objectList.push_back(body);
}
bool PhysicsWorld::removeBody(PhysicsBody* body)
{
    if (body == playerNotHitAnytingYet)
        playerNotHitAnytingYet = nullptr;

    bool removed{ false };
    auto result = std::find_if(this->objectList.begin(), this->objectList.end(),
        [body](PhysicsBody* c) { return c == body; });
    if (result != objectList.end()) {
        delete (*result);
        this->objectList.erase(result);
        removed = true;
    }
    // check enemy removing
    result = std::find_if(this->enemyList.begin(), this->enemyList.end(),
        [body](PhysicsBody* c) { return c == body; });
    if (result != enemyList.end()) {
        delete (*result);
        this->enemyList.erase(result);
        removed = true;
    }
    // check player removing
    result = std::find_if(this->playerList.begin(), this->playerList.end(),
        [body](PhysicsBody* c) { return c == body; });
    if (result != playerList.end()) {
        delete (*result);
        this->playerList.erase(result);
        removed = true;
    }

    // destroyed bodylist check
    auto dresult = this->destroyedBodyList.find(body);
    if (dresult != destroyedBodyList.end()) {
        destroyedBodyList.erase(dresult);
    }
    return removed;
}
void PhysicsWorld::addEnemy(PhysicsBody* body) {
    if (body)
        this->enemyList.push_back(body);
}

bool PhysicsWorld::exitLevel(const std::optional<sf::Event> event, sf::Vector2f mouseLocation,sf::Vector2f mouseLocationInUI)
{
    if (const sf::Event::KeyPressed* keypressed = event->getIf<sf::Event::KeyPressed>()) { // keypressed
            // reload
        if (keypressed->scancode == sf::Keyboard::Scan::Escape) {
            this->resetLevel();
            return true;
        }
    }
    else if (const sf::Event::MouseButtonPressed* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) { // mouse button pressed
       
        if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
           if (gameOver) {
               if (gameOverMenu.mainMenuButtonClicked(mouseLocationInUI)) {
                   resetLevel();
                   return true;
               }
               else if (gameOverMenu.nextRetryButtonClicked(mouseLocationInUI)) {
                   if (enemyList.empty()) {
                       resetLevel();
                       LevelLoader::LoadLevel(*this, LevelLoader::Levels[this->levelId % LevelLoader::Levels.size()], worldSize);
                   }
                   else {                                       
                       resetLevel();                                    // level id should always be > 0
                       LevelLoader::LoadLevel(*this, LevelLoader::Levels[(this->levelId - 1) % LevelLoader::Levels.size()], worldSize);
                   }
               }
           }
           else if (catapult.contains(mouseLocation)) {
               holdingCatapult = true;
               catapultPullSound.play();
           }
           else if (mainMenuButton.contains(mouseLocationInUI)) {
               resetLevel();
               return true;
           }
        }
    }
    else if (const sf::Event::MouseButtonReleased* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>()) { // mouse button pressed
        if (mouseButtonReleased->button == sf::Mouse::Button::Left) {
            if (catapult.isLoaded() && holdingCatapult) {
                setPlayerLives(playerLives - 1);
                playerNotHitAnytingYet = catapult.release();
                launchingSound.play();
                playerList.push_back(playerNotHitAnytingYet);
                holdingCatapult = false;
                trailList.clear();
            }
        }
        if (mouseButtonReleased->button == sf::Mouse::Button::Right) {
        }
    }
    return false;
}

bool PhysicsWorld::activePlayersNotMoving() {
    for (unsigned int i{ 0 }; i < playerList.size(); i++) {
        if (playerList[i]->getLinearVelocity().length() > 1.f)
            return false;
    }
    return true;
}

void PhysicsWorld::update(float delta, sf::Vector2f mouseLocation)
{
    if (playerNotHitAnytingYet) {
        timeTillNextTrail -= delta;
        if (timeTillNextTrail < 0.f) {
            sf::CircleShape trail(.5f);
            trail.setOrigin(trail.getGlobalBounds().getCenter());
            trail.setPosition(playerNotHitAnytingYet->getPosition());
            trailList.push_back(trail);
            timeTillNextTrail += timeBetweenTrails;
        }
    }
    else {
        reload();
    }

    if (!gameOver) {
        if (enemyList.empty()) {
            gameOver = true;
            victorySound.play();
            gameOverMenu.setValues(true, this->score);
        }
        else if (playerLives == 0) {
            if (playerList.empty() || activePlayersNotMoving()) {
                gameOver = true;
                gameOverMenu.setValues(false, this->score);
            }
        }
    }
    else
        gameOverMenu.update(delta);

    scoreText->setString("Score: " + std::to_string(score));

    checkOutOfBounds();

    if (holdingCatapult)
        catapult.update(mouseLocation);

}


void PhysicsWorld::step(float deltaTime)
{
    this->contactPairs.clear();
    this->stepBodies(deltaTime);
    this->broadPhase();
    this->narrowPhase(deltaTime);
}
void PhysicsWorld::draw(sf::RenderWindow& window) {

    window.draw(this->background);
    for (unsigned int i{ 0 }; i < objectList.size(); i++) {
        objectList[i]->draw(window);
    }
    for (unsigned int i{ 0 }; i < playerList.size(); i++) {
        playerList[i]->draw(window);
    }
    for (unsigned int i{ 0 }; i < enemyList.size(); i++) {
        enemyList[i]->draw(window);
    }
    catapult.draw(window);

    for (unsigned int i{ 0 }; i < trailList.size(); i++) {
        window.draw(trailList[i]);
    }
}

void PhysicsWorld::drawUI(sf::RenderWindow& window) {

    window.draw(*scoreText);
    window.draw(*levelNameText);
    window.draw(playerLivesImage);
    window.draw(*playerLivesText);

    if (gameOver)
        gameOverMenu.draw(window);
    else
        mainMenuButton.draw(window);
}

// checking objects against objects of their own type
void PhysicsWorld::broadPhaseCheckSelf(const std::vector<PhysicsBody*>& list){
    if (list.empty())
        return;
    for (unsigned int i{ 0 }; i < list.size() - 1; i++) {
        PhysicsBody* bodyA = list[i];
        if (bodyA->getIsDestroyed())
            continue;
        AABB bodyA_aabb = bodyA->getAABB();

        for (unsigned int j{ i + 1 }; j < list.size(); j++) {
            PhysicsBody* bodyB = list[j];
            if (bodyB->getIsDestroyed())
                continue;
            AABB bodyB_aabb = bodyB->getAABB();

            if (bodyA->getIsStatic() && bodyB->getIsStatic())
                continue;
            if (!Collision::IntersectAABB(bodyA_aabb, bodyB_aabb))
                continue;

            this->contactPairs.push_back({ bodyA,bodyB });
        }
    }
}
// checking objects against other types of objects
void PhysicsWorld::broadPhaseCheckVersus(const std::vector<PhysicsBody*>& listA, const std::vector<PhysicsBody*>& listB){
    for (unsigned int i{ 0 }; i < listA.size(); i++) {
        PhysicsBody* bodyA = listA[i];
        if (bodyA->getIsDestroyed())
            continue;
        AABB bodyA_aabb = bodyA->getAABB();

        for (unsigned int j{ 0 }; j < listB.size(); j++) {
            PhysicsBody* bodyB = listB[j];
            if (bodyB->getIsDestroyed())
                continue;
            AABB bodyB_aabb = bodyB->getAABB();

            if (bodyA->getIsStatic() && bodyB->getIsStatic())
                continue;
            if (!Collision::IntersectAABB(bodyA_aabb, bodyB_aabb))
                continue;

            this->contactPairs.push_back({ bodyA,bodyB });
        }
    }
}
void PhysicsWorld::broadPhase() {
    this->broadPhaseCheckSelf(objectList);
    this->broadPhaseCheckSelf(enemyList);
    this->broadPhaseCheckSelf(playerList);
    this->broadPhaseCheckVersus(enemyList, objectList);
    this->broadPhaseCheckVersus(playerList, objectList);
    this->broadPhaseCheckVersus(playerList, enemyList);
}
void PhysicsWorld::narrowPhase(float delta) {

    static std::list<PhysicsBody*> bodiesThatCollided;
    bodiesThatCollided.clear();
    for (unsigned int i{ 0 }; i < this->contactPairs.size(); i++) {

        std::tuple<PhysicsBody*, PhysicsBody*> pair = this->contactPairs[i];
        PhysicsBody* bodyA = std::get<0>(pair);
        PhysicsBody* bodyB = std::get<1>(pair);

        Collision::HitResult result{ Collision::collide(bodyA, bodyB) };

        if (result.collided) {
            if (playerNotHitAnytingYet) {
                if (playerNotHitAnytingYet == bodyA || playerNotHitAnytingYet == bodyB) {
                    playerNotHitAnytingYet = nullptr;
                }
            }
            bodiesThatCollided.push_back(bodyA);
            bodiesThatCollided.push_back(bodyB);

            separateBodies(bodyA, bodyB, result.normal * result.depth);

            Collision::ContactResult cr = Collision::findContactPoints(bodyA, bodyB);
            Collision::Manifold contact(bodyA, bodyB, result.normal, result.depth, cr.contact1, cr.contact2, cr.contactCount);
             sf::Vector2f impulseVector = this->resolveCollisionWithRotationAndFriction(contact,delta);

            // damage the objects
            if (!bodyA->getIsStatic() && !bodyB->getIsStatic()) {
                if (impulseVector.length() > this->damageThreshold) {
                    collideSound.play();
                    score += static_cast<int>(impulseVector.length());
                    std::cout << "Damage: " << impulseVector.length() << '\n';
                    bodyA->damage(impulseVector.length());
                    bodyB->damage(impulseVector.length());
                }
            }
        }
    }
    // Now we run through all collided bodies and dampen their angular/linear velocity
    while (!bodiesThatCollided.empty())
    {
        dampenVelocity(bodiesThatCollided.back());
        bodiesThatCollided.pop_back();
    }
}


void PhysicsWorld::stepBodies(float time) {

    constexpr float fadeOutTime = 0.2f;

    PhysicsBody* body;
    for (unsigned int i{ 0 }; i < this->objectList.size(); i++) {
        body = this->objectList[i];
        body->step(time, this->gravity);
        if (body->getIsDestroyed()) {
            // add to destroyed body list to keep rendering them for a bit after destruction
            auto result = this->destroyedBodyList.find(body);
            if (result == destroyedBodyList.end()) {
                destroyedBodyList.insert({ body, fadeOutTime });
                switch (body->getMaterialType()) {
                case MaterialType::Wood: woodBreakingSound.play(); break;
                case MaterialType::Stone: stoneBreakingSound.play(); break;
                case MaterialType::Ice: iceBreakingSound.play(); break;
                }
            }
        }
    }
    for (unsigned int i{ 0 }; i < this->enemyList.size(); i++) {
        body = this->enemyList[i];
        body->step(time, this->gravity);
        if (body->getIsDestroyed()) {
            // add to destroyed body list to keep rendering them for a bit after destruction
            auto result = this->destroyedBodyList.find(body);
            if (result == destroyedBodyList.end()) {
                // WE KILLED AN ENEMY INCREASE SCORE BY A LOT
                score += 5000;
                enemySound.play();
                destroyedBodyList.insert({ body, fadeOutTime });
            }
        }
    }
    for (unsigned int i{ 0 }; i < this->playerList.size(); i++) {
        body = this->playerList[i];
        body->step(time, this->gravity);
        if (body->getIsDestroyed()) {
            // add to destroyed body list to keep rendering them for a bit after destruction
            auto result = this->destroyedBodyList.find(body);
            if (result == destroyedBodyList.end()) {
                destroyedBodyList.insert({ body, fadeOutTime });
                score += 100;
            }
        }
    }
    // update timer and fade out destroyed bodies
    for (std::map<PhysicsBody*, float>::iterator it{ destroyedBodyList.begin() }, nextIt = it;  nextIt != destroyedBodyList.end();it = nextIt) {
        nextIt++;
        it->second -= time;
        if (it->second < 0.f) {
            PhysicsBody* body = it->first;
            this->removeBody(body);
        } 
        else {
            int opacity = std::clamp(static_cast<int>(it->second / fadeOutTime * 255.f ),0,255);
            it->first->setOpacity(opacity);
        }
    }
}


void PhysicsWorld::separateBodies(PhysicsBody* bodyA, PhysicsBody* bodyB, sf::Vector2f mtv) {
    // mtv = normal * depth
    if (bodyA->getIsStatic()) {
        bodyB->move(mtv);
        //if (mtv.x > 0.005f || mtv.y > 0.005f)
        //    std::cout << "How much we are displacing: " << mtv.x << ',' << mtv.y << '\n';
    }
    else if (bodyB->getIsStatic()) {
        bodyA->move(-mtv);
        //if (mtv.x > 0.005f || mtv.y > 0.005f)
        //    std::cout << "How much we are displacing: " << mtv.x << ',' << mtv.y << '\n';
    }
    else {
        bodyA->move(-mtv / 2.f);
        bodyB->move(mtv / 2.f);
    }
}

// use the vector for damaging objects
sf::Vector2f PhysicsWorld::resolveCollisionWithRotationAndFriction(const Collision::Manifold& contact, float delta) {

    PhysicsBody* bodyA = contact.bodyA;
    PhysicsBody* bodyB = contact.bodyB;

    sf::Vector2f normal = contact.normal;
    sf::Vector2f contact1 = contact.contact1;
    sf::Vector2f contact2 = contact.contact2;

    unsigned int contactCount = contact.contactCount;

    float e = std::min(bodyA->getRestitution(), bodyB->getRestitution());

    float sf = (bodyA->getStaticFriction() * bodyB->getStaticFriction()) / 2.f;
    float df = (bodyA->getDynamicFriction() * bodyB->getDynamicFriction()) / 2.f;

    // all static so we dont allocate them every function call
    static std::array<sf::Vector2f, 2> contactList{};
    static std::array<sf::Vector2f, 2> impulseList{};
    static std::array<sf::Vector2f, 2> frictionImpulseList{};
    static std::array<sf::Vector2f, 2> raList{};
    static std::array<sf::Vector2f, 2> rbList{};
    static std::array<float, 2> jList{};

    contactList[0] = contact1;
    contactList[1] = contact2;
    for (unsigned int i{ 0 }; i < 2; i++) {
        frictionImpulseList[i] = zeroVector;
        impulseList[i] = zeroVector;
        jList[i] = 0.f;
    }


    // first 2 loops
    for (unsigned int i{ 0 }; i < contactCount; i++) {
        sf::Vector2f ra = contactList[i] - bodyA->getPosition();
        sf::Vector2f rb = contactList[i] - bodyB->getPosition();

        raList[i] = ra;
        rbList[i] = rb;

        sf::Vector2f raPerp = { -ra.y,ra.x };
        sf::Vector2f rbPerp = { -rb.y,rb.x };

        sf::Vector2f angularLinearVelocityA = raPerp * bodyA->getAngularVelocity();
        sf::Vector2f angularLinearVelocityB = rbPerp * bodyB->getAngularVelocity();

        sf::Vector2f relativeVelocity =
            (bodyB->getLinearVelocity() + angularLinearVelocityB) -
            (bodyA->getLinearVelocity() + angularLinearVelocityA);

        float contactVelocityMag = relativeVelocity.dot(normal);

        // if already moveing apart not necessary 
        // negative dot product means pointing the opposite way
        if (contactVelocityMag > 0.f)
            continue;

        float raPerpDotN = raPerp.dot(normal);
        float rbPerpDotN = rbPerp.dot(normal);

        float denom = bodyA->getInverseMass() + bodyB->getInverseMass() +
            (raPerpDotN * raPerpDotN) * bodyA->getInverseInertia() +
            (rbPerpDotN * rbPerpDotN) * bodyB->getInverseInertia();

        float j = -(1.f + e) * contactVelocityMag;
        j /= denom;
        j /= static_cast<float>(contactCount);

        jList[i] = j;

        sf::Vector2f impulse = j * normal;
        impulseList[i] = impulse;
    }

    // NOW WE CALCULATE THE FRICTION
    for (unsigned int i{ 0 }; i < contactCount; i++) {
        sf::Vector2f impulse = impulseList[i];


        // ANGULAR VELOCITY ON A IS NORMAL AND B IS NEGATIVE BECAUSE ROTATION WORKS DIFFERENTLY IN SFML
        bodyA->addLinearVelocity(-impulse * bodyA->getInverseMass());
        bodyA->addAngularVelocity(impulse.cross(raList[i]) * bodyA->getInverseInertia());
        bodyB->addLinearVelocity(impulse * bodyB->getInverseMass());
        bodyB->addAngularVelocity(-impulse.cross(rbList[i]) * bodyB->getInverseInertia());
    }

    for (unsigned int i{ 0 }; i < contactCount; i++) {
        sf::Vector2f ra = contactList[i] - bodyA->getPosition();
        sf::Vector2f rb = contactList[i] - bodyB->getPosition();

        raList[i] = ra;
        rbList[i] = rb;

        sf::Vector2f raPerp = { -ra.y,ra.x };
        sf::Vector2f rbPerp = { -rb.y,rb.x };

        sf::Vector2f angularLinearVelocityA = raPerp * bodyA->getAngularVelocity();
        sf::Vector2f angularLinearVelocityB = rbPerp * bodyB->getAngularVelocity();

        sf::Vector2f relativeVelocity =
            (bodyB->getLinearVelocity() + angularLinearVelocityB) -
            (bodyA->getLinearVelocity() + angularLinearVelocityA);

        sf::Vector2f tangent = relativeVelocity - relativeVelocity.dot(normal) * normal;

        if (PhysicsMath::nearlyEqual(tangent, zeroVector)) {
            continue;
        }
        else {
            tangent = tangent.normalized();
        }

        float raPerpDotT = raPerp.dot(tangent);
        float rbPerpDotT = rbPerp.dot(tangent);

        float denom = bodyA->getInverseMass() + bodyB->getInverseMass() +
            (raPerpDotT * raPerpDotT) * bodyA->getInverseInertia() +
            (rbPerpDotT * rbPerpDotT) * bodyB->getInverseInertia();

        float jt = -relativeVelocity.dot(tangent);
        jt /= denom;
        jt /= static_cast<float>(contactCount);

        float j = jList[i];
        sf::Vector2f frictionImpulse;
        if (std::abs(jt) <= j * sf) {
            frictionImpulse = jt * tangent;
        }
        else {
            frictionImpulse = -j * tangent;
        }
        frictionImpulseList[i] = frictionImpulse;
    }

    for (unsigned int i{ 0 }; i < contactCount; i++) {
        sf::Vector2f frictionImpulse = frictionImpulseList[i];

        // ANGULAR VELOCITY ON A IS NORMAL AND B IS NEGATIVE BECAUSE ROTATION WORKS DIFFERENTLY IN SFML
        bodyA->addLinearVelocity(-frictionImpulse * bodyA->getInverseMass());
        bodyA->addAngularVelocity(frictionImpulse.cross(raList[i]) * bodyA->getInverseInertia());
        bodyB->addLinearVelocity(frictionImpulse * bodyB->getInverseMass());
        bodyB->addAngularVelocity(-frictionImpulse.cross(rbList[i]) * bodyB->getInverseInertia());
    }
    
    //dampenVelocity(bodyA);
    //dampenVelocity(bodyB);

    sf::Vector2f impulseTotal = impulseList[0] +  frictionImpulseList[0];
    if (contactCount > 1) {
        impulseTotal += impulseList[1] + frictionImpulseList[1];
        impulseTotal /= 4.f;
    }
    else {
        impulseTotal /= 2.f;
    }

    return impulseTotal;
}

void PhysicsWorld::dampenVelocity(PhysicsBody* body)
{
    // damping to Make stacking blocks doable in the engine
#define damping
#ifdef damping


    // rotational friction
    float rotationalFrictionCoefficient = 0.0009f;
    if (body->getAngularVelocity() > 0.f) {
        body->addAngularVelocity(-rotationalFrictionCoefficient);
    }
    else {
        body->addAngularVelocity(rotationalFrictionCoefficient);
    }

    // stop small rotational movement
    float angularVelocityThreshold = 0.01f;
    if (std::abs(body->getAngularVelocity()) < angularVelocityThreshold) body->addAngularVelocity(-body->getAngularVelocity());


    
    // stop small movement due to innacurate floating point values
    float linearFrictionCoefficient = 0.0002f;
    if (body->getLinearVelocity().x > 0.f) {
        body->addLinearVelocity({ -linearFrictionCoefficient,0.f });
    }
    else {
        body->addLinearVelocity({  linearFrictionCoefficient,0.f });
    }
    // y coordinate 
    if (body->getLinearVelocity().y > 0.f) {
        body->addLinearVelocity({ 0.f,-linearFrictionCoefficient });
    }
    else {
        body->addLinearVelocity({ 0.f,linearFrictionCoefficient });
    }

    // stop small linear movement
    float linearVelocityThreshold = 0.3f;
    if (std::abs(body->getLinearVelocity().length()) < linearVelocityThreshold) body->addLinearVelocity(-body->getLinearVelocity());
    

    #endif
}


void PhysicsWorld::checkOutOfBounds()
{
    // OUT OF BOUNDS CHECK
    float bottomScreen = worldSize.y * -0.5f;

    // removing out of bounds objects
    for (unsigned int i{ 0 }; i < objectList.size(); i++) {
        PhysicsBody* body = objectList[i];
        if (body->getIsStatic())
            continue;
        if (body->getAABB().max.y < bottomScreen) {
            removeBody(body);
            i--;
        }
    }
    // removing out of bounds enemies
    for (unsigned int i{ 0 }; i < enemyList.size(); i++) {
        PhysicsBody* body = enemyList[i];
        if (body->getIsStatic())
            continue;
        if (body->getAABB().max.y < bottomScreen) {
            removeBody(body);
            i--;
        }
    }
    // removing out of bounds playerobjects
    for (unsigned int i{ 0 }; i < playerList.size(); i++) {
        PhysicsBody* body = playerList[i];
        if (body->getIsStatic())
            continue;
        if (body->getAABB().max.y < bottomScreen) {
            removeBody(body);
            i--;
        }
    }
}
void PhysicsWorld::setLevelNameAndId(const std::string& name, unsigned int id)
{
    this->levelId = id;
    this->levelNameText->setString(name);
    this->levelNameText->setOrigin({ levelNameText->getGlobalBounds().size.x / 2.f, 0.f });
}

void PhysicsWorld::setPlayerLives(int lives)
{
    this->playerLives = lives;
    if(playerLivesText)
        playerLivesText->setString(std::to_string(lives));
}


/* old stuff

    // rotational friction
    float rotationalFrictionCoefficient = 0.001f;
    if (body->getAngularVelocity() > 0.f) {
        body->addAngularVelocity(-rotationalFrictionCoefficient);
    }
    else {
        body->addAngularVelocity(rotationalFrictionCoefficient);
    }

    // stop small rotational movement
    float angularVelocityThreshold = 0.08f;
    if (std::abs(body->getAngularVelocity()) < angularVelocityThreshold) body->addAngularVelocity(-body->getAngularVelocity());



    // stop small movement due to innacurate floating point values
    float linearFrictionCoefficient = 0.01f;
    if (body->getLinearVelocity().x > 0.f) {
        body->addLinearVelocity({ -linearFrictionCoefficient,0.f });
    }
    else {
        body->addLinearVelocity({  linearFrictionCoefficient,0.f });
    }
    // y coordinate
    if (body->getLinearVelocity().y > 0.f) {
        body->addLinearVelocity({ 0.f,-linearFrictionCoefficient });
    }
    else {
        body->addLinearVelocity({ 0.f,linearFrictionCoefficient });
    }

    // stop small linear movement
    float linearVelocityThreshold = 0.5f;
    if (std::abs(body->getLinearVelocity().length()) < linearVelocityThreshold) body->addLinearVelocity(-body->getLinearVelocity());
    
    
    
    
    // TEMP 2
    // rotational friction
    float rotationalFrictionCoefficient = 0.0001f;
    if (body->getAngularVelocity() > 0.f) {
        body->addAngularVelocity(-rotationalFrictionCoefficient);
    }
    else {
        body->addAngularVelocity(rotationalFrictionCoefficient);
    }

    // stop small rotational movement
    float angularVelocityThreshold = 0.01f;
    if (std::abs(body->getAngularVelocity()) < angularVelocityThreshold) body->addAngularVelocity(-body->getAngularVelocity());


    
    // stop small movement due to innacurate floating point values
    float linearFrictionCoefficient = 0.0002f;
    if (body->getLinearVelocity().x > 0.f) {
        body->addLinearVelocity({ -linearFrictionCoefficient,0.f });
    }
    else {
        body->addLinearVelocity({  linearFrictionCoefficient,0.f });
    }
    // y coordinate 
    if (body->getLinearVelocity().y > 0.f) {
        body->addLinearVelocity({ 0.f,-linearFrictionCoefficient });
    }
    else {
        body->addLinearVelocity({ 0.f,linearFrictionCoefficient });
    }

    // stop small linear movement
    float linearVelocityThreshold = 0.3f;
    if (std::abs(body->getLinearVelocity().length()) < linearVelocityThreshold) body->addLinearVelocity(-body->getLinearVelocity());
    */