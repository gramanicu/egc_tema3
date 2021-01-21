#pragma once

#include "Colliders.hpp"

namespace GameEngine {
	class CollisionManager {
	public:
		/// <summary>
		/// Check for all collisions between an objects collider and some other colliders
		/// </summary>
		/// <param name="source">The collider of the object</param>
		/// <param name="others">An vector with the colliders of all the other objects</param>
		/// <returns>An array with the id's of all the objects this one collided with</returns>
		static std::vector<int> getCollisions(const Collider& source, std::vector<Collider*> others);
	private:
		CollisionManager();
		static bool isCollision(const Collider& a, const Collider& b);
	};
}