#pragma once

// mostly used for sounds
enum class MaterialType {
	None,
	Wood,
	Stone,
	Ice,
	Player,
	Enemy,
	Ground
};


struct Material {
	float density;
	float restitution;
	float dynamicFriction;
	float staticFriction;
};



// density
// restitution
// dynamic friction
// static friction

namespace Materials {
	extern const Material Wood;
	extern const Material Stone;
	extern const Material Ice;
	extern const Material Player;
	extern const Material Enemy;
	extern const Material Ground;

	const Material& getMaterial(MaterialType mType);
}