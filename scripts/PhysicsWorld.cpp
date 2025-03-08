#include "PhysicsWorld.h"

	const float PhysicsWorld::minBodySize{ 0.01f * 0.01f };
	const float PhysicsWorld::maxBodySize{ 64.f * 64.f };

	const float PhysicsWorld::MinDensity = 0.5f; // g/cm^3
	const float PhysicsWorld::MaxDensity = 21.4f;