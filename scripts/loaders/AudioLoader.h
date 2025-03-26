#pragma once

#include <SFML/Audio.hpp>
#include <map>

namespace AudioLoader
{
	static std::map<std::string, sf::SoundBuffer*> allSounds;
	sf::SoundBuffer* getAudio(std::string fileLocation);
};

