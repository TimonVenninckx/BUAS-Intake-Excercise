#include "PhysicsWorld.h"
#include <iostream>
#include "AABB.h"
#include "PhysicsMath.h"


const float PhysicsWorld::minBodySize{ 0.01f * 0.01f };
const float PhysicsWorld::maxBodySize{ 64.f * 64.f };

const float PhysicsWorld::minDensity = 0.5f; // g/cm^3
const float PhysicsWorld::maxDensity = 21.4f;


const unsigned int PhysicsWorld::minIterations = 1;
const unsigned int PhysicsWorld::maxIterations = 128;


PhysicsWorld::PhysicsWorld()
{
	this->gravity = sf::Vector2f(0.f, -9.81f);
    this->contactPointsList.setPrimitiveType(sf::PrimitiveType::Triangles);
    this->contactDirectionList.setPrimitiveType(sf::PrimitiveType::Lines);
}


void PhysicsWorld::addBody(PhysicsBody* body)
{
    if (body)
	    this->bodylist.push_back(body);
}



bool PhysicsWorld::removeBody(PhysicsBody* body)
{
    auto result = std::find_if(this->bodylist.begin(), this->bodylist.end(),
        [body](PhysicsBody* c) { return c == body; });

	if (result == bodylist.end())
		return false;
    delete (*result);
	this->bodylist.erase(result);
	return true;
}

PhysicsBody* PhysicsWorld::getBody(unsigned int index)
{
	if (index >= bodylist.size() || index < 0)
		return nullptr;

	return bodylist[index];
}



void PhysicsWorld::step(float time,unsigned int totalIterations)
{
    totalIterations = std::clamp(totalIterations, minIterations, maxIterations);
    contactPointsList.clear();
    contactDirectionList.clear();

    for (unsigned int currentIteration{ 0 }; currentIteration < totalIterations; currentIteration++) {

        this->contactPairs.clear();
        this->stepBodies(time,totalIterations);
        this->broadPhase();
        this->narrowPhase();
    }
}


void PhysicsWorld::broadPhase() {
    for (unsigned int i{ 0 }; i < bodylist.size() - 1; i++) {
        PhysicsBody* bodyA = this->bodylist[i];
        AABB bodyA_aabb = bodyA->getAABB();

        for (unsigned int j{ i + 1 }; j < bodylist.size(); j++) {
            PhysicsBody* bodyB = this->bodylist[j];
            AABB bodyB_aabb = bodyB->getAABB();

            if (bodyA->getIsStatic() && bodyB->getIsStatic())
                continue;
            if (!Collision::IntersectAABB(bodyA_aabb, bodyB_aabb))
                continue;

            this->contactPairs.push_back({ i,j });
        }
    }
}

void PhysicsWorld::narrowPhase() {

    for (unsigned int i{ 0 }; i < this->contactPairs.size(); i++) {

        std::tuple<unsigned int, unsigned int> pair = this->contactPairs[i];
        PhysicsBody* bodyA = this->bodylist[std::get<0>(pair)];
        PhysicsBody* bodyB = this->bodylist[std::get<1>(pair)];

        Collision::HitResult result{ Collision::collide(bodyA, bodyB) };

        if (result.collided) {

            seperateBodies(bodyA, bodyB, result.normal * result.depth);

            Collision::ContactResult cr = Collision::findContactPoints(bodyA, bodyB);
            Collision::Manifold contact(bodyA, bodyB, result.normal, result.depth, cr.contact1, cr.contact2, cr.contactCount);
            this->resolveCollisionWithRotationAndFriction(contact);


            #define debugging
            #ifdef debugging
            sf::Vertex v;
            sf::Vertex l;
            v.color = sf::Color(255, 0, 0);
            l.color = sf::Color(255, 255, 0);

            v.position = (cr.contact1 + sf::Vector2f{ 0.f, 0.3f });
            this->contactPointsList.append(v);
            v.position = (cr.contact1 + sf::Vector2f{ -0.3f, -0.3f });
            this->contactPointsList.append(v);
            v.position = (cr.contact1 + sf::Vector2f{ 0.3f, -0.3f });
            this->contactPointsList.append(v);

            l.position = cr.contact1;
            this->contactDirectionList.append(l);
            l.position += result.normal * result.depth * 50.f;
            this->contactDirectionList.append(l);
            if (cr.contactCount > 1) {
                v.position = (cr.contact2 + sf::Vector2f{ 0.f, 0.3f });
                this->contactPointsList.append(v);
                v.position = (cr.contact2 + sf::Vector2f{ -0.3f, -0.3f });
                this->contactPointsList.append(v);
                v.position = (cr.contact2 + sf::Vector2f{ 0.3f, -0.3f });
                this->contactPointsList.append(v);

                l.position = cr.contact2;
                this->contactDirectionList.append(l);
                l.position += result.normal * result.depth * 50.f;
                this->contactDirectionList.append(l);
            }
            #endif
        }
    }
}

