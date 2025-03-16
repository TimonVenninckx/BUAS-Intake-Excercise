#include "Game.h"
#include <iostream>
#include "Collision.h"
#include "PhysicsMath.h"
#include <sstream>

Game::Game()
{
    window = sf::RenderWindow(sf::VideoMode({ 800, 450 }), "Buas Intake!");

    window.setPosition({10, 40});

    sf::Vector2f viewSize{ 96.f,-54.f };
    view = sf::View({ 0.f,0.f }, viewSize);
    uiView = sf::View(static_cast<sf::Vector2f>(window.getSize()) / 2.f, static_cast<sf::Vector2f>(window.getSize()));

    window.setView(view);

    window.setFramerateLimit(30);

    float left = 0.f - viewSize.x / 2.f;
    float right = viewSize.x / 2.f;
    float bottom = 0.f - viewSize.y / 2.f;
    float top = viewSize.y / 2.f;

    if (font.openFromFile("font.otf")) {
        std::cout << "Loaded Font\n";
    }
    

    frameCounterText = new sf::Text(font, "FrameCounter", 30);
    frameCounterText->setPosition({ uiView.getSize().x, 0.f });
    frameCounterText->setStyle(sf::Text::Bold);
    frameCounterText->setFillColor(sf::Color::Red);
    frameCounterText->setOrigin({ frameCounterText->getLocalBounds().size.x , 0.f });



    worldStepTimeText = new sf::Text(font, "WorldStepTime:",30);
    worldStepTimeText->setStyle(sf::Text::Bold);
    worldStepTimeText->setPosition({ 0.f, uiView.getSize().y - 35.f });
    worldStepTimeText->setFillColor(sf::Color::Red);

    bodyCountText = new sf::Text(font, "BodyCount:", 30);
    bodyCountText->setPosition({ 0.f, 0.f });
    bodyCountText->setPosition({ 0.f, uiView.getSize().y - 70.f });
    bodyCountText->setStyle(sf::Text::Bold);
    bodyCountText->setFillColor(sf::Color::Red);




    std::srand(static_cast<unsigned int>(std::abs(std::time({})))); // use current time as seed for random generator


    std::cout << "Screen size X:" << view.getSize().x << " y:" << view.getSize().y << '\n';

    sf::Vector2f margin{ 10.f,10.f };

    float padding = std::abs(right - left) * 0.1f;

    

    // bottom bar
    PhysicsBody* body = new PhysicsBody{ right - left - padding * 2,3.f,1.f,true,0.5f };
    body->moveTo(sf::Vector2f(0.f, -12.f));
    body->setColor(sf::Color::White);
    //body->setColor(sf::Color(std::rand() % 255, std::rand() % 255, std::rand() % 255));
    world.addBody(body);

    // left ledge
    PhysicsBody* ledge1 = new PhysicsBody{ 25.f,2.f,1.f,true,0.5f };
    ledge1->moveTo(sf::Vector2f(-14.f, 5.f));
    ledge1->rotate(-M_PI * 2.f / 20.f);
    ledge1->setColor(sf::Color::White);
    //ledge1->setColor(sf::Color(std::rand() % 255, std::rand() % 255, std::rand() % 255));
    world.addBody(ledge1);

    // right ledge
    PhysicsBody* ledge2 = new PhysicsBody{ 25.f,2.f,1.f,true,0.5f };
    ledge2->moveTo(sf::Vector2f(14.f, 15.f));
    ledge2->rotate(M_PI * 0.1f);
    ledge2->setColor(sf::Color::White);
    //ledge2->setColor(sf::Color(std::rand() % 255, std::rand() % 255, std::rand() % 255));
    world.addBody(ledge2);

    //window.setFramerateLimit(30);

    clock.restart();
    sampleTimer.restart();

    while (window.isOpen())
    {
        try {
            float delta = clock.restart().asSeconds();
            update(delta);
            draw();
        }
        catch (std::string s) {
            std::cout << "CAUGHT ERROR |||" << s << "\t|||\n";
        }
    }
}




