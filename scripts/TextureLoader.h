#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <iostream>


namespace TextureLoader {


	static std::map<std::string, sf::Texture*> textures;

	sf::Texture* LoadTexture(std::string textureLocation) {
		if (textures.find(textureLocation) != textures.end()) {
			return textures.at(textureLocation);
		}
		sf::Texture* texture = new sf::Texture;
		if (!texture->loadFromFile("textures/" + textureLocation)) {
			std::cout << "Couldnt find texture" << '\n';
			return nullptr;
		}
		


		textures.emplace(textureLocation, texture);
		return texture;
		
	}
}