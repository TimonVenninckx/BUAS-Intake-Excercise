#include "Game.h"
#include <iostream>
#include "Collision.h"
#include "PhysicsMath.h"

Game::Game()
{
    window = sf::RenderWindow(sf::VideoMode({ 800, 400 }), "Buas Intake!");


    view = sf::View({ 0.f,0.f }, { 80.f,-40.f });
    window.setView(view);


    line[0].position = sf::Vector2f(0, 0);
    line[0].color = sf::Color::Red;
    line[1].position = sf::Vector2f(5, 5);
    line[1].color = sf::Color::Red;


    line2[0].position = sf::Vector2f(0, 0);

    std::srand(std::time({})); // use current time as seed for random generator


    int bodycount = 10;

    std::cout << "Screen size X:" << view.getSize().x << " y:" << view.getSize().y << '\n';

    sf::Vector2f margin{ 30.f,30.f };

    for (int i{ 0 }; i < bodycount; i++) {
        int type = std::rand() % 2;
        //int type = PhysicsBody::Box;

        float x = (std::rand() % static_cast<int>(view.getSize().x - margin.x) - static_cast<int>((view.getSize().x + margin.x) / 2.f)) + margin.x;
        float y = (std::rand() % static_cast<int>(-view.getSize().y - margin.y) - static_cast<int>((-view.getSize().y + margin.y) / 2.f)) + margin.y;

        std::cout << "x:" << x << " y:" << y << '\n';

        PhysicsBody* body;
        
        if (type == static_cast<int>(PhysicsBody::Circle)) {
            body = new PhysicsBody(1.f, { x,y }, 2.f, false, 1.f, PhysicsBody::Circle);
        }
        else if (type == static_cast<int>(PhysicsBody::Box)) {
            body = new PhysicsBody(2.f, 2.f, { x,y }, 2.f, false, 1.f, PhysicsBody::Box);
        }


        bodylist.push_back(body);
        if(bodylist[i])
            bodylist[i]->SetColor(sf::Color(std::rand() % 255, std::rand() % 255, std::rand() % 255));
        
    }

    clock.restart();

    while (window.isOpen())
    {
        float delta = clock.restart().asSeconds();
        Update(delta);
        Draw();
    }
}




void Game::Update(float delta)
{


    float dx = 0.f;
    float dy = 0.f;
    float rot = 0.f;
    float speed = 8.f;
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();

        
        else if (const sf::Event::KeyPressed* keypressed = event->getIf<sf::Event::KeyPressed>()) { // keypressed
            if (keypressed->scancode == sf::Keyboard::Scancode::E) {
            }
            if (keypressed->scancode == sf::Keyboard::Scan::R) {
            }
        }
    }


    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W)) { dy++; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S)) { dy--; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A)) { dx--; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D)) { dx++; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Q)) { rot++; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::E)) { rot--; }
    if (dx != 0.f || dy != 0.f) {
        sf::Vector2f direction = sf::Vector2f( dx,dy ).normalized();
        sf::Vector2f velocity = direction * speed * delta;
        if (this->bodylist[0]) {
            this->bodylist[0]->Move(velocity);
        }
    }
    if (rot != 0.f) {
        this->bodylist[0]->Rotate(rot * delta);
    }
    
    /*
    // rotate everything 
    for (unsigned int i{ 0 }; i < bodylist.size(); i++)
        bodylist[i]->Rotate(M_PI / 2.f * -delta);
    */

    std::vector<bool> colliding;
    colliding.resize(bodylist.size());
    std::fill(colliding.begin(), colliding.end(), false);

    static bool firstcollision = true;
    static bool secondcollision = true;

    for (unsigned int i{ 0 }; i < bodylist.size() - 1; i++) {
        PhysicsBody*  bodyA = this->bodylist[i];
        if (!bodyA) {
            std::cout << "BODY A = NULLPOINTER\n";
            continue; // invalid pointer!
        }

        for (unsigned int j{ i + 1 }; j < bodylist.size(); j++) {
            PhysicsBody* bodyB = this->bodylist[j];
            if (!bodyB) {
                std::cout << "BODY B = NULLPOINTER\n";
                continue; // invalid pointer!
            }
            Collision::HitResult result;

            if (bodyA->GetShapeType() == PhysicsBody::Box && bodyB->GetShapeType() == PhysicsBody::Circle) {
                result = Collision::IntersectCirclePolygon(bodyB->GetPosition(), bodyB->GetRadius(), bodyA->GetTransformedVertices());
                if (result.collided) {
                    // result.normal is flipped we push a out of b
                    bodyA->Move(result.normal * (result.depth / 2.f));
                    bodyB->Move(-result.normal * (result.depth / 2.f));
                    colliding[i] = true;
                    colliding[j] = true;
                    continue; // otherwise we would do result.collided twice
                }
            }
            else if (bodyB->GetShapeType() == PhysicsBody::Box && bodyA->GetShapeType() == PhysicsBody::Circle) {
                result = Collision::IntersectCirclePolygon(bodyA->GetPosition(), bodyA->GetRadius(), bodyB->GetTransformedVertices());
            }
            else if (bodyA->GetShapeType() == PhysicsBody::Box && bodyB->GetShapeType() == PhysicsBody::Box) {
                result = Collision::IntersectPolygons(bodyA->GetTransformedVertices(), bodyB->GetTransformedVertices());
            }
            else if (bodyA->GetShapeType() == PhysicsBody::Circle && bodyB->GetShapeType() == PhysicsBody::Circle) {
                result = Collision::IntersectCircles(bodyA->GetPosition(), bodyA->GetRadius(),
                    bodyB->GetPosition(), bodyB->GetRadius());
            }
            if (result.collided) {
                bodyA->Move(-result.normal * (result.depth / 2.f));
                bodyB->Move(result.normal * (result.depth / 2.f));

                //std::cout << "Normal Result:" << result.normal.x << ',' << result.normal.y << '\n';
                colliding[i] = true;
                colliding[j] = true;

                if (firstcollision) {
                    firstcollision = false;
                    line[1].position = sf::Vector2f(result.normal * 5.f);
                }
                else if (secondcollision) {
                    secondcollision = false;
                    line2[1].position = sf::Vector2f(result.normal * 5.f);

                }
            }
        }
    }

    // coloring
    for (unsigned int i{ 0 }; i < bodylist.size(); i++) {
        if(colliding[i])
            bodylist[i]->SetColor(sf::Color::Green);
        else
            bodylist[i]->SetColor(sf::Color::Magenta);
    }



    
    sf::Vector2i pixelPos;
    sf::Vector2f mouseLocation;
    pixelPos =  sf::Mouse::getPosition(window);
    mouseLocation = window.mapPixelToCoords(pixelPos, view);
    bodylist[1]->MoveTo(mouseLocation);
    
}


void Game::Draw()
{
    window.clear();
    
    for (unsigned int i{ 0 }; i < bodylist.size(); i++) {
        if(bodylist[i])
            bodylist[i]->Draw(window);
    }
    window.draw(line, 2, sf::PrimitiveType::Lines, sf::RenderStates::Default);
    window.draw(line2, 2, sf::PrimitiveType::Lines, sf::RenderStates::Default);

    

    window.display();
}





Game::~Game()
{
}