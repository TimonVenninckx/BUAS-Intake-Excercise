#include "Material.h"

namespace Materials {


	// density
	// restitution
	// dynamic friction
	// static friction
	const Material Wood{
		0.4f,
		0.4f,
		0.4f,
		0.6f
	};

	const Material Stone{
		0.6f,
		0.4f,
		0.4f,
		0.6f
	};

	const Material Ice{
		0.4f,
		1.0f,
		1.0f,
		1.0f
	};

	const Material Player{
		0.6f,
		0.4f,
		0.7f,
		1.0f
	};

	const Material Enemy{
		0.4f,
		0.4f,
		1.0f,
		1.0f
	};

	const Material Ground{
		1.0f,
		0.4f,
		0.4f,
		0.8f
	};

	const Material& getMaterial(MaterialType mType) {
		switch (mType) {
		case MaterialType::Wood: return Wood;
		case MaterialType::Stone: return Stone;
		case MaterialType::Ice: return Ice;
		case MaterialType::Player: return Player;
		case MaterialType::Enemy: return Enemy;
		case MaterialType::Ground: return Ground;
		}
	}

}