void PhysicsWorld::stepBodies(float time, int total) {

    for (unsigned int i{ 0 }; i < this->bodylist.size(); i++) {
        this->bodylist[i]->step(time, this->gravity, total);
    }
}

void PhysicsWorld::seperateBodies(PhysicsBody* bodyA, PhysicsBody* bodyB, sf::Vector2f mtv) {
    // mtv = normal * depth
    if (bodyA->getIsStatic()) {
        bodyB->move(mtv);
    }
    else if (bodyB->getIsStatic()) {
        bodyA->move(-mtv);
    }
    else {
        bodyA->move(-mtv / 2.f);
        bodyB->move(mtv / 2.f);
    }
}


void PhysicsWorld::resolveCollisionBasic(const Collision::Manifold& contact){

    //  https://www.chrishecker.com/Rigid_Body_Dynamics
    //  https://www.chrishecker.com/images/e/e7/Gdmphys3.pdf PAGE 4 FOR THE FORMULA

    sf::Vector2f relativeVelocity = contact.bodyB->linearVelocity - contact.bodyA->linearVelocity;

    // if already moveing apart not necessary 
    // negative dot product means pointing the opposite way
    if (relativeVelocity.dot(contact.normal) > 0.f)
        return;


    float e = std::min(contact.bodyA->getRestitution(), contact.bodyB->getRestitution());

    // numerator
    float j = -(1.f + e) * relativeVelocity.dot(contact.normal);
    // denominator
    j /= contact.bodyA->getInverseMass() + contact.bodyB->getInverseMass();

    sf::Vector2f impulse = j * contact.normal;

    contact.bodyA->linearVelocity -= contact.bodyA->getInverseMass() * impulse;
    contact.bodyB->linearVelocity += contact.bodyB->getInverseMass() * impulse;

}


void PhysicsWorld::resolveCollisionWithRotation(const Collision::Manifold& contact) {

    PhysicsBody* bodyA = contact.bodyA;
    PhysicsBody* bodyB = contact.bodyB;

    sf::Vector2f normal = contact.normal;
    sf::Vector2f contact1 = contact.contact1;
    sf::Vector2f contact2 = contact.contact2;

    int contactCount = contact.contactCount;

    float e = std::min(bodyA->getRestitution(), bodyB->getRestitution());

    // all static so we dont allocate them every function call
    static std::array<sf::Vector2f, 2> contactList{};
    static std::array<sf::Vector2f, 2> impulseList{};
    static std::array<sf::Vector2f, 2> raList{};
    static std::array<sf::Vector2f, 2> rbList{};
    contactList[0] = contact1;
    contactList[1] = contact2;


    for (unsigned int i{ 0 }; i < contactCount; i++) {
        sf::Vector2f ra = contactList[i] - bodyA->getPosition();
        sf::Vector2f rb = contactList[i] - bodyB->getPosition();

        raList[i] = ra;
        rbList[i] = rb;

        sf::Vector2f raPerp = { -ra.y,ra.x };
        sf::Vector2f rbPerp = { -rb.y,rb.x };

        sf::Vector2f angularLinearVelocityA = raPerp * bodyA->angularVelocity;
        sf::Vector2f angularLinearVelocityB = rbPerp * bodyB->angularVelocity;

        sf::Vector2f relativeVelocity = 
            (bodyB->linearVelocity + angularLinearVelocityB) -
            (bodyA->linearVelocity + angularLinearVelocityA);

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

        sf::Vector2f impulse = j * normal;
        impulseList[i] = impulse;
    }

    for (unsigned int i{ 0 }; i < contactCount; i++) {
        sf::Vector2f impulse = impulseList[i];
        //std::cout << "Impulse:" << impulse.x << ',' << impulse.y << '\n';

        // ANGULAR VELOCITY ON A IS NORMAL AND B IS NEGATIVE BECAUSE ROTATION WORKS DIFFERENTLY IN SFML
        bodyA->linearVelocity += bodyA->getInverseMass() * -impulse;
        bodyA->angularVelocity += (impulse.cross(raList[i]) * bodyA->getInverseInertia());
        bodyB->linearVelocity += bodyB->getInverseMass() * impulse;
        bodyB->angularVelocity += (-impulse.cross(rbList[i]) * bodyB->getInverseInertia());
    }

    //std::cout << "A angular: " << bodyA->angularVelocity << '\n';
    //std::cout << "B angular: " << bodyB->angularVelocity << '\n';
}

