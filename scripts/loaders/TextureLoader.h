#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <iostream>


namespace TextureLoader {


	static std::map<std::string, sf::Texture*> textures;

	sf::Texture* LoadTexture(std::string textureLocation);
}