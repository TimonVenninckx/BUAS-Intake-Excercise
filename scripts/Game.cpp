#include "Game.h"
#include <iostream>
#include <sstream>
#include "loaders/FontLoader.h"

#define DebuggingNot
    
// we do (1 / timebetweenPhysicsStep) physics update every second
constexpr float timeBetweenPhysicsStep = 0.001f;
                    // worldsize  ,  screensize
Game::Game() :world{ {192.f,108.f},{ 1600.f, 900.f } }
{
    // set in constructor aswell
    viewSize = { 192.f, 108.f };

    window = sf::RenderWindow(sf::VideoMode({ 1600, 900 }), "Buas Intake!");

    window.setPosition({ 10, 40 });

    view = sf::View({ 0.f , viewSize.y * 0.5f }, { viewSize.x, -viewSize.y });
    uiView = sf::View(static_cast<sf::Vector2f>(window.getSize()) / 2.f, static_cast<sf::Vector2f>(window.getSize()));

    window.setView(view);

    // SETUP TEXT
    frameCounterText = new sf::Text(*FontLoader::getFont("font.otf"), "FrameCounter", 30);
    frameCounterText->setPosition({ uiView.getSize().x, 0.f });
    frameCounterText->setStyle(sf::Text::Bold);
    frameCounterText->setFillColor(sf::Color::Red);
    frameCounterText->setOrigin({ frameCounterText->getLocalBounds().size.x , 0.f });
    
    //window.setFramerateLimit(60);

    clock.restart();

    this->window.setVerticalSyncEnabled(true);
    //this->window.setFramerateLimit(30);

    while (window.isOpen())
    {
        try {
            float delta = clock.restart().asSeconds();
            if(delta < 0.1f)
                update(delta );
            draw();
        }
        catch (std::string s) {
            std::cout << "CAUGHT ERROR |||" << s << "\t|||\n";
        }
    }
}




void Game::update(float delta)
{
    //constexpr int iterations = 128;

    // update mous position 
    sf::Vector2i pixelPos;
    sf::Vector2f mouseLocation;
    sf::Vector2f mouseLocationInUI;
    pixelPos = sf::Mouse::getPosition(window);
    mouseLocation = window.mapPixelToCoords(pixelPos, view);
    mouseLocationInUI = window.mapPixelToCoords(pixelPos, uiView);

    if (inLevel)
        this->world.update(delta, mouseLocation);


    while (const std::optional<sf::Event> event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();
        else if (inLevel) {
            if (this->world.exitLevel(event, mouseLocation,mouseLocationInUI)) {
                inLevel = false;
            }
        }
        else { // IF WE ARE NOT IN A LEVEL
            if (const sf::Event::MouseButtonReleased* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>()) { // mouse button pressed
                if (mouseButtonReleased->button == sf::Mouse::Button::Left) {
                    if (const Level::LevelInfo* info = this->mainMenu.ChooseLevel(mouseLocationInUI)) {
                        std::cout << "WE CHOSE A LEVEL\n";
                        Level::LoadLevel(world, *info, viewSize);
                        inLevel = true;
                    }
                }
            }
        }
    }

    acumulatedPhysicsDelta += delta;
    while (acumulatedPhysicsDelta >= timeBetweenPhysicsStep) {
        acumulatedPhysicsDelta -= timeBetweenPhysicsStep;
        world.step(timeBetweenPhysicsStep);
    }


    int fps = static_cast<int>(1.f / delta);
    frameCounterText->setString(std::to_string(fps));
    frameCounterText->setOrigin({ frameCounterText->getLocalBounds().size.x , 0.f });
}

// internal physics timer
// limit physics to certain amount of times per frame

void Game::draw()
{
    window.clear();
    
    if (inLevel) {
        world.draw(this->window);
    }
    // ui drawing
    window.setView(uiView);
    if(inLevel)
        world.drawUI(this->window);
    else {
        this->mainMenu.draw(window);
    }
    window.draw(static_cast<sf::Text>(*frameCounterText));


    // setting view back to normal for mouse calculations
    window.setView(view);
    window.display();
}


Game::~Game()
{
    
    delete frameCounterText;

    auto& textures = TextureLoader::textures;

    while (!textures.empty()) {
        std::cout << "Unloaded Texture\n";
        delete textures.begin()->second;
        textures.erase(textures.begin()->first);
    }

    auto& fonts = FontLoader::allFonts;
    while (!fonts.empty()) {
        std::cout << "Unloaded Font\n";
        delete fonts.begin()->second;
        fonts.erase(textures.begin()->first);
    }

    auto& sounds = AudioLoader::allSounds;
    while (!sounds.empty()) {
        std::cout << "Unloaded Font\n";
        delete sounds.begin()->second;
        fonts.erase(sounds.begin()->first);
    }


}