void PhysicsWorld::resolveCollisionWithRotationAndFriction(const Collision::Manifold& contact) {

    PhysicsBody* bodyA = contact.bodyA;
    PhysicsBody* bodyB = contact.bodyB;

    sf::Vector2f normal = contact.normal;
    sf::Vector2f contact1 = contact.contact1;
    sf::Vector2f contact2 = contact.contact2;

    int contactCount = contact.contactCount;

    float e = std::min(bodyA->getRestitution(), bodyB->getRestitution());

    float sf = (bodyA->getStaticFriction() * bodyB->getStaticFriction()) * 0.5f;
    float df = (bodyA->getDynamicFriction() * bodyB->getDynamicFriction()) * 0.5f;

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

        sf::Vector2f angularLinearVelocityA = raPerp * bodyA->angularVelocity;
        sf::Vector2f angularLinearVelocityB = rbPerp * bodyB->angularVelocity;

        sf::Vector2f relativeVelocity =
            (bodyB->linearVelocity + angularLinearVelocityB) -
            (bodyA->linearVelocity + angularLinearVelocityA);

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
        //std::cout << "Impulse:" << impulse.x << ',' << impulse.y << '\n';

        // ANGULAR VELOCITY ON A IS NORMAL AND B IS NEGATIVE BECAUSE ROTATION WORKS DIFFERENTLY IN SFML
        bodyA->linearVelocity += bodyA->getInverseMass() * -impulse;
        bodyA->angularVelocity += (impulse.cross(raList[i]) * bodyA->getInverseInertia());
        bodyB->linearVelocity += bodyB->getInverseMass() * impulse;
        bodyB->angularVelocity += (-impulse.cross(rbList[i]) * bodyB->getInverseInertia());
    }

    for (unsigned int i{ 0 }; i < contactCount; i++) {
        sf::Vector2f ra = contactList[i] - bodyA->getPosition();
        sf::Vector2f rb = contactList[i] - bodyB->getPosition();

        raList[i] = ra;
        rbList[i] = rb;

        sf::Vector2f raPerp = { -ra.y,ra.x };
        sf::Vector2f rbPerp = { -rb.y,rb.x };

        sf::Vector2f angularLinearVelocityA = raPerp * bodyA->angularVelocity;
        sf::Vector2f angularLinearVelocityB = rbPerp * bodyB->angularVelocity;

        sf::Vector2f relativeVelocity =
            (bodyB->linearVelocity + angularLinearVelocityB) -
            (bodyA->linearVelocity + angularLinearVelocityA);

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
            frictionImpulse = -j * tangent * df;
        }
        frictionImpulseList[i] = frictionImpulse;
    }

    for (unsigned int i{ 0 }; i < contactCount; i++) {
        sf::Vector2f frictionImpulse = frictionImpulseList[i];
        //std::cout << "Impulse:" << impulse.x << ',' << impulse.y << '\n';

        // ANGULAR VELOCITY ON A IS NORMAL AND B IS NEGATIVE BECAUSE ROTATION WORKS DIFFERENTLY IN SFML
        bodyA->linearVelocity += bodyA->getInverseMass() * -frictionImpulse;
        bodyA->angularVelocity += (frictionImpulse.cross(raList[i]) * bodyA->getInverseInertia());
        bodyB->linearVelocity += bodyB->getInverseMass() * frictionImpulse;
        bodyB->angularVelocity += (-frictionImpulse.cross(rbList[i]) * bodyB->getInverseInertia());
    }

    //std::cout << "A angular: " << bodyA->angularVelocity << '\n';
    //std::cout << "B angular: " << bodyB->angularVelocity << '\n';
}


unsigned int PhysicsWorld::getBodyCount() const
{
    return bodylist.size();
}






/*



        std::vector<bool> colliding;
        colliding.resize(bodylist.size());
        std::fill(colliding.begin(), colliding.end(), false);

        colliding[i] = true;
        colliding[j] = true;

        */




        // Add contact points for display (DEBUG)
        //#define debugging