void Game::update(float delta)
{


    float dx = 0.f;
    float dy = 0.f;
    float rot = 0.f;
    // in newtons
    float forceMagnitude = 48.f;

    constexpr int iterations = 32;


    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();

        
        else if (const sf::Event::KeyPressed* keypressed = event->getIf<sf::Event::KeyPressed>()) { // keypressed
       
            if (keypressed->scancode == sf::Keyboard::Scancode::E) {
            } 
            if (keypressed->scancode == sf::Keyboard::Scancode::Z) {
            }
            if (keypressed->scancode == sf::Keyboard::Scan::B) {
                std::cout << "Bodycount: " << world.getBodyCount() << '\n';
            }
            if (keypressed->scancode == sf::Keyboard::Scan::Space) {
                for (unsigned int i{ 0 }; i < world.getBodyCount(); i++) {
                    PhysicsBody* body = world.getBody(i);

                    sf::Vector2f forceDirection = sf::Vector2f(static_cast<float>(std::rand() % 201 - 100), static_cast<float>(std::rand() % 101 - 50));
                    sf::Vector2f force = forceDirection * forceMagnitude;

                    if (body) {
                        body->addForce(force);
                    }
                }
            }
        }
        else if (const sf::Event::MouseButtonPressed* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) { // mouse button pressed
           
            // spawning 
            sf::Vector2i pixelPos;
            sf::Vector2f mouseLocation;
            pixelPos = sf::Mouse::getPosition(window);
            mouseLocation = window.mapPixelToCoords(pixelPos, view);
            
            if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                
                sf::Vector2f size;
                size.x = (std::rand() % 11) / 10.f + 2.0f;
                size.y = (std::rand() % 11) / 10.f + 2.0f;

                PhysicsBody* body = new PhysicsBody{ size.x,size.y,1.f,false,0.5f };
                body->moveTo(mouseLocation);
                body->setColor(sf::Color(std::rand() % 255, std::rand() % 255, std::rand() % 255));
                world.addBody(body);
            } 
            if (mouseButtonPressed->button == sf::Mouse::Button::Right) {

                float radius = (std::rand() % 4) / 10.f + 1.2f; // 1.2-1.5
                PhysicsBody* body = new PhysicsBody{ radius,1.f,false,0.5f };
                body->moveTo(mouseLocation);
                body->setColor(sf::Color(std::rand() % 255, std::rand() % 255, std::rand() % 255));
                world.addBody(body);
            }
        }
    }

    // view size is negative because we invert view
    // in SFML y-down = + so we negate this by making the viewsize negative
    float bottomScreen = this->view.getCenter().y - (-this->view.getSize().y * 0.5f);
    for (unsigned int i{ 0 }; i < world.getBodyCount(); i++) {
        PhysicsBody* body = world.getBody(i);
        if (body->getIsStatic())
            continue;
        if (body->getAABB().max.y < bottomScreen) {
            world.removeBody(body);
            i--;
        }
    }


    if (this->sampleTimer.getElapsedTime().asSeconds() > 1.f) {

        std::stringstream ss;
        ss << std::fixed << std::setprecision(0) << "BodyCount: " << this->totalBodyCount / static_cast<float>(this->totalSampleCount);
        this->bodyCountText->setString(ss.str());

        ss.str(std::string());
        ss.clear();
        ss << std::fixed << std::setprecision(5) << "StepTime: " << this->totalWorldStepTime / static_cast<float>(totalSampleCount);

        this->worldStepTimeText->setString(ss.str());
        
        totalWorldStepTime = 0.f;
        totalBodyCount = 0;
        totalSampleCount = 0;
        this->sampleTimer.restart();
    }

    static sf::Clock stepclock;

    stepclock.restart();
    world.step(delta,iterations);
    float stepTime = stepclock.restart().asSeconds();
    
    this->totalWorldStepTime += stepTime;
    this->totalBodyCount += this->world.getBodyCount();
    this->totalSampleCount++;

    //std::cout << "time passed since step: " << stepTime << '\n';


    int fps = static_cast<int>(1.f / delta);
    frameCounterText->setString(std::to_string(fps));
    frameCounterText->setOrigin({ frameCounterText->getLocalBounds().size.x , 0.f });

}


void Game::draw()
{
    window.clear();
    
    for (unsigned int i{ 0 }; i < world.getBodyCount(); i++) {
        PhysicsBody* body = world.getBody(i);
        if (!body)
            throw std::string("GAME::DRAW() ERROR INVALID BODY");

        body->draw(window);
    }
    window.draw(world.contactPointsList);
    window.draw(world.contactDirectionList);


    // ui drawing
    window.setView(uiView);
    window.draw(static_cast<sf::Text>(*frameCounterText));
    window.draw(static_cast<sf::Text>(*worldStepTimeText));
    window.draw(static_cast<sf::Text>(*bodyCountText));

    window.setView(view);


    window.display();
}


Game::~Game()
{
    delete frameCounterText;
    delete worldStepTimeText;
    delete bodyCountText;



}
