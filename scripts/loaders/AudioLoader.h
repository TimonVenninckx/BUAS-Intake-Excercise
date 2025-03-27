#pragma once
#ifndef AUDIO
#define AUDIO

#include <SFML/Audio.hpp>
#include <map>

namespace AudioLoader
{
	extern const float Volume;
	static std::map<std::string, sf::SoundBuffer*> allSounds;
	sf::SoundBuffer* getAudio(std::string fileLocation);
};

#endif